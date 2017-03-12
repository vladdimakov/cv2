#include <iostream>

using namespace std;

const int featuresNum = 50;
const int statisticsNum = 100;

struct Feature
{
	bool value;
	int num;
	bool isTarget;
};

struct Child
{
	int M[2], T[2];
};

struct Node
{
	int data[2];
	bool dataFilled = false;
	Node* left = NULL;
	Node* right = NULL;
	Child childs[featuresNum];
};

class Tree
{
public:
	Tree();
	void fillData(Node* node, Feature feature);
	float calcGiniCoefficient(Child child);
	//void divideNode(Node* node);
	void buildLeafs(Node* node, Feature feature);
	void buildTree(Feature feature);

	Node *root = new Node;
};

Tree::Tree()
{
	root->data[0] = 0;
	root->data[1] = 0;
	/*
	for (int i = 0; i < featuresNum; i++)
	{
		root->left->childs[i].M[0] = 0;
		root->left->childs[i].M[1] = 0;
		root->left->childs[i].T[0] = 0;
		root->left->childs[i].T[1] = 0;
	}

	for (int i = 0; i < featuresNum; i++)
	{
		root->right->childs[i].M[0] = 0;
		root->right->childs[i].M[1] = 0;
		root->right->childs[i].T[0] = 0;
		root->right->childs[i].T[1] = 0;
	}
	*/
}

void Tree::fillData(Node* node, Feature feature)
{
	if (!feature.isTarget)
		node->data[0]++;
	else
		node->data[1]++;
	
	if (node->data[0] + node->data[1] >= statisticsNum)
		node->dataFilled = true;

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
}

float calcGiniCoefficient(Child child)
{
	float giniCoefficient = 0;

	for (int j = 0; j < 2; j++)
	{
		giniCoefficient += (float)(child.M[j] * child.M[j] + child.T[j] * child.T[j]) / (child.M[j] + child.T[j]);
	}

	return giniCoefficient;
}
/*
void divideNode(Node* node)
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
*/
void Tree::buildLeafs(Node* node, Feature feature)
{
	if (node->left == NULL && node->right == NULL)
	{
		if (!node->dataFilled)
			fillData(node, feature);
		else{}
			//divideNode(node);
	}
	else
	{
		buildLeafs(node->left, feature);
		buildLeafs(node->right, feature);
	}
}

void Tree::buildTree(Feature feature)
{
	buildLeafs(root, feature);
}

void main()
{
	//Feature features[100];
	Tree tree;
	/*
	for (int i = 0; i < 100; i++)
	{
		features[i].num = i;
		features[i].value = true;
		features[i].isTarget = true;
	}

	for (int i = 0; i < 100; i++)
	{
		tree.buildTree(features[i]);
	}
	*/
}