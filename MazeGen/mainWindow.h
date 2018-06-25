#include "bucksWindow.h"
#include "bspWindow.h"
#pragma once

class mainWindow
{
public:
	mainWindow();
	void run();
	~mainWindow();
private:

	void CreateButtons();
	void ClearVectors();

	double xScale, yScale;

	sf::RenderWindow* m_Window;
	sf::Image m_His;
	sf::Sprite m_HisSprite;
	sf::Texture m_HisTexture;

	sf::Sprite m_BackgroundSprite;
	sf::Texture m_BackgroundTexture;

	std::vector<Tilemap>* m_BuckDungeons;
	std::vector<Tilemap>* m_BSPDungeons;
	std::vector<double>* m_BuckRuntime;
	std::vector<double>* m_BSPRuntime;
	std::vector<button*> m_Buttons;
};

