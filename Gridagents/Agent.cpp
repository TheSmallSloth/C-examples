#include"Agent.h"
Agent::Agent()
{
	hasValidRoute = false;
	routeCurrentIndex = 0;
}

Agent::~Agent()
{
	for (unsigned int i = 1; i < route.size(); ++i)
	{
		delete route[i];
	}
}

void Agent::setId(unsigned int agentId)
{
	id = agentId;
}

unsigned int* Agent::getPosition()
{
	return position;
}

unsigned int* Agent::getDestination()
{
	return destination;
}

unsigned int* Agent::getNextPosition()
{
	if (routeCurrentIndex < route.size() - 1)
	{
		return route[routeCurrentIndex + 1];
	}
	else
	{
		return position;
	}
}

void Agent::setPosition(unsigned int x, unsigned int y)
{
	position[0] = x;
	position[1] = y;
}
void Agent::setDestination(unsigned int x, unsigned int y)
{
	destination[0] = x;
	destination[1] = y;
}

//Breadth first search to find a route through the grid
void Agent::findRoute(std::vector<std::vector<int>>* grid)
{
	for (unsigned int i = 1; i < route.size(); ++i)
	{
		delete route[i];
	}

	route.clear();

	if (atDestination())
	{
		return;
	}

	//List of locations discovered during BFS
	std::vector<unsigned int*> locations;

	//Previous [i] holds value j such that
	//locations[j] is the previous location on the route to locations[i]
	std::vector<unsigned int> previous;

	//visited[i][j] is true if BFS has already visited grid[i][j]
	std::vector<std::vector<bool>> visited(grid->size(), std::vector<bool>(grid->size(), false));

	locations.push_back(position);
	previous.push_back(0);
	visited[position[0]][position[1]] = true;

	unsigned int currentLocation = 0; //Index of the current location being expanded during BFS


	while (currentLocation < locations.size())
	{
		
		unsigned int x = locations[currentLocation][0];
		unsigned int y = locations[currentLocation][1];

		unsigned int newPos[4][2] = { {x - 1,y}, {x + 1,y}, {x,y - 1}, {x,y + 1} };
		for (int i = 0; i < 4; ++i)
		{
			unsigned int newX = newPos[i][0];
			unsigned int newY = newPos[i][1];

			if (newX < grid->size() &&
				newY < grid->size() &&
				!visited[newX][newY] &&
				(*grid)[newX][newY] == -1)
			{
				unsigned int* newLocation = new unsigned int[2];
				newLocation[0] = newX;
				newLocation[1] = newY;
				locations.push_back(newLocation);
				previous.push_back(currentLocation);
				visited[newX][newY] = true;
				if (destination[0] == newX && destination[1] == newY)
				{
					currentLocation = locations.size();
					break;
				}
			}
		}
		
		currentLocation++;
	}


	if (locations.back()[0] != destination[0] || locations.back()[1] != destination[1]) //No route found
	{
		for (unsigned int i = 1; i < locations.size(); ++i)
		{
			delete locations[i];
		}
		hasValidRoute = false;
		return;
	}

	hasValidRoute = true;
	routeCurrentIndex = 0;

	currentLocation = locations.size() - 2;

	route.push_back(locations.back());

	unsigned int next = previous.back();
	while (currentLocation != 0)
	{
		if (currentLocation == next)
		{
			route.push_back(locations[currentLocation]);
			next = previous[currentLocation];
		}
		else
		{
			delete locations[currentLocation];
		}
		currentLocation--;
	}
	route.push_back(position);

	for (unsigned int i = 0; i < route.size() / 2; ++i)
	{
		unsigned int* temp = route[i];
		route[i] = route[route.size() - (i + 1)];
		route[route.size() - (i + 1)] = temp;
	}

}

void Agent::update(std::vector<std::vector<int>>* grid)
{
	if (atDestination()) { return; }

	if (!hasValidRoute)
	{
		unsigned int randLocs[4][2] = { {position[0] - 1,position[1]},
										{position[0] + 1,position[1]},
										{position[0],position[1] - 1},
										{position[0],position[1] + 1}};
		bool canMove = false;
		for (int i = 0; i < 4; ++i)
		{
			unsigned int randX = randLocs[i][0];
			unsigned int randY = randLocs[i][1];
			if (randX >= grid->size() || randY >= grid->size())
			{
				randLocs[i][0] = randX = position[0];
				randLocs[i][1] = randY = position[1];
			}
			if ((*grid)[randX][randY] == -1)
			{
				canMove = true;
			}
		}
		if (canMove)
		{
			int randIndex = -1;
			unsigned int randX;
			unsigned int randY;
			do
			{
				randIndex = rand() % 4;
				randX = randLocs[randIndex][0];
				randY = randLocs[randIndex][1];
			} while ((*grid)[randX][randY] != -1);
			(*grid)[position[0]][position[1]] = -1;
			(*grid)[randX][randY] = id;
			position[0] = randX;
			position[1] = randY;
		}
		findRoute(grid);
		return;
	}
	unsigned int curX = position[0];
	unsigned int curY = position[1];
	unsigned int* newPos = getNextPosition();
	unsigned int newX = newPos[0];
	unsigned int newY = newPos[1];

	if ((*grid)[newX][newY] != -1) //Another agent is blocking the way
	{
		findRoute(grid);
		update(grid);
		return;
	}

	routeCurrentIndex++;
	(*grid)[curX][curY] = -1;
	(*grid)[newX][newY] = id;
	position[0] = newX;
	position[1] = newY;
}

bool Agent::atDestination()
{
	return (position[0] == destination[0]) && (position[1] == destination[1]);
}

bool Agent::hasRoute()
{
	return hasValidRoute;
}
