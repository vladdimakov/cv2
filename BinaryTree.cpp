#include "lib.h"

BinaryTree::BinaryTree()
{
	root = new Node;

	root->data[0] = 0;
	root->data[1] = 0;

	for (int i = 0; i < featuresNum; i++)
	{
		root->childs[i].M[0] = 0;
		root->childs[i].M[1] = 0;
		root->childs[i].T[0] = 0;
		root->childs[i].T[1] = 0;
	}
}

void BinaryTree::fillData(Node* node, Feature feature)
{
	if (!feature.isTarget)
		node->data[0]++;
	else
		node->data[1]++;

	if (feature.value == false)
	{
		if (feature.isTarget == false)
			node->childs[feature.num].M[0]++;
		else
			node->childs[feature.num].M[1]++;
	}
	else
	{
		if (feature.isTarget == false)
			node->childs[feature.num].T[0]++;
		else
			node->childs[feature.num].T[1]++;
	}

	if (node->data[0] + node->data[1] >= statisticsNum)
		divideNode(node);
}

float BinaryTree::calcGiniCoefficient(Child child)
{
	float giniCoefficient = 0;

	for (int j = 0; j < 2; j++)
	{
		if (child.M[j] + child.T[j] != 0)
			giniCoefficient += (float)(child.M[j] * child.M[j] + child.T[j] * child.T[j]) / (child.M[j] + child.T[j]); // Может быть бесконечность
	}

	return giniCoefficient;
}

void BinaryTree::divideNode(Node* node)
{
	float giniCoefficients[featuresNum];
	for (int i = 0; i < featuresNum; i++)
	{
		giniCoefficients[i] = calcGiniCoefficient(node->childs[i]);
	}

	float maxGiniCoefficient = 0;
	int childWithMaxGiniCoefficient;
	for (int i = 0; i < featuresNum; i++)
	{
		if (giniCoefficients[i] >= maxGiniCoefficient)
		{
			maxGiniCoefficient = giniCoefficients[i];
			childWithMaxGiniCoefficient = i;
		}
	}

	node->left = new Node;
	node->left->data[0] = node->childs[childWithMaxGiniCoefficient].M[0];
	node->left->data[1] = node->childs[childWithMaxGiniCoefficient].M[1];

	for (int i = 0; i < featuresNum; i++)
	{
		node->left->childs[i].M[0] = 0;
		node->left->childs[i].M[1] = 0;
		node->left->childs[i].T[0] = 0;
		node->left->childs[i].T[1] = 0;
	}

	node->right = new Node;
	node->right->data[0] = node->childs[childWithMaxGiniCoefficient].T[0];
	node->right->data[1] = node->childs[childWithMaxGiniCoefficient].T[1];

	for (int i = 0; i < featuresNum; i++)
	{
		node->right->childs[i].M[0] = 0;
		node->right->childs[i].M[1] = 0;
		node->right->childs[i].T[0] = 0;
		node->right->childs[i].T[1] = 0;
	}
}

void BinaryTree::buildLeafs(Node* node, Feature feature)
{
	if (node->left == NULL && node->right == NULL)
	{
		fillData(node, feature);
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