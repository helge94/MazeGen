#include "stdafx.h"
#include "button.h"
#include <iostream>

const int m_RoomMultiplier = 16;

button::button(sf::Font f, sf::FloatRect r, sf::RenderWindow * w, int distance, int type) :
	m_Font(f), m_Window(w)
{
	m_TextRect = sf::RectangleShape(sf::Vector2f(r.width / 2, r.height));
	m_TextRect.setPosition(sf::Vector2f(r.left + (r.width / 2) + distance, r.top));
	m_TextRect.setOutlineColor(sf::Color::Green);
	m_TextRect.setOutlineThickness(-2.f);

	switchArray[type] = true;
	if		(GetIsRuntime())	SetText(m_RunText, "0", m_TextRect);
	else if (GetIsIndex())		SetText(m_IndexText, "0", m_TextRect);
	else if (GetIsAverage())	SetText(m_AverageText, "0", m_TextRect);
	else if (GetIsHighest())	SetText(m_HighestText, "0", m_TextRect);
	else if (GetIsLowest())		SetText(m_LowestText, "0", m_TextRect);
	
	else if (GetIsMulti()) {
		m_ButtonRect = sf::RectangleShape(sf::Vector2f(r.width / 2, r.height));
		m_ButtonRect.setPosition(sf::Vector2f(r.left + (r.width / 2) + distance, r.top));
		SetText(m_MultiText, "MultiConnections", m_TextRect);
		m_TextRect.setOutlineColor(sf::Color::Red);
	}
	else if (GetIsSplitSwitch()) {
		m_ButtonRect = sf::RectangleShape(sf::Vector2f(r.width / 2, r.height));
		m_ButtonRect.setPosition(sf::Vector2f(r.left + (r.width / 2) + distance, r.top));
		SetText(m_SplitSwitchText, "Split Switch", m_TextRect);
		m_TextRect.setOutlineColor(sf::Color::Red);
	}
}

button::button(sf::Font f, sf::FloatRect r, sf::RenderWindow * w, std::string s, int type) :
	m_Font(f), m_Window(w)
{
	m_ButtonRect = sf::RectangleShape(sf::Vector2f(r.width / 2, r.height));
	m_ButtonRect.setPosition(sf::Vector2f(r.left, r.top));
	m_ButtonRect.setOutlineColor(sf::Color::Red);
	m_ButtonRect.setOutlineThickness(-2.f);

	switchArray[type] = true;
	SetText(m_NameText, s, m_ButtonRect);
}

button::button(sf::Font f, sf::FloatRect r, sf::RenderWindow* w, int distance, std::string s, int type) :
	m_Font(f), m_Window(w)
{
	m_ButtonRect = sf::RectangleShape(sf::Vector2f(r.width / 2, r.height));
	m_ButtonRect.setPosition(sf::Vector2f(r.left, r.top));
	m_ButtonRect.setOutlineColor(sf::Color::Red);
	m_ButtonRect.setOutlineThickness(-2.f);

	m_TextRect = sf::RectangleShape(sf::Vector2f(r.width / 2, r.height));
	m_TextRect.setPosition(sf::Vector2f(r.left + (r.width / 2) + distance, r.top));
	m_TextRect.setOutlineColor(sf::Color::Green);
	m_TextRect.setOutlineThickness(-2.f);

	switchArray[type] = true;
	SetText(m_NameText, s, m_ButtonRect);
	if		(GetIsMap())		SetText(m_MapText, std::to_string(m_Mapsize) + "x" + std::to_string(m_Mapsize), m_TextRect);
	else if(GetIsRooms())		SetText(m_RoomText, std::to_string(m_RoomCount * m_RoomCount), m_TextRect);
	else if(GetIsLaps())		SetText(m_LapText, std::to_string(m_LapsCount), m_TextRect);
	else if(GetIsWinding())		SetText(m_WindingText, std::to_string(m_Winding), m_TextRect);
	else if (GetIsThreshold()) {
		std::string temp = std::to_string(m_SplitThreshold);
		temp.insert(1, ".");
		SetText(m_SplitThresholdText, temp, m_TextRect);
	}
}


void button::SetText(sf::Text& text, std::string s, sf::RectangleShape r)
{
	text.setFont(m_Font);
	//15 vid 1600:900
	text.setCharacterSize(12);
	text.setString(s);
	float height = (r.getGlobalBounds().height - text.getGlobalBounds().height) / 2;
	float width = (r.getGlobalBounds().width - text.getGlobalBounds().width) / 2;
	text.setPosition(r.getPosition() + sf::Vector2f(width, height));
	text.setFillColor(sf::Color::Black);
	text.move(0, -((height - 8) / 2));
}


