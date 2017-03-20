#include "lib.h"

Forest::Forest()
{
	treesNum = 1;

	isTrained = false;

	trees = new BinaryTree*[treesNum];
	//trees[0] = new BinaryTree(0, 20, 100);
	trees[0] = new BinaryTree(1, 10, 200);
	//trees[2] = new BinaryTree(3, FEATURES_NUM, STATISTICS_NUM + 20, DEPTH_OF_TREE);
}

void Forest::buildForest(Features** features)
{
	for (int i = 0; i < treesNum; i++)
	{
		trees[i]->buildTree(trees[i]->root, features[i]);
	}
}

void Forest::writeForest()
{
	string fileName;
	for (int i = 0; i < treesNum; i++)
	{
		fileName = to_string(i) + ".txt";
		trees[i]->writeTree(fileName);
	}
}

void Forest::readForest()
{
	string fileName;
	for (int i = 0; i < treesNum; i++)
	{
		fileName = to_string(i) + ".txt";
		trees[i]->readTree(fileName);
	}
}

bool Forest::classifyFeatures(Features **features)
{
	int voteYesNum = 0;
	int voteNoNum = 0;

	for (int i = 0; i < treesNum; i++)
	{
		if (trees[i]->classifyFeatures(trees[i]->root, features[i]))
		{
			voteYesNum++;
		}
		else
		{
			voteNoNum++;
		}
	}

	return voteYesNum > voteNoNum;
}