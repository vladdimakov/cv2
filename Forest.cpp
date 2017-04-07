#include "lib.h"

Forest::Forest()
{
	treesNum = 100;

	trees = new BinaryTree*[treesNum];
	for (int i = 0; i < treesNum; i++)
		trees[i] = new BinaryTree(100, 100, 10, 50, 0.1); 
	//(int depthOfTree, int featuresNum, int randomlySelectedFeaturesNum, int minStatisticsNumForDivide, float minGiniCoefficient)
}

void Forest::buildTree(int treeNum, Features* features)
{
	trees[treeNum]->buildTree(trees[treeNum]->root, features);
}

bool Forest::classifyFeaturesByTree(int treeNum, Features *features)
{
	return trees[treeNum]->classifyFeatures(trees[treeNum]->root, features);
}