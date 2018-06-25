#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  
#include "stdafx.h"
#include <chrono>
#include <iterator>
#include "DungeonGen.h"
#include <time.h>
#include <iostream>
#include <unordered_set>


DungeonGen::DungeonGen(int c, int r, sf::RenderWindow* window, float w, float h, int room, bool multi, int winding) :
	m_Window(window), m_Col(c), m_Row(r), m_Width(w), m_Height(h), m_MultiRoom(multi), m_Winding(winding),
	m_TileSize(sf::Vector2f(m_Width/r, m_Height/c))
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	m_Regions = -1;

	auto start = std::chrono::high_resolution_clock::now();

	m_WallShape = sf::RectangleShape(m_TileSize);
	m_WallShape.setOutlineThickness(1.f);
	m_WallShape.setOutlineColor(m_OutlineColor);
	m_WallShape.setFillColor(m_WallColor);

	m_Neighbours.push_back(sf::Vector2i(0, 1));
	m_Neighbours.push_back(sf::Vector2i(0, -1));
	m_Neighbours.push_back(sf::Vector2i(1, 0));
	m_Neighbours.push_back(sf::Vector2i(-1, 0));

	CreateTilemap();

	for(int i = 0; i < room; i++)
		AddRooms();

	m_MazeStartRegion = m_Regions + 1;

	for (int i = 1; i < m_Col; i += 2) {
		for (int j = 1; j < m_Row; j += 2) {
			if (m_Tiles[i][j].m_Type != none) continue;
				CreateMaze(m_Tiles[i][j]);
		}
	}

	CreRegions();
	RemoveDeadEnds();
	auto finish = std::chrono::high_resolution_clock::now();
	runtime = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count() / 1000000000.f;
	
}

void DungeonGen::CreateMaze(Tile start)
{
	m_Regions++;
	std::vector<Tile> cells;
	sf::Vector2i lastDir = sf::Vector2i(0, 0);
	start.m_Region = m_Regions;
	cells.push_back(start);

	while (cells.size() > 0) {
		Tile cell = cells.back();
		std::vector<sf::Vector2i> adjacentCells;

		for (int i = 0; i < m_Neighbours.size(); i++) {
			if (CanCarve(cell, m_Neighbours[i])) adjacentCells.push_back(m_Neighbours[i]);
		}
		if (adjacentCells.size() > 0) {
			sf::Vector2i dir;
			for (int i = 0; i < adjacentCells.size(); i++) {
				if (adjacentCells[i] == lastDir) {
					int temp = rand() % 100;
					if (temp < m_Winding) dir = lastDir;
					break;
				}
				else if (i == adjacentCells.size() - 1)
					dir = adjacentCells[rand() % adjacentCells.size()];				
			}
			cell.pos += dir;
			m_Tiles[cell.pos.y][cell.pos.x].m_Type = tileFloor;
			m_Tiles[cell.pos.y][cell.pos.x].m_Region = m_Regions;
			m_RemovableTiles[m_RemovableTiles.size()] = std::make_pair(cell.pos.y, cell.pos.x);
			cell.pos += dir;
			m_Tiles[cell.pos.y][cell.pos.x].m_Type = tileFloor;
			m_Tiles[cell.pos.y][cell.pos.x].m_Region = m_Regions;
			m_RemovableTiles[m_RemovableTiles.size()] = std::make_pair(cell.pos.y, cell.pos.x);

			cells.push_back(m_Tiles[cell.pos.y][cell.pos.x]);
			lastDir = dir;
			
		}
		else {
			cells.pop_back();
			lastDir = sf::Vector2i(0, 0);
		}
		
	}

}