void button::Draw()
{
	m_Window->draw(m_ButtonRect);
	m_Window->draw(m_TextRect);
	m_Window->draw(m_NameText);
	m_Window->draw(m_MapText);
	m_Window->draw(m_RoomText);
	m_Window->draw(m_RunText);
	m_Window->draw(m_LapText);
	m_Window->draw(m_IndexText);
	m_Window->draw(m_AverageText);
	m_Window->draw(m_LowestText);
	m_Window->draw(m_HighestText);
	m_Window->draw(m_MultiText);
	m_Window->draw(m_WindingText);
	m_Window->draw(m_SplitSwitchText);
	m_Window->draw(m_SplitThresholdText);
}

void button::SetSplit(float & split)
{
	switch (m_SplitThreshold) {
	case 200:
		m_SplitThreshold = 100;
		break;
	default:
		m_SplitThreshold += 10;
		break;
	}
	split = (float)m_SplitThreshold / 100;	
	std::string temp = std::to_string(m_SplitThreshold);
	temp.insert(1, ".");
	SetText(m_SplitThresholdText, temp, m_TextRect);
}

void button::SetMapsize(int& mapsize)
{
	if (m_Mapsize >= 500)
		switch (m_Mapsize) {
		case 1000:
			m_Mapsize = 25;
			break;
		case 125:
			m_Mapsize = 200;
			break;
		case 225:
			m_Mapsize = 250;
			break;
		case 250:
			m_Mapsize = 400;
			break;
		case 400:
			m_Mapsize = 500;
			break;
		default:
			m_Mapsize = m_Mapsize * 2;
			break;
		}
	else m_Mapsize += 25;

	SetText(m_MapText, std::to_string(m_Mapsize) + "x" + std::to_string(m_Mapsize), m_TextRect);
	mapsize = m_Mapsize;
}

void button::SetWinding(int & winding)
{
	switch (m_Winding) {
	case 100:
		m_Winding = 0;
		break;
	default:
		m_Winding += 10;
		break;
	}
	SetText(m_WindingText, std::to_string(m_Winding), m_TextRect);
	winding = m_Winding;
}

void button::SetRoomCount(int& roomcount)
{

	switch (m_RoomCount) {
	case 24:
		m_RoomCount = 29;
		break;
	case 49:
		m_RoomCount = 54;
		break;
	case 74:
		m_RoomCount = 79;
		break;
	case 83:
		m_RoomCount = 4;
		break;
	default:
		m_RoomCount += 4;
		break;
	}
	SetText(m_RoomText, std::to_string(m_RoomCount * m_RoomCount), m_TextRect);
	roomcount = m_RoomCount * m_RoomCount;
}

void button::SetLapCount(int& lapcount)
{

	switch (m_LapsCount) {
	case 1000:
		m_LapsCount = 1;
		break;
	case 1:
		m_LapsCount = 3;
		break;
	case 3:
		m_LapsCount = 5;
		break;
	case 5:
		m_LapsCount = 10;
		break;
	case 10:
		m_LapsCount = 18;
		break;
	case 18:
		m_LapsCount = 25;
		break;
	case 25:
		m_LapsCount = 50;
		break;
	case 50:
		m_LapsCount = 100;
		break;
	case 100:
		m_LapsCount = 200;
		break;
	case 200:
		m_LapsCount = 250;
		break;
	case 250:
		m_LapsCount = 500;
		break;
	default:
		m_LapsCount = 1000;
		break;
	}

	SetText(m_LapText, std::to_string(m_LapsCount), m_TextRect);
	lapcount = m_LapsCount;
}

void button::SetTimeText(float time)
{
	if (GetIsAverage())			SetText(m_AverageText, "(A) " + std::to_string(time) + " (s)", m_TextRect);
	else if (GetIsLowest())		SetText(m_LowestText, "(L) " + std::to_string(time) + " (s)", m_TextRect);
	else if (GetIsHighest())	SetText(m_HighestText, "(H) " + std::to_string(time) + " (s)", m_TextRect);
	else if(GetIsRuntime())		SetText(m_RunText, std::to_string(time) + " (s)", m_TextRect);
}

void button::SetIndexText(int index)
{
	SetText(m_IndexText, "Index: " + std::to_string(index), m_TextRect);
}


void button::SetTextColor(sf::Color c) {
	m_TextRect.setFillColor(c);
}
void button::ColorSwitch(bool b) {
	if (b) m_TextRect.setFillColor(m_TextRect.getOutlineColor());
	else m_TextRect.setFillColor(sf::Color::White);
}

void button::BoolSwitch(bool & b)
{
	if (!b) b = true;
	else b = false;
	ColorSwitch(b);
}

button::~button()
{
}
