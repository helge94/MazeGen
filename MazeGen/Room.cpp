#include "stdafx.h"
#include "Room.h"
#include <iostream>
#include <time.h>


Room::Room(int n, sf::Vector2u w, int c, int r, sf::RenderWindow* window) :
	m_NumberOfTries(n), m_WindowSize(w), m_Cols(c), m_Rows(r), m_Window(window)
{
	//Basstorleken = fönsterstorleken på x delat med antalet kolumner * 5
	//Så att ett rum kan vara exempelvis 1.2 i storlek
	m_RoomBaseSize = m_WindowSize.x / (m_Cols * 5);
	srand(time(NULL));
	AddRooms();
}


Room::~Room()
{
}

void Room::Draw()
{
	for (int i = 0; i < m_Rooms.size(); i++) {

		m_Window->draw(*m_Rooms[i]);
	}

}

void Room::AddRooms()
{
	for (int i = 0; i < m_NumberOfTries; i++) {

		int height	= rand() % 60 + 25;
		int width	= rand() % 60 + 25;
		
		sf::RectangleShape* room = new sf::RectangleShape(sf::Vector2f(height * m_RoomBaseSize,
			width * m_RoomBaseSize));
		bool overlaps = false;

		float xpos, ypos;
		int x, y;
		x = m_WindowSize.x - room->getSize().x + 0;
		y = m_WindowSize.y - room->getSize().y + 0;
		xpos = rand() % x;
		ypos = rand() % y;

		room->setPosition(xpos, ypos);

		for (int i = 0; i < m_Rooms.size(); i++) {
			sf::FloatRect temp = m_Rooms[i]->getGlobalBounds();
			temp.left	-= m_WindowSize.x / 100;
			temp.top	-= m_WindowSize.x / 100;
			temp.width	+= m_WindowSize.x / 50;
			temp.height += m_WindowSize.x / 50;

			if (room->getGlobalBounds().intersects(temp)) {
				overlaps = true;
				break;
			}
		}
		if (overlaps) continue;

		SetColor(*room, rand() % 5);
		m_Rooms.push_back(room);
	}

	std::cout << m_Rooms.size() << std::endl;
}

void Room::SetColor(sf::RectangleShape & room, int val)
{
	switch (val) {
	case 0:
		room.setFillColor(m_StartColor);
		break;
	case 1:
		room.setFillColor(m_EndColor);
		break;
	case 2:
		room.setFillColor(m_InStackColor);
		break;
	case 3:
		room.setFillColor(m_VisitedColor);
		break;
	case 4:
		room.setFillColor(m_BlockColor);
		break;
	}
}