void DungeonGen::RemoveDeadEnds()
{

	bool done = false;
	while (!done) {
		done = true;
		std::map<int, Vector2D>::iterator it;
		for (it = m_RemovableTiles.begin(); it != m_RemovableTiles.end(); it) {
			Vector2D tempVec = it->second;
			sf::Vector2i pos = sf::Vector2i(tempVec.first, tempVec.second);

			int exits = 0;
			for (int i = 0; i < m_Neighbours.size(); i++) {
				sf::Vector2i temp = (pos + m_Neighbours[i]);
				if ((m_Tiles[temp.x][temp.y].m_Type != none) && (m_Tiles[temp.x][temp.y].m_Type != connectionFloor)) exits++;
			}
			if (exits != 1) {
				it++;
				continue;
			}
			done = false;

			m_Tiles[pos.x][pos.y].m_Type = none;
			m_Tiles[pos.x][pos.y].m_Region = -1;
			m_RemovableTiles.erase(it++);
		}

	}

}

bool DungeonGen::CanCarve(Tile tile, sf::Vector2i dir)
{
	sf::Vector2i temp = tile.pos + dir * 3;
	sf::FloatRect worldBounds = sf::FloatRect(0,0, m_Row, m_Col);
	if (!worldBounds.contains(temp.x, temp.y))
		return false;
	
	temp = tile.pos + dir * 2;

	if (m_Tiles[temp.y][temp.x].m_Type != none)
		return false;

	for (int i = 0; i < m_Neighbours.size(); i++) {
		if (m_Neighbours[i] == dir || m_Neighbours[i] == -dir) continue;
		if (m_Tiles[temp.y + m_Neighbours[i].y][temp.x + m_Neighbours[i].x].m_Type != none)
			return false;
	}

	return true;


}

