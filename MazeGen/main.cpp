#include "stdafx.h"
#include "bucksWindow.h"
#include <vector>
#include "mainWindow.h"
#include "DungeonGen.h"
#include <iostream>

int main() {

	srand((unsigned)time(NULL));
	std::vector<DungeonGen> *dung = new std::vector<DungeonGen>();
	//bucksWindow* window = new bucksWindow(dung);
	mainWindow* window = new mainWindow();
	window->run();
	return 0;
}