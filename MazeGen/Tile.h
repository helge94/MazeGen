#include "SFML/Graphics.hpp"
#pragma once
namespace {
	sf::RectangleShape m_WallShape, m_RoomShape;
	sf::Color m_RoomColor = sf::Color(0, 0, 255, 255);
	sf::Color m_TempColor = sf::Color(255, 255, 255, 160);
	sf::Color m_WallColor = sf::Color(128, 128, 128, 255);
	sf::Color m_OutlineColor = sf::Color(128, 128, 128, 128);
}

enum Type {
	roomFloor, none, tileFloor, lame, connectionFloor, connectedFloor, roomWall
};
struct Tile {
	Tile() {
	}
	Tile(sf::Vector2i p) {
		pos = p;
		m_Type = none;
		m_Region = -1;
	}
	void Draw(sf::RenderWindow& wind) {
		wind.draw(m_Shape);
	}
	void SetShape(sf::RectangleShape& shape) {
		m_Shape = shape;
	}

	Type m_Type;
	int m_Region;
	sf::RectangleShape& GetShape() { return m_Shape; }
	sf::RectangleShape m_Shape;
	sf::Vector2i pos;
};


typedef std::vector<std::vector<Tile>> Tilemap;
