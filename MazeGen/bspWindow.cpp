#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "DungeonEvaluator.h"
#include "bspWindow.h"


bspWindow::bspWindow(std::vector<Tilemap> *dung, std::vector<double> *runtime)
{
	m_DungPointer = dung;
	m_RuntimePointer = runtime;
	m_Window = new sf::RenderWindow(sf::VideoMode(1600, 900), "Buck's Algorithm",
		sf::Style::Default);

	xScale = 1600 / 1600.f;
	yScale = 900 / 900.f;
	if (!m_His.loadFromFile("his.png")) {}
	m_HisTexture.loadFromImage(m_His);
	m_HisSprite.setTexture(m_HisTexture);
	m_HisSprite.setPosition(m_Window->getSize().x - 300, m_Window->getSize().y - 300);

	sf::Image image;
	image.create(1600, 900, sf::Color::White);
	m_BackgroundTexture.loadFromImage(image);
	m_BackgroundSprite.setTexture(m_BackgroundTexture);
}

void bspWindow::run()
{
	CreateButtons();
	for (int i = 0; i < m_Buttons.size(); i++) {
		if (m_Buttons[i]->GetIsMap())
			m_MapSize = m_Buttons[i]->GetMapsize();
		else if (m_Buttons[i]->GetIsLaps())
			m_LapCount = m_Buttons[i]->GetLapCount();
		else if (m_Buttons[i]->GetIsThreshold())
			m_SplitCount = m_Buttons[i]->GetThresholdCount();
	}
	bool Generated = false;
	while (m_Window->isOpen())
	{
		sf::Event event;
		while (m_Window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				m_Window->close();
			if (event.type == sf::Event::KeyPressed)
				if (event.key.code == sf::Keyboard::Escape)
					m_Window->close();				
				else if (event.key.code == sf::Keyboard::Left && Generated) {
					if (m_Index != 0) m_Index--;
					for (int j = 0; j < m_Buttons.size(); j++) {
						if (m_Buttons[j]->GetIsRuntime()) m_Buttons[j]->SetTimeText(m_Runtime[m_Index]);
						else if (m_Buttons[j]->GetIsIndex()) m_Buttons[j]->SetIndexText(m_Index);
					}
				}
				else if (event.key.code == sf::Keyboard::Right && Generated) {
					if (m_Index < m_Dungeons.size() - 1) m_Index++;
					for (int j = 0; j < m_Buttons.size(); j++) {
						if (m_Buttons[j]->GetIsRuntime()) m_Buttons[j]->SetTimeText(m_Runtime[m_Index]);
						else if (m_Buttons[j]->GetIsIndex()) m_Buttons[j]->SetIndexText(m_Index);
					}
				}
			if (event.type == sf::Event::MouseButtonPressed)
				if (event.mouseButton.button == sf::Mouse::Left)
					for (int i = 0; i < m_Buttons.size(); i++)
						if (m_Buttons[i]->GetButton().getGlobalBounds().contains(sf::Mouse::getPosition(*m_Window).x, sf::Mouse::getPosition(*m_Window).y)) {
							if (m_Buttons[i]->GetIsMap()) m_Buttons[i]->SetMapsize(m_MapSize);
							else if(m_Buttons[i]->GetIsSplitSwitch()) m_Buttons[i]->BoolSwitch(m_SplitSwitch);
							else if (m_Buttons[i]->GetIsLaps()) m_Buttons[i]->SetLapCount(m_LapCount);
							else if (m_Buttons[i]->GetIsThreshold()) m_Buttons[i]->SetSplit(m_SplitCount);
							else if (m_Buttons[i]->GetIsGenerator()) {
								if (m_MapSize % 2 != 1)	m_MapSize += 1;
								//ClearTextfiles();
								m_Dungeons.clear();
								m_Runtime.clear();
								m_Tilemaps.clear();
								Generated = true;
								m_Av = m_Index = m_LowIndex = m_HighIndex = 0;
								for (int k = 0; k < m_LapCount; k++) {
									m_Dungeons.push_back(bspDungeon(m_MapSize, m_MapSize, m_Window, 600, 600, m_SplitSwitch, m_SplitCount));
									m_Runtime.push_back(m_Dungeons.back().runtime);
									m_Tilemaps.push_back(m_Dungeons.back().GetTilemap());

									DungeonEvaluator eval = DungeonEvaluator(&m_Tilemaps.back(), false, &m_Runtime.back());

									if (m_Dungeons.size() > 1 && m_MapSize > 300) PopVectors();
									else if (m_Dungeons.size() > 10 && m_MapSize > 200) PopVectors();
									else if (m_Dungeons.size() > 20 && m_MapSize >= 150) PopVectors();
									else if (m_Dungeons.size() > 50 && m_MapSize >= 100) PopVectors();
									else if (m_Dungeons.size() > 100) PopVectors();

									std::cout << k << std::endl;
									std::cout << std::endl;
								}
								for (int i = 0; i < m_Runtime.size(); i++) {
									if (m_Runtime[m_HighIndex] < m_Runtime[i]) m_HighIndex = i;
									if (m_LowIndex == 0 || m_Runtime[m_LowIndex] > m_Runtime[i]) m_LowIndex = i;
									m_Av += m_Runtime[i];
								}
								
								for (int j = 0; j < m_Buttons.size(); j++) {
									if (m_Buttons[j]->GetIsIndex()) m_Buttons[j]->SetIndexText(m_Index);
									else if (m_Buttons[j]->GetIsRuntime()) m_Buttons[j]->SetTimeText(m_Runtime[m_Index]);
									else if (m_Buttons[j]->GetIsLowest())  m_Buttons[j]->SetTimeText(m_Runtime[m_LowIndex]);
									else if (m_Buttons[j]->GetIsAverage()) m_Buttons[j]->SetTimeText(m_Av / m_LapCount);
									else if (m_Buttons[j]->GetIsHighest()) m_Buttons[j]->SetTimeText(m_Runtime[m_HighIndex]);
								}
								break;
								
							}
						}
		}

		m_Window->clear();
		m_Window->draw(m_BackgroundSprite);
		m_Window->draw(m_HisSprite);
		for (int i = 0; i < m_Buttons.size(); i++) {
			if (m_Buttons[i]->GetIsLowest()) m_Buttons[i]->ColorSwitch(m_Index == m_LowIndex);
			else if (m_Buttons[i]->GetIsHighest()) m_Buttons[i]->ColorSwitch(m_Index == m_HighIndex);
			m_Buttons[i]->Draw();
		}
		if (Generated)
			m_Dungeons[m_Index].Draw();
		m_Window->display();
	}
	*m_DungPointer = m_Tilemaps;
	*m_RuntimePointer = m_Runtime;
}

