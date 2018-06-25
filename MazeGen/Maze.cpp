#include "stdafx.h"
#include "Maze.h"

Maze::Maze(int w, int h, int c, int r, sf::RenderWindow * window) :
	m_Width(w), m_Height(h), m_Cols(c), m_Rows(r), m_Window(window)
{
	srand(time(NULL));
	Init(sf::Vector2f(0, 0), sf::Vector2f(c - 1, r - 1));
}

Maze::~Maze()
{
}

void Maze::Init(sf::Vector2f start, sf::Vector2f end)
{
	m_Blocks.clear();
	
	int size = (int)(m_Width / m_Cols);

	for (int i = 0; i < m_Cols; i += 2) {
		for (int j = 0; j < m_Rows; j +=2) {
			m_Blocks.push_back(new Block(i, j, size, m_Window));

			if (sf::Vector2f(i, j) == start)
				m_Blocks[m_Blocks.size() - 1]->m_IsStart = true;

			else if (sf::Vector2f(i, j) == end)
				m_Blocks[m_Blocks.size() - 1]->m_IsEnd = true;
		}
	}

	m_CurrBlock = m_Blocks[0];
	m_Stack.push_back(m_CurrBlock);
}

void Maze::Draw()
{
	if (!m_Completed) {
		int it = 0;
		while (it < m_Speed) {
			it++;
			m_CurrBlock->m_IsVisited = true;
			m_CurrBlock->m_InStack = true;
			Block* next = GetRandomNeighbour(m_CurrBlock);
			if (next != nullptr) {

				RemoveWalls(m_CurrBlock, next);
				m_CurrBlock = next;
				m_Stack.push_back(m_CurrBlock);
			}
			else {
				if (m_Stack.size() > 1) {
					m_Stack[m_Stack.size() - 1]->m_InStack = false;
					m_Stack.pop_back();
					m_CurrBlock = m_Stack[m_Stack.size() - 1];
				}
				if (m_Stack.size() == 1) {
					m_Stack[m_Stack.size() - 1]->m_InStack = false;
					m_Stack.clear();
					m_Completed = true;

					it = m_Speed;
				}
			}
		}
	}

	for (int i = 0; i < m_Blocks.size(); i++) {
		m_Blocks[i]->Draw();
	}
}

Block * Maze::GetRandomNeighbour(Block * block)
{
	std::vector<Block*> available;

	int size = m_Blocks.size();

	std::vector<int> m_Index;
	m_Index.push_back(GetMazeBlockIndexByPos(block->GetCol() + 1, block->GetRow()));
	m_Index.push_back(GetMazeBlockIndexByPos(block->GetCol() - 1, block->GetRow()));
	m_Index.push_back(GetMazeBlockIndexByPos(block->GetCol(), block->GetRow() - 1));
	m_Index.push_back(GetMazeBlockIndexByPos(block->GetCol(), block->GetRow() + 1));

	for (int i = 0; i < m_Index.size(); i++) {
		if (m_Index[i] < size && m_Index[i] != -1) {
			if (!m_Blocks[m_Index[i]]->m_IsVisited) {
				available.push_back(m_Blocks[m_Index[i]]);
			}
		}
	}
	if (available.size() > 0) {
		return available[rand() % available.size()];
	}
	else {
		return nullptr;
	}

}

void Maze::RemoveWalls(Block * block1, Block * block2)
{
		if (block1->GetCol() == block2->GetCol() + 1) {
		block1->m_Left = false;
		block2->m_Right = false;
	}
	else if (block1->GetCol() == block2->GetCol() - 1) {
		block1->m_Right = false;
		block2->m_Left = false;
	}
	else if (block1->GetRow() == block2->GetRow() + 1) {
		block1->m_Top = false;
		block2->m_Bot = false;
	}
	else if (block1->GetRow() == block2->GetRow() - 1) {
		block1->m_Bot = false;
		block2->m_Top = false;
	}
}

int Maze::GetMazeBlockIndexByPos(int x, int y)
{
	if (x < 0 || y < 0 || x > m_Cols - 1 || y > m_Rows - 1)
		return -1;

	int retValue = (y) + (x * m_Cols);
	return retValue;
}
