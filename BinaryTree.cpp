#include "lib.h"

Child::Child()
{
	memset(leftStatistics, 0, sizeof(leftStatistics));
	memset(rightStatistics, 0, sizeof(rightStatistics));
}

Node::Node(int childsNum)
{
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

void BinaryTree::buildLeafsForCurrentNode(Node* node, Feature feature)
{
	if (!feature.isTarget)
		node->statistics[0]++;
	else
		node->statistics[1]++;

	if (feature.value == false)
	{
		if (feature.isTarget == false)
			node->childs[feature.num].leftStatistics[0]++;
		else
			node->childs[feature.num].leftStatistics[1]++;
	}
	else
	{
		if (feature.isTarget == false)
			node->childs[feature.num].rightStatistics[0]++;
		else
			node->childs[feature.num].rightStatistics[1]++;
	}

	if (node->statistics[0] + node->statistics[1] >= _statisticsNum)
		divideNode(node);
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

bool BinaryTree::isUsedFeature(int featureNum, vector<int> usedFeatures)
{
	for (int i = 0; i < usedFeatures.size(); i++)
	{
		if (featureNum == usedFeatures[i])
			return true;
	}

	return false;
}

void BinaryTree::divideNode(Node* node)
{
	float *giniCoefficients = new float[_featuresNum];
	for (int i = 0; i < _featuresNum; i++)
	{
		if (isUsedFeature(i, node->usedFeatures))
			giniCoefficients[i] = 0;
		else 
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

	cout << node->childs[maxGiniCoefficientNum].leftStatistics[0] << " " << node->childs[maxGiniCoefficientNum].leftStatistics[1] <<
		"     " << node->childs[maxGiniCoefficientNum].rightStatistics[0] << " " << node->childs[maxGiniCoefficientNum].rightStatistics[1] << endl;

	node->left = new Node(_featuresNum);
	node->left->statistics[0] = node->childs[maxGiniCoefficientNum].leftStatistics[0];
	node->left->statistics[1] = node->childs[maxGiniCoefficientNum].leftStatistics[1];
	node->left->usedFeatures.push_back(maxGiniCoefficientNum);

	
	node->right = new Node(_featuresNum);
	node->right->statistics[0] = node->childs[maxGiniCoefficientNum].rightStatistics[0];
	node->right->statistics[1] = node->childs[maxGiniCoefficientNum].rightStatistics[1];
	node->right->usedFeatures.push_back(maxGiniCoefficientNum);

	node->removeChilds();
}

void BinaryTree::buildLeafs(Node* node, Feature feature)
{
	if (node->left == NULL && node->right == NULL)
	{
		buildLeafsForCurrentNode(node, feature);
	}
	else
	{
		buildLeafs(node->left, feature);
		buildLeafs(node->right, feature);
	}
}

void BinaryTree::buildTree(Feature feature)
{
	buildLeafs(root, feature);
}