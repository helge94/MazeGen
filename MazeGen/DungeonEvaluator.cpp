#include "stdafx.h"
#include <fstream>
#include "DungeonEvaluator.h"
#include <iostream>


typedef std::pair<int, int> Vector2D;

DungeonEvaluator::DungeonEvaluator(std::vector<float>* runs, bool buck)
{
	if (buck)
		m_Buck = true;
	else
		m_BSP = true;

	if (buck)
		m_BuckRuntime = *runs;
	else
		m_BSPRuntime = *runs;
}

DungeonEvaluator::DungeonEvaluator(Tilemap* dung, bool buck, double* runtime)
{
	m_Dungeon = dung;
	m_Runtime = *runtime;
	m_AllTiles = *dung;
	m_Buck = buck;
	m_Reliable = reliability();
	Size();

	m_Results = Result(m_Density, m_RelativeSize, m_Runtime, m_Reliable);

	std::cout << std::endl;
	std::cout << "End of evaluation" << std::endl;
	std::cout << std::endl;

}



DungeonEvaluator::~DungeonEvaluator()
{
}

bool DungeonEvaluator::reliability()
{
	Tilemap t = *m_Dungeon;
	std::map<Vector2D, int> m_CheckedTiles;
	std::vector<Tile> m_FloorTiles;
	std::vector<sf::Vector2i> markedTiles;
	for (int i = 0; i < t.size(); i++) {
		for (int j = 0; j < t[i].size(); j++) {
			if (t[j][i].m_Type == tileFloor || t[j][i].m_Type == roomFloor || t[j][i].m_Type == connectedFloor || t[j][i].m_Type == roomWall) {
				m_FloorTiles.push_back(t[j][i]);
				Vector2D temp = std::make_pair(j, i);
				m_CheckedTiles[temp] = 1;
				continue;
			}
		}
	}

	markedTiles.push_back(m_FloorTiles[rand() % m_FloorTiles.size()].pos);

	while (markedTiles.size() > 0) {
		sf::Vector2i curr = markedTiles.front();

		for (int i = North; i <= East; i++) {
			sf::Vector2i v = curr + dirArr[i];
			Vector2D t = std::make_pair(v.y, v.x);
			if (CheckTile(v)) {
				if (m_CheckedTiles[t] == 1) {
					markedTiles.push_back(v);
					m_CheckedTiles[t] = 2;
				}
			}
		}
		markedTiles.erase(markedTiles.begin());
	}

	if (m_FloorTiles.size() != m_CheckedTiles.size()) {
		std::cout << "Failed!" << std::endl;
		return false;
	}
	for(int i = 0; i < m_FloorTiles.size(); i++){
		sf::Vector2i v = m_FloorTiles[i].pos;
		Vector2D t = std::make_pair(v.y, v.x);
		if (m_CheckedTiles[t] != 2) {
			std::cout << "Failed!" << std::endl;
			return false;
		}
	}
	std::cout << "Success!" << std::endl;
	return true;

}

bool DungeonEvaluator::CheckTile(sf::Vector2i pos)
{
	Type t = m_AllTiles[pos.y][pos.x].m_Type;
	if (t == tileFloor || t == roomFloor || t == connectedFloor || t == roomWall) return true;
	return false;
}

void DungeonEvaluator::Size()
{
	Tilemap t = *m_Dungeon;

	int TotalTiles = t.size() * t[0].size();
	int FloorTiles = 0, CorridorTiles = 0, RoomTiles = 0;
	for (int i = 0; i < t.size(); i++) {
		for (int j = 0; j < t[i].size(); j++) {
			if (t[j][i].m_Type == tileFloor || t[j][i].m_Type == roomFloor || t[j][i].m_Type == connectedFloor || t[j][i].m_Type == roomWall) {
				FloorTiles++;
				if (t[j][i].m_Type == tileFloor || t[j][i].m_Type == connectedFloor)
					CorridorTiles++;
				else
					RoomTiles++;
			}
		}
	}
	m_CorridorTiles = CorridorTiles;
	m_RoomTiles = RoomTiles;
	m_RelativeSize = m_CorridorTiles / (double)FloorTiles;
	m_Density = (double)FloorTiles / (double)TotalTiles;


	std::ofstream m_RuntimeFile, m_DensityFile, m_RelativSizeFile;
	std::string type, size, name;

	if (m_Buck) type = "Bucks_";
	else type = "BSP_";

	size = std::to_string(m_Dungeon->size());
	name = type + size + "_";
	m_RuntimeFile = std::ofstream("ResultMap/" + name + "Runtime.txt", std::ofstream::out | std::ofstream::app);
	m_DensityFile = std::ofstream("ResultMap/" + name + "Density.txt", std::ofstream::out | std::ofstream::app);
	m_RelativSizeFile = std::ofstream("ResultMap/" + name + "RelativeSize.txt", std::ofstream::out | std::ofstream::app);

	m_RuntimeFile << std::to_string(m_Runtime) << std::endl;
	m_DensityFile << std::to_string(m_Density) << std::endl;
	m_RelativSizeFile << std::to_string(m_RelativeSize) << std::endl;

	std::cout << "Densitet: " << m_Density << std::endl;
	std::cout << "Antal gångtiles: " << m_CorridorTiles << std::endl;
	std::cout << "Antal rumtiles: " << m_RoomTiles << std::endl;
	std::cout << "Relativ storlek: " << m_RelativeSize << std::endl;
}
