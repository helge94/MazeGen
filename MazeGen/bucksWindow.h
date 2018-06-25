#include "DungeonGen.h"
#include "button.h"
#pragma once

class bucksWindow
{
public:
	bucksWindow(std::vector<Tilemap> *dung, std::vector<double> *runtime);
	void run();
	~bucksWindow();
private:
	int m_MapSize, m_RoomCount, m_LapCount, m_WindingCount;
	double xScale, yScale;
	bool m_MultiRoom = false;
	double m_Av;
	int m_Index = 0;
	int m_HighIndex, m_LowIndex;
	void CreateButtons();
	void PopVectors();
	void ClearTextfiles();
	sf::RenderWindow* m_Window;
	sf::Image m_His;
	sf::Sprite m_HisSprite;
	sf::Texture m_HisTexture;

	sf::Sprite m_BackgroundSprite;
	sf::Texture m_BackgroundTexture;
	std::vector<button*> m_Buttons;
	std::vector<double> m_Runtime;
	std::vector<double> *m_RuntimePointer;
	std::vector<DungeonGen> m_Dungeons;
	std::vector<Tilemap> m_Tilemap;
	std::vector<Tilemap> *m_DungPointer;
};
