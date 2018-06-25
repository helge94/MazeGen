#include "stdafx.h"
#include "SFML/Graphics.hpp"
#include <vector>
#pragma once
class Room
{
public:
	Room(int n, sf::Vector2u w, int c, int r, sf::RenderWindow* window);
	~Room();
	void Draw();

private:

	void AddRooms();
	void SetColor(sf::RectangleShape& room, int val);

	sf::Color m_BlockColor		= sf::Color(128, 128, 0, 255);
	sf::Color m_VisitedColor	= sf::Color(192, 192, 192, 255);
	sf::Color m_InStackColor	= sf::Color(255, 0, 255, 255);
	sf::Color m_StartColor		= sf::Color(0, 128, 128, 255);
	sf::Color m_EndColor		= sf::Color(48, 25, 61, 255);
	
	sf::RenderWindow* m_Window;
	sf::Vector2u m_WindowSize;
	std::vector<sf::RectangleShape*> m_Rooms;
	int m_NumberOfTries;
	int m_Cols, m_Rows, m_RoomBaseSize;
};

