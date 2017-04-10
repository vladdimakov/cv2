#include "lib.h"

Features::Features(int featuresNum)
{
	values = new int[featuresNum];
}

Features::~Features()
{
	//delete[] values; // TODO!!!!
}

Child::Child()
{
	memset(statistics, 0, sizeof(statistics));
}

Node::Node(int level, int childsNum, int randomlySelectedFeaturesNum)
{
	this->level = level;
	featureNumToDivide = -1;
	left = NULL;
	right = NULL;
	childs = new Child[childsNum];
	randomlySelectedFeatures = new int[randomlySelectedFeaturesNum];
	previousSelectedFeatures = new int[level];

	memset(statistics, 0, sizeof(statistics));
}

void Node::removeOldData()
{
	delete[] childs;
	childs = NULL;
	delete[] randomlySelectedFeatures;
	randomlySelectedFeatures = NULL;
	delete[] previousSelectedFeatures;
	previousSelectedFeatures = NULL;
}
