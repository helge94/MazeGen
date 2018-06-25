#include "SFML/Graphics.hpp"
#pragma once
class button
{
public:
	button(sf::Font f, sf::FloatRect r, sf::RenderWindow* w, int distance, int type);
	button(sf::Font f, sf::FloatRect r, sf::RenderWindow* w, std::string s, int type);
	button(sf::Font f, sf::FloatRect r, sf::RenderWindow* w, int distance, std::string s,int type);
	void Draw();
	void SetSplit(float& split);
	void SetMapsize(int& mapsize);
	void SetWinding(int& winding);
	void SetRoomCount(int& roomcount);
	void SetLapCount(int& lapcount);
	void SetIndexText(int index);
	void SetTimeText(float time);
	void SetTextColor(sf::Color c);
	void ColorSwitch(bool b);
	void BoolSwitch(bool& b);
	int GetRoomCount()		{ return m_RoomCount; }
	int GetMapsize()		{ return m_Mapsize; }
	int GetLapCount()		{ return m_LapsCount; }
	float GetThresholdCount() { return (float)m_SplitThreshold / 100; }

	bool GetIsGenerator()	const { return switchArray[0]; }
	bool GetIsRuntime()		const { return switchArray[1]; }
	bool GetIsMap()			const { return switchArray[2]; }
	bool GetIsRooms()		const { return switchArray[3]; }
	bool GetIsLaps()		const { return switchArray[4]; }
	bool GetIsIndex()		const { return switchArray[5]; }
	bool GetIsAverage()		const { return switchArray[6]; }
	bool GetIsHighest()		const { return switchArray[7]; }
	bool GetIsLowest()		const { return switchArray[8]; }
	bool GetIsMulti()		const { return switchArray[9]; }
	bool GetIsWinding()		const { return switchArray[10]; }
	bool GetIsBucks()		const { return switchArray[11]; }
	bool GetIsBSP()			const { return switchArray[12]; }
	bool GetIsEvaluator()	const { return switchArray[13]; }
	bool GetIsSplitSwitch() const { return switchArray[14]; }
	bool GetIsThreshold()	const { return switchArray[15]; }

	sf::RectangleShape GetButton()  { return m_ButtonRect; }
	~button();
private:
	int m_Mapsize = 25, m_RoomCount = 4, m_LapsCount = 1, m_Winding = 10, m_SplitThreshold = 110;
	bool switchArray[16] = { false };
	void SetText(sf::Text& text, std::string s, sf::RectangleShape r);
	sf::Text m_MapText, m_RoomText, m_RunText, m_LapText, m_NameText, m_IndexText, m_AverageText, m_LowestText, m_HighestText, m_MultiText, m_WindingText, m_SplitSwitchText, m_SplitThresholdText;
	sf::Font m_Font;
	sf::RectangleShape m_TextRect;
	sf::RectangleShape m_ButtonRect;
	sf::RenderWindow* m_Window;
};

