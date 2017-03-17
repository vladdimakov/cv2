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

BinaryTree::BinaryTree(int featureType, int featuresNum, int statisticsNum, int depthOfTree)
{
	this->featureType = featureType;
	this->featuresNum = featuresNum;
	this->statisticsNum = statisticsNum;
	this->depthOfTree = depthOfTree;

	root = new Node(featuresNum);	
	root->level = 0;

	featuresPositions = new Object[featuresNum];
	
	// ѕусть эталонное окно имеет размер координаты (0,0) (100,100)
	int windowWidth = 100;
	int windowHeight = 100;
	
	int maxFeatureWidth = 80;
	int minFeatureWidth = 4;
	int maxFeatureHeight = 80;
	int minFeatureHeight = 4;

	for (int i = 0; i < featuresNum; i++)
	{
		featuresPositions[i].top = rand() % (windowHeight - minFeatureHeight);
		featuresPositions[i].left = rand() % (windowWidth - minFeatureWidth);

		featuresPositions[i].bottom = featuresPositions[i].top + minFeatureHeight + rand() % (maxFeatureHeight - minFeatureHeight);
		if (featuresPositions[i].bottom >= windowHeight)
			featuresPositions[i].bottom = windowHeight - 1;

		featuresPositions[i].right = featuresPositions[i].left + minFeatureWidth + rand() % (maxFeatureWidth - minFeatureWidth);
		if (featuresPositions[i].right >= windowWidth)
			featuresPositions[i].right = windowWidth - 1;
	}
}

void BinaryTree::buildLeafsForCurrentNode(Node* node, Features features)
{	
	node->statistics[features.isTarget]++;

	if (node->level < depthOfTree)
	{
		for (int i = 0; i < featuresNum; i++)
		{
			if (features.values[i] == -1)
				continue;

			node->childs[i].statistics[features.values[i]][features.isTarget]++;
		}

		if (node->statistics[0] + node->statistics[1] >= statisticsNum)
		{
			divideNode(node);
		}
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
	float *giniCoefficients = new float[featuresNum];
	for (int i = 0; i < featuresNum; i++)
	{
		giniCoefficients[i] = calcGiniCoefficient(node->childs[i]);
	}

	float maxGiniCoefficient = 0;
	int maxGiniCoefficientNum;
	for (int i = 0; i < featuresNum; i++)
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

	node->left = new Node(featuresNum);
	node->left->level = node->level + 1;

	node->right = new Node(featuresNum);
	node->right->level = node->level + 1;
	
	cout << node->level + 1 << endl; ///
	cout << node->level + 1 << endl;

	node->removeChilds();
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