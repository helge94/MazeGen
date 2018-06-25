#include "stdafx.h"
#include "bspWindow.h"
#include "DungeonEvaluator.h"
#include "mainWindow.h"


mainWindow::mainWindow()
{

	m_Window = new sf::RenderWindow(sf::VideoMode(1600, 900), "MazeGen",
		sf::Style::Default);

	if (!m_His.loadFromFile("his.png")) {}
	m_HisTexture.loadFromImage(m_His);
	m_HisSprite.setTexture(m_HisTexture);
	m_HisSprite.setPosition(m_Window->getSize().x - 300, m_Window->getSize().y - 300);

	sf::Image image;
	image.create(1600, 900, sf::Color::White);
	m_BackgroundTexture.loadFromImage(image);
	m_BackgroundSprite.setTexture(m_BackgroundTexture);
}


void mainWindow::run()
{
	std::vector<Result> m_Result;
	CreateButtons();
	while (m_Window->isOpen()){
		sf::Event event;
		while (m_Window->pollEvent(event)){
			if (event.type == sf::Event::Closed)
				m_Window->close();

			if (event.type == sf::Event::KeyPressed)
				if (event.key.code == sf::Keyboard::Escape)
					m_Window->close();

			if (event.type == sf::Event::MouseButtonPressed)
				if (event.mouseButton.button == sf::Mouse::Left) {
					for (int i = 0; i < m_Buttons.size(); i++) {
						if (m_Buttons[i]->GetButton().getGlobalBounds().contains(sf::Mouse::getPosition(*m_Window).x, sf::Mouse::getPosition(*m_Window).y)) {
							if (m_Buttons[i]->GetIsBucks()) {
								ClearVectors();
								m_Window->setActive(false);
								m_Window->setVisible(false);
								bucksWindow* window = new bucksWindow(m_BuckDungeons, m_BuckRuntime);
								window->run();
								delete window;
								m_Window->setActive(true);
								m_Window->setVisible(true);
								break;
							}
							else if (m_Buttons[i]->GetIsBSP()) {
								ClearVectors();
								m_Window->setActive(false);
								m_Window->setVisible(false);
								bspWindow* window = new bspWindow(m_BSPDungeons, m_BSPRuntime);
								window->run();
								delete window;
								m_Window->setActive(true);
								m_Window->setVisible(true);
								break;
							}
							else if (m_Buttons[i]->GetIsEvaluator()) {
								std::vector<Tilemap> temp;
								std::vector<double> runtime;
								if (m_BuckDungeons->size() > 0) {
									temp = *m_BuckDungeons;
									runtime = *m_BuckRuntime;
								}
								else if (m_BSPDungeons->size() > 0) {
									temp = *m_BSPDungeons;
									runtime = *m_BSPRuntime;
								}
								for (int i = 0; i < temp.size(); i++) {
									DungeonEvaluator eval = DungeonEvaluator(&temp[i], true, &runtime[i]);
									m_Result.push_back(eval.GetResults());
								}

							}
						}
						
					}
				}

		}
		m_Window->clear();
		m_Window->draw(m_BackgroundSprite);
		m_Window->draw(m_HisSprite);
		for (int i = 0; i < m_Buttons.size(); i++) {
			m_Buttons[i]->Draw();
		}
		m_Window->display();
	}
}

void mainWindow::CreateButtons()
{
	sf::Font myfont;
	if (!myfont.loadFromFile("arial.ttf")) {}

	sf::FloatRect temp(1100, 25, 250, 50);
	button* genButton = new button(myfont, temp, m_Window, "Buck's Algorithm", 11);
	m_Buttons.push_back(genButton);

	sf::FloatRect temp2(1100, 200, 250, 50);
	button* bspButton = new button(myfont, temp2, m_Window, "BSP", 12);
	m_Buttons.push_back(bspButton);

	sf::FloatRect temp3(1100, 375, 250, 50);
	button* evalButton = new button(myfont, temp3, m_Window, "Evaluate", 13);
	m_Buttons.push_back(evalButton);
}

void mainWindow::ClearVectors()
{
	delete m_BSPDungeons;
	delete m_BSPRuntime;
	delete m_BuckDungeons;
	delete m_BuckRuntime;

	m_BSPDungeons = new std::vector<Tilemap>();
	m_BSPRuntime = new std::vector<double>();
	m_BuckDungeons = new std::vector<Tilemap>();
	m_BuckRuntime = new std::vector<double>();
}

mainWindow::~mainWindow()
{
}