void DungeonGen::CreRegions() {
	std::map<Vector2D, std::vector<std::pair<int, int>>> m_MapRegionTiles;
	std::map<Vector2D, std::vector<std::pair<int, int>>> m_MazeTiles;



	//Hittar connection tiles, sparar dessa i m_MapRegionTiles
	for (int ix = 0; ix < m_Tiles.size(); ix++) {
		for (int iy = 0; iy < m_Tiles[ix].size(); iy++) {
			sf::Vector2i pos = sf::Vector2i(ix, iy);
			Vector2D RegionTiles;
			//Endast oanv�nda "none" tiles ska kollas
			if (m_Tiles[ix][iy].m_Type != none) continue;

			//Lista �ver alla regioner som den h�r tilen gr�nsar till
			std::unordered_set<int> regions;
			//G�r igenom alla fyra grannpositioner
			for (int i = 0; i < m_Neighbours.size(); i++) {
				//H�mtar grannens index
				sf::Vector2i index = (pos + m_Neighbours[i]);

				sf::FloatRect worldBounds = sf::FloatRect(0, 0, m_Row, m_Col);
				//Vissa grannar kan vara utanf�r v�r dungeon, exempelvis om nuvarande pos �r 25, 25 s� �r grannen 1, 0 och 0, 1 utanf�r v�r dungeon
				if (!worldBounds.contains(index.x, index.y)) continue;

				//H�mtar regionen p� nuvarande grannen
				int region = m_Tiles[index.x][index.y].m_Region;

				//Om grannens region inte �r -1 (-1 �r startv�rdet som endast oanv�nda tiles fortfarande har)
				if (region != -1) regions.insert(region);

			}
			//Om noden gr�nsar till mer �n tv� regioner kan inte en anslutning skapas �nnu
			if (regions.size() < 2) continue;
			//H�mtar f�rsta regionen
			int x = *std::next(regions.begin(), 0);
			//H�mtar andra regionen
			int y = *std::next(regions.begin(), 1);

			//Om x �r st�rre �n y s� ska de paras y, x
			if (x > y) RegionTiles = std::make_pair(y, x);
			//Annars paras de x, y. En unordered_set inneh�ller endast unika element, d�rmed kommer x och y aldrig vara likadana.
			else RegionTiles = std::make_pair(x, y);

			//S�tter v�r nuvarande tile till connection tile
			m_Tiles[pos.x][pos.y].m_Type = connectionFloor;
			//Trycker in tilen i m_MapRegionTiles
			m_MapRegionTiles[RegionTiles].push_back(std::make_pair(pos.x, pos.y));
			//Extra map f�r att hantera MazeRegions -> RoomRegions delen
			if (RegionTiles.second >= m_MazeStartRegion)
				m_MazeTiles[RegionTiles].push_back(std::make_pair(pos.x, pos.y));

		}
	}


	std::map<int, int> merged;
	std::unordered_set<int> openRegions;

	//S�tter alla regioner som omergade och �ppna
	for (int i = 0; i <= m_Regions; i++) {
		merged[i] = i;
		openRegions.insert(i);
	}

	//Tills det att inga fler regioner �r �ppna samt att det finns connection tiles kvar
	while ((openRegions.size() > 1) && (m_MapRegionTiles.size() > 0)) {
		Vector2D region;
			
		//Om det f�r finnas flera v�gar ifr�n varje rum
		if (m_MultiRoom) {
			//Slumpa fram ett index ifr�n connection tiles
			int index = rand() % m_MapRegionTiles.size(); 
			int i = 0;
			//G� igenom alla connection tiles, kan inte anv�nda random access d� [] operator efters�ker nyckel och inte position
			for (auto const& x : m_MapRegionTiles) {
				
				if (i < index) {
					i++;
					continue;
				}
				//Sparar ner regionerna som ska mergas i region
				region = x.first;
				break;
			}
		}
		else {
			//Om mazeTiles inte �r tom
			if (m_MazeTiles.size() > 0)
				//h�mta f�rsta elementet i mazetiles och ta bort det ur mazetiles
				for (auto const& x : m_MazeTiles) {
					//Sparar ner regionerna som ska mergas i region
					region = x.first;
					//Ta bort mazetiles
					m_MazeTiles.erase(region);
					//Avbryt
					break;
				}
			//Om mazetiles �r tom
			else
				//G� igenom mapregiontiles
				for (auto const& x : m_MapRegionTiles) {
					//Ta f�rsta elementet
					region = x.first;
					//Avbryt
					break;
				}
		}

		//Vilken region vi ska till
		int dest = region.first;
		//Vilken region vi mergar med regionen vi ska till
		int source = region.second;

		//Om vi till�ter flera connections
		if (m_MultiRoom) {
			for (int i = 0; i < merged.size(); i++) {
				if (source == merged[i]) {
					merged[i] = merged[dest];
					break;
				}
			}
		}
		//Om vi inte till�ter flera connection
		else {
			//Om region dest och region source redan �r mergade tas dessa bort
			if (merged[dest] == merged[source]) {
				m_MapRegionTiles.erase(region);
				continue;
			}
			//S�tter merge till v�rdet av merged[source], merged h�ller koll p� vilken region andra regioner �r efter merge, exempelvis om vi mergar region 1 och 8 kommer merged[1] vara 8
			int merge = merged[source];
			//G�r igenom hela merged listan vilket �r alla regioner
			for (int i = 0; i < merged.size(); i++) {
				//Om v�rdet p� merged[source] �r samma som merged[i] s�tts merged[i] till merged[dest], finns till f�r att om exempelvis region 1, 2, 3 mergats och alla merged[1], [2] och [3] = 1 s� m�ste dessa uppdateras
				if (merge == merged[i])
					merged[i] = merged[dest];
			}
		}
		//H�mtar alla m�jliga connectors som ansluter de tv� regionerna
		std::vector<Vector2D> connectors = m_MapRegionTiles[region];
		//Tar en slumpad connector av alla m�jliga connectors
		Vector2D randConnector = connectors[rand() % connectors.size()];
		//S�tter randConnectorns typ till connnectedFloor
		m_Tiles[randConnector.first][randConnector.second].m_Type = connectedFloor;
		m_RemovableTiles[m_RemovableTiles.size()] = randConnector;


		//G�r igenom openREgions
		std::unordered_set<int>::const_iterator got = openRegions.find(source);
		if(!(got == openRegions.end()))
			openRegions.erase(got);

		//Tar bort region ur m_MapRegionTiles
		m_MapRegionTiles.erase(region);

	}



}
void DungeonGen::CreateTilemap()
{
	for (int i = 0; i < m_Col; i++) {
		std::vector<Tile> col;
		for (int j = 0; j < m_Row; j++) {
			Tile tile(sf::Vector2i(j, i));
			col.push_back(tile);
		}
		m_Tiles.push_back(col);
	}
}

