#pragma once
#include<vector>
#include<stdlib.h>
class Agent
{
public:
	Agent();
	~Agent();
	unsigned int* getPosition();
	unsigned int* getDestination();
	unsigned int* getNextPosition();
	void setId(unsigned int);
	void setPosition(unsigned int, unsigned int);
	void setDestination(unsigned int, unsigned int);
	void update(std::vector<std::vector<int>>*);
	void findRoute(std::vector<std::vector<int>>*);
	bool atDestination();
	bool hasRoute();
	
private:
	unsigned int id;
	unsigned int position[2];
	unsigned int destination[2];

	std::vector<unsigned int*> route;
	unsigned int routeCurrentIndex;
	bool hasValidRoute;
};