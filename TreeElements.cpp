#include "lib.h"

Features::Features(int featuresNum)
{
	values = new int[featuresNum];
}

Child::Child()
{
	memset(statistics, 0, sizeof(statistics));
}

Node::Node(int level, int childsNum)
{
	this->level = level;
	featureNumToDivide = -1;
	left = NULL;
	right = NULL;
	childs = new Child[childsNum];
	previousSelectedFeatures = new int[level];

	memset(statistics, 0, sizeof(statistics));
}

void Node::removeOldData()
{
	delete[] childs;
	childs = NULL;
	delete[] previousSelectedFeatures;
	previousSelectedFeatures = NULL;
}
