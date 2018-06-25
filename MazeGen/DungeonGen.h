#include "Tile.h"
#include <vector>
#include <set>
#pragma once

typedef std::pair<int, int> Vector2D;
class DungeonGen
{
public:
	DungeonGen(int c, int r, sf::RenderWindow* window, float w, float h, int room, bool multi, int winding);
	void Draw();
	Tilemap GetTilemap() { return m_Tiles; }
	~DungeonGen();
	double runtime;
private:
	void AddRooms();
	void CreRegions();
	void CreateTilemap();
	void CreateMaze(Tile start);
	void RemoveDeadEnds();
	bool CanCarve(Tile tile, sf::Vector2i dir);

	int m_Col, m_Row, m_Regions, m_MazeStartRegion, m_Winding;
	float m_Height, m_Width;
	bool m_MultiRoom;

	std::vector<sf::Vector2i> m_Neighbours;
	Tilemap m_Tiles;
	std::vector<sf::RectangleShape> m_Rooms;
	std::map<int, Vector2D> m_RemovableTiles;

	sf::RenderWindow* m_Window;
	sf::Vector2f m_TileSize;
};

