#include "lib.h"

Forest::Forest()
{
	treesNum = 101;

	trees = new Tree*[treesNum];
	for (int i = 0; i < treesNum; i++)
		trees[i] = new Tree(100, 100, 10, 10, 5);
	//(int depthOfTree, int featuresNum, int randomlySelectedFeaturesNum, int minStatisticsNumForDivide, float minGiniCoefficient)
	//BinaryTree(100, 100, 10, 50, 0.1)

	correctlyClassifiedOOB = 0;
	incorrectlyClassifiedOOB = 0;
	OOBE = 0.0f;
}

void Forest::buildTree(int treeNum, Features* features)
{
	trees[treeNum]->buildTree(trees[treeNum]->root, features);
}

bool Forest::classifyFeaturesByTree(int treeNum, Features *features)
{
	return trees[treeNum]->classifyFeatures(trees[treeNum]->root, features);
}
void Forest::discardTree(int treeNum)
{
	treesNum--;
	for (int i = treeNum; i < treesNum; i++)
		trees[i] = trees[i + 1];
}