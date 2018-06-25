#include "stdafx.h"
#include <iostream>
#include <chrono>
#include "bspDungeon.h"


bspDungeon::bspDungeon(int c, int r, sf::RenderWindow* window, float w, float h, bool split, bool threshold) :
	m_Window(window), m_Col(c), m_Row(r), m_Width(w), m_Height(h), m_TileSize(sf::Vector2f(m_Width / r, m_Height / c)), m_Split(split), m_Threshold(threshold)
{


	auto start = std::chrono::high_resolution_clock::now();

	m_Tree = new Leaf(0, 0, m_Col - 1, m_Row - 1);
	m_Regions = 0;
	m_Tree->m_Region = m_Regions++;

	m_WallShape = sf::RectangleShape(m_TileSize);
	m_WallShape.setOutlineThickness(1.f);
	m_WallShape.setOutlineColor(m_OutlineColor);
	m_WallShape.setFillColor(m_WallColor);


	CreateTilemap();
	Split(m_Tree);
	AddRoomAreas(m_Tree);
	Connect(m_Tree);
	ConnectAreas(m_Tree);

	auto finish = std::chrono::high_resolution_clock::now();
	runtime = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count() / 1000000000.f;
	Clear(m_Tree);
}

void bspDungeon::Clear(Leaf * l)
{
	if (l->m_RightChild)
		Clear(l->m_RightChild);
	if (l->m_LeftChild)
		Clear(l->m_LeftChild);
	delete l;
}

bool bspDungeon::ComparePos(sf::Vector2i& p, sf::IntRect& r)
{
	if (p.x >= r.left && p.x <= r.left + r.width - 1 && p.y >= r.top && p.y <= r.top + r.height - 1) return true;
	return false;
}

void bspDungeon::CreateTilemap()
{
	for (int i = 0; i < m_Col; i++) {
		std::vector<Tile> col;
		for (int j = 0; j < m_Row; j++) {
			Tile tile(sf::Vector2i(j, i));
			tile.m_Type = none;
			tile.m_Region = -1;
			col.push_back(tile);
		}
		m_Tiles.push_back(col);
	}
}

void bspDungeon::AddRooms(Leaf * l)
{
	int temp;
	int w = ((temp = rand() % l->m_RoomArea.width) < (MinRoom - 1) ? (MinRoom - 1) : temp);
	int h = ((temp = rand() % l->m_RoomArea.height) < (MinRoom - 1) ? (MinRoom - 1) : temp);
	if (w > MaxRoom) w = MaxRoom;
	if (h > MaxRoom) h = MaxRoom;
	int posx = ((l->m_RoomArea.width - w) <= 0 ? 0 : rand() % (l->m_RoomArea.width - w + 1));
	int posy = ((l->m_RoomArea.height - h) <= 0 ? 0 : rand() % (l->m_RoomArea.height - h + 1));
	l->m_Room = sf::IntRect(l->m_RoomArea.left + posx, l->m_RoomArea.top + posy, w, h);

	for (int i = l->m_Room.left; i < l->m_Room.left + l->m_Room.width; i++) {
		for (int j = l->m_Room.top; j < l->m_Room.top + l->m_Room.height; j++) {
			m_Tiles[j][i].m_Type = roomFloor;
			m_Tiles[j][i].m_Region = l->m_Region;
		}
	}
}

void bspDungeon::AddRoomAreas(Leaf * l)
{
	if (l->m_LeftChild)
		AddRoomAreas(l->m_LeftChild);
	if (l->m_RightChild)
		AddRoomAreas(l->m_RightChild);
	if (!l->m_RightChild && !l->m_LeftChild) {
		l->m_RoomArea = sf::IntRect(l->x + 1, l->y + 1, l->w - 1, l->h - 1);
		AddRooms(l);
	}
}

