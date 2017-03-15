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

Node::Node(int childsNum)
{
	featureNumToDivide = -1;
	left = NULL;
	right = NULL;
	childs = new Child[childsNum];

	memset(statistics, 0, sizeof(statistics));	
}

void Node::removeChilds()
{
	delete[] childs;
	childs = NULL;
}

BinaryTree::BinaryTree(int featuresNum, int statisticsNum)
{
	_featuresNum = featuresNum;
	_statisticsNum = statisticsNum;

	root = new Node(_featuresNum);	
}

void BinaryTree::buildLeafsForCurrentNode(Node* node, Features features)
{	
	node->statistics[features.isTarget]++;

	for (int i = 0; i < _featuresNum; i++)
	{	
		if (features.values[i] == -1)
			continue;
		
		node->childs[i].statistics[features.values[i]][features.isTarget]++;
	}

	if (node->statistics[0] + node->statistics[1] >= _statisticsNum)
	{
		divideNode(node);
	}
}

float BinaryTree::calcGiniCoefficient(Child child)
{
	float giniCoefficient = 0;

	for (int j = 0; j < 2; j++)
	{
		if (child.statistics[0][j] + child.statistics[1][j] != 0)
		{
			giniCoefficient += (float)(child.statistics[0][j] * child.statistics[0][j] + child.statistics[1][j] * child.statistics[1][j]) /
								(child.statistics[0][j] + child.statistics[1][j]);
		}
	}

	return giniCoefficient;
}

void BinaryTree::divideNode(Node* node)
{
	float *giniCoefficients = new float[_featuresNum];
	for (int i = 0; i < _featuresNum; i++)
	{
		giniCoefficients[i] = calcGiniCoefficient(node->childs[i]);
	}

	float maxGiniCoefficient = 0;
	int maxGiniCoefficientNum;
	for (int i = 0; i < _featuresNum; i++)
	{
		if (giniCoefficients[i] >= maxGiniCoefficient)
		{
			maxGiniCoefficient = giniCoefficients[i];
			maxGiniCoefficientNum = i;
		}
	}

	delete[] giniCoefficients;

	node->featureNumToDivide = maxGiniCoefficientNum;

	node->left = new Node(_featuresNum);	
	node->right = new Node(_featuresNum);

//	node->removeChilds();
}

void BinaryTree::buildLeafs(Node* node, Features features)
{
	if (node->featureNumToDivide != -1)
	{
		if (features.values[node->featureNumToDivide] == 0)
		{
			features.values[node->featureNumToDivide] = -1;
			buildLeafs(node->left, features);
		}
		else
		{
			features.values[node->featureNumToDivide] = -1;
			buildLeafs(node->right, features);
		}		
	}
	else
	{
		buildLeafsForCurrentNode(node, features);
	}
}

void BinaryTree::buildTree(Features features)
{
	buildLeafs(root, features);
}