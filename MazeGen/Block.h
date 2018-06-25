#include "SFML/Graphics.hpp"

#pragma once
class Block
{
public:
	Block(int c, int r, int s, sf::RenderWindow* window);
	~Block();

	bool m_IsVisited = false, m_InStack = false, m_IsStart = false, m_IsEnd = false;
	bool m_Left = true, m_Right = true, m_Top = true, m_Bot = true;

	void Draw();

	int GetCol() const { return m_Col; }
	int GetRow() const { return m_Row; }

	void SetColor(sf::Color color);

private:
	int m_Col, m_Row;
	int m_Size;
	int m_WallWidth = 2;

	sf::Color m_BlockColor = sf::Color(0, 0, 0, 255);
	sf::Color m_WallColor = sf::Color(0, 0, 0, 255);
	sf::Color m_VisitedColor = sf::Color(255, 255, 255, 255);
	sf::Color m_InStackColor = sf::Color(255, 0, 0, 255);
	sf::Color m_StartColor = sf::Color(0, 255, 0, 255);
	sf::Color m_EndColor = sf::Color(255, 0, 0, 255);


	sf::RectangleShape m_BlockShape;
	sf::RectangleShape m_RightShape, m_LeftShape, m_TopShape, m_BotShape;
	sf::RenderWindow* m_Window;
};