void bspDungeon::Split(Leaf* l)
{
	if (l->m_LeftChild || l->m_RightChild) return;
	bool m_SplitH = rand() % 2 > 0;
	
	if (m_Split) {
		if (l->w > l->h && (double)l->w / (double)l->h >= m_Threshold)
			m_SplitH = false;
		else if (l->h > l->w && (double)l->h / (double)l->w >= m_Threshold)
			m_SplitH = true;		
	}
	int size = (m_SplitH ? l->h : l->w) - MinRoom;
	if (size <= MinRoom) return;

	int pos = rand() % (size - MinRoom) + MinRoom;

	l->m_LeftChild = (m_SplitH ? new Leaf(l->x, l->y, l->w, pos) : new Leaf(l->x, l->y, pos, l->h));
	l->m_RightChild = (m_SplitH ? new Leaf(l->x, l->y + pos, l->w, l->h - pos) : new Leaf(l->x + pos, l->y, l->w - pos, l->h));

	l->m_LeftChild->m_Parent = l;
	l->m_LeftChild->m_Region = m_Regions++;
	l->m_RightChild->m_Parent = l;
	l->m_RightChild->m_Region = m_Regions++;

	Split(l->m_LeftChild);
	Split(l->m_RightChild);
}

void bspDungeon::ConnectAreas(Leaf * l)
{
	if (!l->m_LeftChild || !l->m_RightChild) return;
	if (l->m_LeftChild->m_Region != l->m_Region) ConnectAreas(l->m_LeftChild);
	if (l->m_RightChild->m_Region != l->m_Region) ConnectAreas(l->m_RightChild);
	if (l == m_Tree) return;
	if (l->m_LeftChild->m_Region == l->m_RightChild->m_Region) {
		Leaf* sibling = l->m_Parent->GetSibling(l);
		if (sibling->m_LeftChild || sibling->m_RightChild)
			if (!(sibling->m_LeftChild->m_Region == sibling->m_RightChild->m_Region)) return;

		std::vector<std::pair<Leaf*, sf::Vector2i>> point1, point2;
		l->GetMergedRoomPoint(point1);
		sibling->GetMergedRoomPoint(point2);

		std::vector<std::pair<Leaf*, sf::Vector2i>> points = GetClosestRoom(point1, point2);
		sf::Vector2i p1, p2;
		p1 = points[0].first->GetRoomPoint();
		p2 = points[1].first->GetRoomPoint();
		l->ComparePoints(p1, p2);
		int LeftOrRight = rand() % 2;

		while (true) {
			l->ComparePoints(p1, p2);
			
			std::vector<int> temp;
			Directions dir1;
			for (int i = 0; i < l->m_Dir.size(); i++) {
				sf::Vector2i p = p1 + dirArr[l->m_Dir[i]];
				if (m_Tiles[p.y][p.x].m_Region == l->m_Region) temp.push_back(i);
			}
			if (temp.size() > 0) dir1 = l->m_Dir[temp[rand() % temp.size()]];
			else dir1 = CheckPoint(p1, l, LeftOrRight);
			p1 += dirArr[dir1];
			if (m_Tiles[p1.y][p1.x].m_Type != roomFloor) l->m_Hallway.push_back(sf::Vector2i(p1.x, p1.y));
			if (CheckCompletion(p1, l)) break;

		}
		MergeAreas(l, true);
		MergeAreas(sibling, false);

	}

}

