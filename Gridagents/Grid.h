#pragma once
#include<windows.h>
#include<vector>
#include"Agent.h"
#include<time.h>
#include<stdlib.h>
#include<gdiplus.h>
using namespace Gdiplus;
class Grid
{
public:
	Grid(unsigned int, unsigned int, unsigned int);
	~Grid();
	void draw(HWND);
	bool isFinished();
	void update();
private:
	std::vector<std::vector<int>> grid;
	std::vector<Agent> agents;
	std::vector<unsigned int*> obstacleLocations;
	std::vector<unsigned int*> visitedLocations;
	std::vector<unsigned int*> startingLocations;
	unsigned int gridsize;
	bool finished;
};