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
	memset(leftStatistics, 0, sizeof(leftStatistics));
	memset(rightStatistics, 0, sizeof(rightStatistics));
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
	if (!features.isTarget)
		node->statistics[0]++;
	else
		node->statistics[1]++;

	for (int i = 0; i < _featuresNum; i++)
	{
		if (features.values[i] == false)
		{
			if (features.isTarget == false)
				node->childs[i].leftStatistics[0]++;
			else
				node->childs[i].leftStatistics[1]++;
		}
		if (features.values[i] == true)
		{
			if (features.isTarget == false)
				node->childs[i].rightStatistics[0]++;
			else
				node->childs[i].rightStatistics[1]++;
		}
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
		if (child.leftStatistics[j] + child.rightStatistics[j] != 0)
		{
			giniCoefficient += (float)(child.leftStatistics[j] * child.leftStatistics[j] + child.rightStatistics[j] * child.rightStatistics[j]) /
								(child.leftStatistics[j] + child.rightStatistics[j]);
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

	//cout << maxGiniCoefficientNum << endl;

	delete[] giniCoefficients;

	node->featureNumToDivide = maxGiniCoefficientNum;

	node->left = new Node(_featuresNum);	

	node->right = new Node(_featuresNum);

//	node->removeChilds();
}

void BinaryTree::buildLeafs(Node* node, Features features)
{
	if (node->left == NULL && node->right == NULL)
	{
		buildLeafsForCurrentNode(node, features);
	}
	else
	{
		if (node->featureNumToDivide == -1)
		{
			buildLeafs(node->left, features);
			buildLeafs(node->right, features);
		}
		else
		{
			if (features.values[node->featureNumToDivide] == false)
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
	}
}

void BinaryTree::buildTree(Features features)
{
	buildLeafs(root, features);
}