void bspDungeon::Connect(Leaf * l)
{
	if (l->m_LeftChild) Connect(l->m_LeftChild);
	if (l->m_RightChild) Connect(l->m_RightChild);
	if (!l->m_RightChild || !l->m_LeftChild) {
		if (l->m_Parent->GetSibling(l)->m_LeftChild || l->m_Parent->GetSibling(l)->m_RightChild) return;
		if (l->m_Region == l->m_Parent->m_Region) return;
		Leaf* sibling = l->m_Parent->GetSibling(l);
		sf::Vector2i p1 = l->GetRoomPoint();
		sf::Vector2i p2 = sibling->GetSiblingRoomPoint(l);
		int LeftOrRight = rand() % 2;
	
		while (true) {
			l->ComparePoints(p1, p2);			
			Directions dir1 = CheckPoint(p1, l, LeftOrRight);
			p1 += dirArr[dir1];
			if (m_Tiles[p1.y][p1.x].m_Type != roomFloor) l->m_Hallway.push_back(sf::Vector2i(p1.x, p1.y));
			if (CheckCompletion(p1, l)) break;
		}
		MergeAreas(l, true);
		MergeAreas(sibling, false);
	}
}

void bspDungeon::MergeAreas(Leaf * l, bool hall)
{
	l->m_Region = l->m_Parent->m_Region;

	if (l->m_MergedArea.empty()) {
		for (int i = l->m_Room.top; i < l->m_Room.top + l->m_Room.height; i++) {
			for (int j = l->m_Room.left; j < l->m_Room.left + l->m_Room.width; j++) {
				l->m_Parent->m_MergedArea.push_back(sf::Vector2i(j, i));
				m_Tiles[i][j].m_Region = l->m_Region;
			}
		}
	}
	else {
		for (int i = 0; i < l->m_MergedArea.size(); i++) {
			l->m_Parent->m_MergedArea.push_back(sf::Vector2i(l->m_MergedArea[i].x, l->m_MergedArea[i].y));
			m_Tiles[l->m_MergedArea[i].y][l->m_MergedArea[i].x].m_Region = l->m_Region;
		}
	}

	if (hall) {
		for (int i = 0; i < l->m_Hallway.size(); i++) {
			l->m_Parent->m_MergedArea.push_back(l->m_Hallway[i]);
			m_Tiles[l->m_Hallway[i].y][l->m_Hallway[i].x].m_Region = l->m_Region;
			m_Tiles[l->m_Hallway[i].y][l->m_Hallway[i].x].m_Type = tileFloor;

		}
	}
}



bool bspDungeon::CheckDirections(Directions& dir, sf::Vector2i p, Leaf* l)
{
	Directions revDir = l->GetDirection(dir);
	for (int i = m_North; i <= m_East; i++) {
		if (i == revDir) continue;
		sf::Vector2i checkPoint = p + dirArr[i];

		if(!ComparePos(checkPoint, l->m_Room))
			if (m_Tiles[checkPoint.y][checkPoint.x].m_Region == l->m_Region) return false;

		if (m_Tiles[checkPoint.y][checkPoint.x].m_Region != -1)
			if (m_Tiles[checkPoint.y][checkPoint.x].m_Region != l->m_Parent->GetSibling(l)->m_Region) return false;	
	}

	return true;
}

bool bspDungeon::CheckCompletion(sf::Vector2i& p, Leaf * l)
{
	for (int i = m_North; i <= m_East; i++) {
		sf::Vector2i checkPoint = p + dirArr[i];
		if (m_Tiles[checkPoint.y][checkPoint.x].m_Region == l->m_Parent->GetSibling(l)->m_Region) return true;
	}
	return false;
}


Directions bspDungeon::CheckPoint(sf::Vector2i& point, Leaf * l, int i)
{
	/*std::vector<int> temp;
	for (int i = 0; i < l->m_Dir.size(); i++) {
		sf::Vector2i p = point + dirArr[l->m_Dir[i]];
		if (m_Tiles[p.y][p.x].m_Region == l->m_Region)
			temp.push_back(i);
	}
	if (temp.size() > 0)
		return l->m_Dir[temp[rand() % temp.size()]];
*/
	int value = rand() % l->m_Dir.size();
	if (l->m_Dir.size() > 1) value = i;
	if (CheckDirections(l->m_Dir[value], point + dirArr[l->m_Dir[value]], l)) return l->m_Dir[value];
	if (l->m_Dir.size() < 2) return l->m_Dir[value];
	//return l->m_Dir[i];
	if (value == 0) return l->m_Dir[1];
	return l->m_Dir[0];

}

