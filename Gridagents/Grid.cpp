#include"Grid.h"

Grid::Grid(unsigned int size, unsigned int numObstacles, unsigned int numAgents)
{
	gridsize = size;
	srand(time(NULL));

	finished = false;

	grid.assign(gridsize, std::vector<int>(gridsize, -1));
	agents.assign(numAgents, Agent());

	//Place obstacles
	for (unsigned int i = 0; i < numObstacles; ++i)
	{
		unsigned int x;
		unsigned int y;

		//Ensure the same location is not set as obstacle twice
		do
		{
			x = rand() % gridsize;
			y = rand() % gridsize;
		} while (grid[x][y] == -2);

		grid[x][y] = -2;

		unsigned int* obsLoc = new unsigned int[2];
		obsLoc[0] = x;
		obsLoc[1] = y;
		obstacleLocations.push_back(obsLoc);
	}

	//Place agents
	std::vector<std::vector<bool>> usedDestinations(gridsize, std::vector<bool>(gridsize, false));
	for (unsigned int i = 0; i < numAgents; ++i)
	{
		unsigned int x;
		unsigned int y;
		unsigned int desx;
		unsigned int desy;

		//Set new agent position in an empty location
		do
		{
			x = rand() % gridsize;
			y = rand() % gridsize;
		} while (grid[x][y] != -1);

		grid[x][y] = i;
		agents[i].setId(i);

		unsigned int* startPos = new unsigned int[2];
		startPos[0] = x;
		startPos[1] = y;
		startingLocations.push_back(startPos);

		//Set new agent destination, ensure destination is empty and not already another agent's destination
		do
		{
			desx = rand() % gridsize;
			desy = rand() % gridsize;
		} while (grid[desx][desy] != -1 || usedDestinations[desx][desy]);

		usedDestinations[desx][desy] = true;

		agents[i].setPosition(x, y);
		agents[i].setDestination(desx, desy);
		agents[i].findRoute(&grid);
	}
	usedDestinations.clear();
}

Grid::~Grid()
{
	for (unsigned int i = 0; i < startingLocations.size(); ++i)
	{
		delete startingLocations[i];
	}

	for (unsigned int i = 0; i < obstacleLocations.size(); ++i)
	{
		delete obstacleLocations[i];
	}

	for (unsigned int i = 0; i < visitedLocations.size(); ++i)
	{
		delete visitedLocations[i];
	}
}

void Grid::draw(HWND hwnd)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	Graphics graphics(hdc);
	SolidBrush backGroundBrush(Color(255, 0, 0, 0));
	Pen whitePen(Color(255, 255, 255, 255));
	SolidBrush redBrush(Color(255, 255, 0, 0));
	SolidBrush greenBrush(Color(255, 0, 255, 0));
	SolidBrush orangeBrush(Color(255, 255, 100, 0));
	SolidBrush yellowBrush(Color(255, 255, 255, 0));

	graphics.FillRectangle(&backGroundBrush, 0, 0, 800, 800);
	for (unsigned int i = 0; i <= gridsize; ++i)
	{
		int xPos = 20 + ((double)i / (double)gridsize) * 700;
		int yPos = 20 + ((double)i / (double)gridsize) * 700;
		graphics.DrawLine(&whitePen, xPos, 20, xPos, 720);
		graphics.DrawLine(&whitePen, 20, yPos, 720, yPos);
	}

	for (unsigned int i = 0; i < obstacleLocations.size(); ++i)
	{
		int x = obstacleLocations[i][0];
		int y = obstacleLocations[i][1];
		int xPos = 21 + ((double)x / (double)gridsize) * 700;
		int yPos = 21 + ((double)y / (double)gridsize) * 700;
		int size = 720.0 / (double)gridsize;
		size -= 1;
		graphics.FillRectangle(&redBrush, xPos, yPos, size, size);
	}

	for (unsigned int i = 0; i < visitedLocations.size(); ++i)
	{
		int x = visitedLocations[i][0];
		int y = visitedLocations[i][1];
		int xPos = 21 + ((double)x / (double)gridsize) * 700;
		int yPos = 21 + ((double)y / (double)gridsize) * 700;
		int size = 720.0 / (double)gridsize;
		size -= 1;
		graphics.FillRectangle(&yellowBrush, xPos, yPos, size, size);
	}

	for (unsigned int i = 0; i < startingLocations.size(); ++i)
	{
		int x = startingLocations[i][0];
		int y = startingLocations[i][1];
		int xPos = 21 + ((double)x / (double)gridsize) * 700;
		int yPos = 21 + ((double)y / (double)gridsize) * 700;
		int size = 720.0 / (double)gridsize;
		size -= 1;
		graphics.FillRectangle(&greenBrush, xPos, yPos, size, size);
	}

	for (unsigned int i = 0; i < agents.size(); ++i)
	{
		int x = agents.at(i).getPosition()[0];
		int y = agents.at(i).getPosition()[1];
		int xPos = 21 + ((double)x / (double)gridsize) * 700;
		int yPos = 21 + ((double)y / (double)gridsize) * 700;
		int size = 720.0 / (double)gridsize;
		size -= 1;
		graphics.FillRectangle(&orangeBrush, xPos, yPos, size, size);
	}

	if (finished)
	{
		FontFamily  fontFamily(L"Times New Roman");
		Font        font(&fontFamily, 24, FontStyleRegular, UnitPixel);
		PointF      pointF(20.0f, 725.0f);
		SolidBrush  solidBrush(Color(255, 255, 255, 255));

		graphics.DrawString(L"All agents have reached their destination. Use any key to exit.", -1, &font, pointF, &solidBrush);
	}
	EndPaint(hwnd, &ps);
}

bool Grid::isFinished()
{
	return finished;
}

void Grid::update()
{
	finished = true;
	for (unsigned int i = 0; i < agents.size(); ++i)
	{
		unsigned int* visitedLoc = new unsigned int[2];
		visitedLoc[0] = agents.at(i).getPosition()[0];
		visitedLoc[1] = agents.at(i).getPosition()[1];
		visitedLocations.push_back(visitedLoc);

		agents.at(i).update(&grid);
		if (!agents.at(i).atDestination())
		{
			finished = false;
		}
	}
}