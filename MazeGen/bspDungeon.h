#include "Tile.h"
#include <iostream>
#include <vector>
#include <set>
#pragma once
static int MinRoom = 5;
static int MaxRoom = 9;
static sf::Vector2i dirArr[4] = {
	sf::Vector2i(0, -1),
	sf::Vector2i(0, 1),
	sf::Vector2i(-1, 0),
	sf::Vector2i(1, 0)
};

enum Directions {
	m_North, m_South, m_West, m_East, m_None
};


struct Leaf {
	Leaf(){}
	Leaf(int X, int Y, int Width, int Height)
	{
		x = X;
		y = Y;
		h = Height;
		w = Width;
	}
	Leaf* GetSibling(Leaf* child) {
		if (child == m_RightChild)
			return m_LeftChild;
		else
			return m_RightChild;
	}

	void GetMergedRoomPoint(std::vector<std::pair<Leaf*, sf::Vector2i>>& p) {
		if (m_LeftChild)
			m_LeftChild->GetMergedRoomPoint(p);
		if (m_RightChild)
			m_RightChild->GetMergedRoomPoint(p);
		if (!m_RightChild && !m_LeftChild) {
			p.push_back(std::pair<Leaf*, sf::Vector2i > (this, sf::Vector2i(m_Room.left, m_Room.top)));
		}
	}
	sf::Vector2i GetRoomPoint() {
		int x = rand() % m_Room.width;
		int y = rand() % m_Room.height;
		return sf::Vector2i(x + m_Room.left, y + m_Room.top);
	}
	sf::Vector2i GetSiblingRoomPoint(Leaf* l) {
		std::vector<sf::Vector2i> points;
		for (int i = m_Room.top; i < m_Room.top + m_Room.height; i++) {
			if (i == (l->m_Room.top - 1) || i == (l->m_Room.top + l->m_Room.height)) continue;
			for (int j = m_Room.left; j < m_Room.left + m_Room.width; j++) {
				if (j == (l->m_Room.left - 1) || j == (l->m_Room.left + l->m_Room.width)) continue;
				points.push_back(sf::Vector2i(j, i));
			}			
		}
		return points[rand() % points.size()];
	}
	Directions GetDirection(Directions& dir)
	{
		if (dir == m_South) return(m_North);
		else if (dir == m_North) return(m_South);
		else if (dir == m_West) return(m_East);
		else if (dir == m_East) return(m_West);
	}
	void ComparePoints(sf::Vector2i& curr, sf::Vector2i& sibling) {
		m_Dir.clear();
		if (curr.y > sibling.y)			m_Dir.push_back(m_North);
		else if (curr.y < sibling.y)	m_Dir.push_back(m_South);
		if (curr.x > sibling.x)			m_Dir.push_back(m_West);
		else if (curr.x < sibling.x)	m_Dir.push_back(m_East);

	}

	int y, x, h, w;
	int m_Region;
	Leaf* m_LeftChild = nullptr;
	Leaf* m_RightChild = nullptr;
	Leaf* m_Parent = nullptr;
	sf::IntRect m_RoomArea;
	sf::IntRect m_Room;
	std::vector<sf::Vector2i> m_Hallway;
	std::vector<sf::Vector2i> m_MergedArea;
	std::vector<Directions> m_Dir;
};

class bspDungeon
{
public:
	bspDungeon(int c, int r, sf::RenderWindow* window, float w, float h, bool split, bool threshold);
	void Draw();
	Tilemap GetTilemap() { return m_Tiles; }
	~bspDungeon();
	double runtime;
private:
	void CreateTilemap();
	void AddRoomAreas(Leaf* l);
	void AddRooms(Leaf* l);
	void Split(Leaf* l);
	void ConnectAreas(Leaf* l);
	void Connect(Leaf* l);
	void MergeAreas(Leaf* l, bool hall);
	void Clear(Leaf* l);
	bool ComparePos(sf::Vector2i& p, sf::IntRect& r);
	bool CheckDirections(Directions& dir, sf::Vector2i p, Leaf* l);
	bool CheckCompletion(sf::Vector2i& p, Leaf* l);

	Directions CheckPoint(sf::Vector2i& point, Leaf* l, int i);
	std::vector<std::pair<Leaf*, sf::Vector2i>> GetClosestRoom(std::vector<std::pair<Leaf*, sf::Vector2i>>& l, std::vector<std::pair<Leaf*, sf::Vector2i>>& sibling);

	Leaf* m_Tree = nullptr;
	bool m_Split;
	int Counter = 0;
	int m_Col, m_Row, m_Regions;
	float m_Height, m_Width, m_Threshold;

	Tilemap m_Tiles;

	sf::RenderWindow* m_Window;
	sf::Vector2f m_TileSize;
};