bspWindow::~bspWindow()
{
	for (int i = 0; i < m_Buttons.size(); i++) {
		delete m_Buttons[i];
	}
	delete m_Window;
}

void bspWindow::CreateButtons()
{

	double x = xScale, y = yScale;
	
	sf::Font myfont;
	if (!myfont.loadFromFile("arial.ttf")) {}

	sf::FloatRect temp(1100 * x, 25 * y, 250 * x, 50 * y);
	button* genButton = new button(myfont, temp, m_Window, "Generate", 0);
	m_Buttons.push_back(genButton);

	sf::FloatRect temp2(750 * x, 25 * y, 250 * x, 50 * y);
	button* runtimeButton = new button(myfont, temp2, m_Window, 0, 1);
	m_Buttons.push_back(runtimeButton);

	sf::FloatRect temp3(1100 * x, 100 * y, 250 * x, 50 * y);
	button* sizeButton = new button(myfont, temp3, m_Window, 100, "Map size", 2);
	m_Buttons.push_back(sizeButton);

	sf::FloatRect temp4(1100 * x, 175 * y, 250 * x, 50 * y);
	button* lapsButton = new button(myfont, temp4, m_Window, 100, "Laps", 4);
	m_Buttons.push_back(lapsButton);

	sf::FloatRect temp5(1100 * x, 250 * y, 250 * x, 50 * y);
	button* thresholdButton = new button(myfont, temp5, m_Window, 100, "Split Threshold", 15);
	m_Buttons.push_back(thresholdButton);

	sf::FloatRect temp6(750 * x, 100 * y, 250 * x, 50 * y);
	button* indexButton = new button(myfont, temp6, m_Window, 0, 5);
	m_Buttons.push_back(indexButton);

	sf::FloatRect temp7(750 * x, 175 * y, 250 * x, 50 * y);
	button* averageButton = new button(myfont, temp7, m_Window, 0, 6);
	m_Buttons.push_back(averageButton);

	sf::FloatRect temp8(750 * x, 250 * y, 250 * x, 50 * y);
	button* highButton = new button(myfont, temp8, m_Window, 0, 7);
	m_Buttons.push_back(highButton);

	sf::FloatRect temp9(750 * x, 325 * y, 250 * x, 50 * y);
	button* lowButton = new button(myfont, temp9, m_Window, 0, 8);
	m_Buttons.push_back(lowButton);

	//1200 vid 1600:900
	//1225 vid 1280:720
	sf::FloatRect temp10(1200 * x, 25 * y, 250 * x, 50 * y);
	button* splitSwitchButton = new button(myfont, temp10, m_Window, 0, 14);
	m_Buttons.push_back(splitSwitchButton);

}

void bspWindow::PopVectors()
{
	m_Dungeons.pop_back();
	//m_Runtime.pop_back();
	m_Tilemaps.pop_back();
}

void bspWindow::ClearTextfiles()
{
	std::string runtimeName = "BSP_" + std::to_string(m_MapSize) + "_Runtime.txt";
	std::string densityName = "BSP_" + std::to_string(m_MapSize) + "_Density.txt";
	std::string relativesizeName = "BSP_" + std::to_string(m_MapSize) + "_RelativeSize.txt";

	std::ofstream ofs("ResultMap/" + runtimeName, std::ofstream::out | std::ofstream::trunc);
	std::ofstream ofs1("ResultMap/" + densityName, std::ofstream::out | std::ofstream::trunc);
	std::ofstream ofs2("ResultMap/" + relativesizeName, std::ofstream::out | std::ofstream::trunc);
	ofs.close();
	ofs1.close();
	ofs2.close();
}