std::vector<std::pair<Leaf*, sf::Vector2i>> bspDungeon::GetClosestRoom(std::vector<std::pair<Leaf*, sf::Vector2i>>& l, std::vector<std::pair<Leaf*, sf::Vector2i>>& sibling)
{
	std::pair<Leaf*, sf::Vector2i> p1 = l[0];
	std::pair<Leaf*, sf::Vector2i> p2 = sibling[0];

	for (int i = 0; i < l.size(); i++) {
		for (int j = 0; j < sibling.size(); j++) {
			int x = std::abs(l[i].second.x - sibling[j].second.x);
			int y = std::abs(l[i].second.y - sibling[j].second.y);
			int xCurr = std::abs(p1.second.x - p2.second.x);
			int yCurr = std::abs(p1.second.y - p2.second.y);
			if ((x + y) < (xCurr + yCurr)) {
				p1 = l[i];
				p2 = sibling[j];
			}
		}
	}
	return std::vector<std::pair<Leaf*, sf::Vector2i>>{p1, p2};
}

bspDungeon::~bspDungeon()
{
}

void bspDungeon::Draw()
{
	for (int i = 0; i < m_Tiles.size(); i++) {
		for (int j = 0; j < m_Tiles[i].size(); j++) {
			if (m_Tiles[i][j].m_Type == tileFloor) {
				m_Tiles[i][j].SetShape(m_WallShape);
				m_Tiles[i][j].GetShape().setPosition((m_Tiles[i][j].pos.x * m_TileSize.x) + 15,
					(m_Tiles[i][j].pos.y * m_TileSize.y) + 15);
				m_Tiles[i][j].GetShape().setFillColor(sf::Color::Black);
				m_Tiles[i][j].Draw(*m_Window);
			}
			else if (m_Tiles[i][j].m_Type == connectedFloor) {
				m_Tiles[i][j].SetShape(m_WallShape);
				m_Tiles[i][j].GetShape().setPosition((m_Tiles[i][j].pos.x * m_TileSize.x) + 15,
					(m_Tiles[i][j].pos.y * m_TileSize.y) + 15);
				m_Tiles[i][j].GetShape().setFillColor(sf::Color::Black);
				m_Tiles[i][j].Draw(*m_Window);
			}
			else if (m_Tiles[i][j].m_Type == connectionFloor) {
				m_Tiles[i][j].SetShape(m_WallShape);
				m_Tiles[i][j].GetShape().setPosition((m_Tiles[i][j].pos.x * m_TileSize.x) + 15,
					(m_Tiles[i][j].pos.y * m_TileSize.y) + 15);
				m_Tiles[i][j].GetShape().setFillColor(sf::Color::Black);
				m_Tiles[i][j].Draw(*m_Window);
			}
			else if (m_Tiles[i][j].m_Type == roomFloor) {
				m_Tiles[i][j].SetShape(m_WallShape);
				m_Tiles[i][j].GetShape().setPosition((m_Tiles[i][j].pos.x * m_TileSize.x) + 15,
					(m_Tiles[i][j].pos.y * m_TileSize.y) + 15);
				m_Tiles[i][j].GetShape().setFillColor(sf::Color::Black);
				m_Tiles[i][j].Draw(*m_Window);
			}
			else if (m_Tiles[i][j].m_Type == roomWall) {
				m_Tiles[i][j].SetShape(m_WallShape);
				m_Tiles[i][j].GetShape().setPosition((m_Tiles[i][j].pos.x * m_TileSize.x) + 15,
					(m_Tiles[i][j].pos.y * m_TileSize.y) + 15);
				m_Tiles[i][j].GetShape().setFillColor(sf::Color::Black);
				m_Tiles[i][j].Draw(*m_Window);
			}
			

		}
	}
}