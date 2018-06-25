#include "stdafx.h"
#include "Block.h"
#include "Time.h"


Block::Block(int c, int r, int s, sf::RenderWindow * window) :
	m_Col(c), m_Row(r), m_Size(s), m_Window(window)
{
	sf::Vector2f m_BlockPos = sf::Vector2f(s * c, s * r);
	m_BlockShape = sf::RectangleShape(sf::Vector2f(s, s));
	m_BlockShape.setPosition(m_BlockPos.x, m_BlockPos.y);
	m_BlockShape.setFillColor(m_BlockColor);

	m_LeftShape = sf::RectangleShape(sf::Vector2f(m_WallWidth, s));
	m_LeftShape.setPosition(m_BlockPos.x, m_BlockPos.y);
	m_LeftShape.setFillColor(m_WallColor);

	m_RightShape = sf::RectangleShape(sf::Vector2f(m_WallWidth, s));
	m_RightShape.setPosition(m_BlockPos.x + s - m_WallWidth, m_BlockPos.y);
	m_RightShape.setFillColor(m_WallColor);

	m_TopShape = sf::RectangleShape(sf::Vector2f(s, m_WallWidth));
	m_TopShape.setPosition(m_BlockPos.x, m_BlockPos.y);
	m_TopShape.setFillColor(m_WallColor);

	m_BotShape = sf::RectangleShape(sf::Vector2f(s, m_WallWidth));
	m_BotShape.setPosition(m_BlockPos.x, m_BlockPos.y + s - m_WallWidth);
	m_BotShape.setFillColor(m_WallColor);

}

Block::~Block()
{
}

void Block::Draw()
{
	if (m_IsStart)
		SetColor(m_StartColor);

	else if (m_IsEnd)
		SetColor(m_EndColor);

	else if (m_InStack)
		SetColor(m_InStackColor);

	else if (m_IsVisited)
		SetColor(m_VisitedColor);

	m_Window->draw(m_BlockShape);

	if (m_Left)
		m_Window->draw(m_LeftShape);
	
	if (m_Right)
		m_Window->draw(m_RightShape);

	if (m_Top)
		m_Window->draw(m_TopShape);
	
	if (m_Bot)
		m_Window->draw(m_BotShape);
}

void Block::SetColor(sf::Color color)
{
	if (m_BlockShape.getFillColor() != color)
		m_BlockShape.setFillColor(color);
}
