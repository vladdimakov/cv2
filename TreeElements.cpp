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

Node::Node(int childsNum, int randomlySelectedFeaturesNum)
{
	featureNumToDivide = -1;
	left = NULL;
	right = NULL;
	childs = new Child[childsNum];
	randomlySelectedFeatures = new int[randomlySelectedFeaturesNum];

	memset(statistics, 0, sizeof(statistics));
}

void Node::removeChilds()
{
	delete[] childs;
	childs = NULL;
}
