#include "SFML/Graphics.hpp"
#include "button.h"
#include "bspDungeon.h"
#pragma once
class bspWindow
{
public:
	bspWindow(std::vector<Tilemap> *dung, std::vector<double> *runtime);
	void run();	
	~bspWindow();

private:
	void CreateButtons();
	void PopVectors();
	void ClearTextfiles();

	bool m_SplitSwitch = false;
	int m_MapSize, m_LapCount;

	double xScale, yScale;
	float m_SplitCount;
	int m_Index = 0;
	int m_HighIndex, m_LowIndex;
	double m_Av;

	sf::RenderWindow* m_Window;
	sf::Image m_His;
	sf::Sprite m_HisSprite;
	sf::Texture m_HisTexture;
	sf::Sprite m_BackgroundSprite;
	sf::Texture m_BackgroundTexture;
	std::vector<button*> m_Buttons;
	std::vector<double> m_Runtime;
	std::vector<double> *m_RuntimePointer;
	std::vector<bspDungeon> m_Dungeons;
	std::vector<Tilemap> m_Tilemaps;
	std::vector<Tilemap> *m_DungPointer;
};