void DungeonGen::AddRooms()
{
	//Minsta storleken �r 5 - st�rsta 9, % 3 + 2 = 2-4, * 2 + 1 = 5(2*2+1)-9(4*2+1)
	int xSize = (rand() % 3 + 2) * 2 + 1, ySize = (rand() % 3 + 2) * 2 + 1;
	//Hittar position, m�ste vara oj�mn och inom kartans gr�nser (oj�mn f�r att kunna skapa v�gar mellan rum och labyrint
	//D�rf�r tas f�rst m_row - xSize vilket allts� �r bredden p� hela kartan - bredden p� rummet / 2
	//Och sedan multipliceras detta med 2 och sedan adderas 1 vilket g�r att rummet alltid placeras p� en oj�mn position
	int x = rand() % (int)((m_Row - xSize) * 0.5) * 2 + 1;
	int y = rand() % (int)((m_Col - ySize) * 0.5) * 2 + 1;	

	if (x + xSize > m_Row - 1 || y + ySize > m_Col - 1)
		return;

	sf::Vector2f size(xSize * m_TileSize.x, ySize * m_TileSize.y);

	sf::RectangleShape room(size);
	room.setPosition(x * m_TileSize.x, y * m_TileSize.y);


	for (int i = 0; i < m_Rooms.size(); i++) {
		if (room.getGlobalBounds().intersects(sf::FloatRect(m_Rooms[i].getGlobalBounds())))
			return;
	}
	m_Regions++;
	for (int j = y; j < (y + ySize); j++) {
		for (int i = x; i < (x + xSize); i++) {
			if (i == x || j == y || i == (x + xSize - 1) || j == (y + ySize - 1))
				m_Tiles[j][i].m_Type = roomWall;
			else
				m_Tiles[j][i].m_Type = roomFloor;

			m_Tiles[j][i].m_Region = m_Regions;
		}
	}


	room.setFillColor(m_RoomColor);
	m_Rooms.push_back(room);

}

void DungeonGen::Draw()
{

	sf::RectangleShape outlining(sf::Vector2f(m_Width + 15, m_Height + 15));
	outlining.setFillColor(sf::Color::Transparent);
	outlining.setOutlineThickness(5);
	outlining.setOutlineColor(sf::Color::White);
	outlining.setPosition(5, 5);
	//m_Window->draw(outlining);

	for (int i = 0; i < m_Tiles.size(); i++) {
		for (int j = 0; j < m_Tiles[i].size(); j++) {
			if (m_Tiles[i][j].m_Type == tileFloor) {
				m_Tiles[i][j].SetShape(m_WallShape);
				m_Tiles[i][j].GetShape().setPosition((m_Tiles[i][j].pos.x * m_TileSize.x) + 15, 
					(m_Tiles[i][j].pos.y * m_TileSize.y) + 15);
				m_Tiles[i][j].GetShape().setFillColor(sf::Color::Black);
				m_Tiles[i][j].Draw(*m_Window);

			}
			/*
			else if (m_Tiles[i][j].m_Type == connectionFloor) {
				m_Tiles[i][j].SetShape(m_WallShape);
				m_Tiles[i][j].GetShape().setPosition(m_Tiles[i][j].pos.x * m_TileSize.x, m_Tiles[i][j].pos.y * m_TileSize.y);
				m_Tiles[i][j].GetShape().setFillColor(sf::Color::Cyan);
				m_Tiles[i][j].Draw(*m_Window);
			}*/
			else if (m_Tiles[i][j].m_Type == connectedFloor) {
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


DungeonGen::~DungeonGen()
{
}

