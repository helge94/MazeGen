#include "Tile.h"
#pragma once

struct Result {
	Result(){}
	Result(double density, double relativesize, float runtime, bool reliable) {
		m_Density = density;
		m_RelativeSize = relativesize;
		m_Runtime = runtime;
		m_Reliability = reliable;
	}
	double m_Density;
	double m_RelativeSize;
	float m_Runtime;
	bool m_Reliability;

};
namespace {
	static sf::Vector2i dirArr[4] = {
		sf::Vector2i(0, -1),
		sf::Vector2i(0, 1),
		sf::Vector2i(-1, 0),
		sf::Vector2i(1, 0)
	};

	enum m_Directions {
		North, South, West, East, None
	};
}
class DungeonEvaluator
{
public:
	DungeonEvaluator(std::vector<float>* runs, bool buck);
	DungeonEvaluator(Tilemap* dung, bool buck, double* runtime);
	Result GetResults() { return m_Results; }
	~DungeonEvaluator();
private:
	bool reliability();
	bool CheckTile(sf::Vector2i pos);
	bool m_Buck = false, m_BSP = false, m_Reliable = false;
	void Size();
	
	Result m_Results;
	double m_Density, m_RoomTiles, m_CorridorTiles, m_RelativeSize;
	float m_Runtime;
	Tilemap* m_Dungeon;
	std::vector<float> m_BuckRuntime;
	std::vector<float> m_BSPRuntime;
	Tilemap m_AllTiles;
};

