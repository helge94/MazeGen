#pragma once
#include <stdio.h>
#include <vector>

#include "SFML/Graphics.hpp"

#include "Block.h"


class Maze 
{
public:
	Maze(int w, int h, int c, int r, sf::RenderWindow* window);
	~Maze();

	void Init(sf::Vector2f start, sf::Vector2f end);

	void Draw();

	Block* GetRandomNeighbour(Block* block);

	void RemoveWalls(Block* block1, Block* block2);

	int GetMazeBlockIndexByPos(int x, int y);

private:

	int m_Width, m_Height;
	int m_Cols, m_Rows;

	sf::RenderWindow* m_Window;

	std::vector<Block*> m_Blocks;
	std::vector<Block*> m_Stack;

	bool m_Completed = false;

	int m_Speed = 20;

	Block* m_CurrBlock;
};

