#include "lib.h"

Forest::Forest()
{
	treesNum = 1;

	trees = new BinaryTree*[treesNum];
		
	trees[0] = new BinaryTree(50, 50, 10, 100, 0.1); 
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