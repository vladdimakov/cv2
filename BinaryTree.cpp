#include "lib.h"

BinaryTree::BinaryTree(int depthOfTree, int featuresNum, int randomlySelectedFeaturesNum, int minStatisticsNumForDivide, float minGiniCoefficient)
{
	this->depthOfTree = depthOfTree;
	this->featuresNum = featuresNum;
	this->randomlySelectedFeaturesNum = randomlySelectedFeaturesNum;
	this->minStatisticsNumForDivide = minStatisticsNumForDivide;
	this->minGiniCoefficient = minGiniCoefficient;

	nodesNum = 1;

	root = new Node(featuresNum, randomlySelectedFeaturesNum);	
	root->level = 0;
	root->num = nodesNum;

	makeRandomlySelectedFeatures(NULL, root);

	featuresPositions = new Object[featuresNum];
	
	// Пусть эталонное окно имеет размер координаты (0,0) (100,100)
	int windowWidth = 100;
	int windowHeight = 100;
	
	int maxFeatureWidth = 100;
	int minFeatureWidth = 20;
	int maxFeatureHeight = 100;
	int minFeatureHeight = 20;

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

	featuresTypes = new int[featuresNum];

	for (int i = 0; i < featuresNum; i++)
	{
		featuresTypes[i] = rand() % 8;
	}
}

void BinaryTree::buildNode(Node* node, Features* features)
{	
	node->statistics[features->isTarget]++;

	if (node->level < depthOfTree)
	{
		for (int i = 0; i < featuresNum; i++)
		{
			if (features->values[i] == -1)
				continue;

			node->childs[i].statistics[features->values[i]][features->isTarget]++;
		}


		if (node->statistics[0] + node->statistics[1] >= minStatisticsNumForDivide)
		{
			divideNode(node);
		}
	}
}

float BinaryTree::calcGiniCoefficient(Child child)
{
	if (child.statistics[0][0] == 0 && child.statistics[0][1] == 0 || // Нельзя, чтобы M1 = 0 и M2 = 0
		child.statistics[1][0] == 0 && child.statistics[1][1] == 0)   // Нельзя, чтобы T1 = 0 и T2 = 0
	{
		return -1.0f;
	}
	
	float giniCoefficient = 0;
	for (int j = 0; j < 2; j++)
	{
		giniCoefficient += (float)(child.statistics[j][0] * child.statistics[j][0] + child.statistics[j][1] * child.statistics[j][1]) /
							(child.statistics[j][0] + child.statistics[j][1]);
	}

	return giniCoefficient;
}

void BinaryTree::makeRandomlySelectedFeatures(Node* parent, Node* child)
{
	if (parent != NULL)
	{
		child->previousSelectedFeatures = new int[child->level];
		for (int i = 0; i < child->level - 1; i++)
			child->previousSelectedFeatures[i] = parent->previousSelectedFeatures[i];
		
		child->previousSelectedFeatures[child->level - 1] = parent->featureNumToDivide;
	}

	bool isSelected;
	int currentSelectedFeature;
	for (int i = 0; i < randomlySelectedFeaturesNum; i++)
	{
		isSelected = false;
		while (!isSelected)
		{
			currentSelectedFeature = rand() % featuresNum;
			isSelected = true;

			for (int j = 0; j < child->level; j++)
			{
				if (currentSelectedFeature == child->previousSelectedFeatures[j])
				{
					isSelected = false;
					break;
				}
			}

			if (i > 0)
			{
				for (int j = 0; j < i; j++)
				{
					if (currentSelectedFeature == child->randomlySelectedFeatures[j])
					{
						isSelected = false;
						break;
					}
				}
			}
		}

		child->randomlySelectedFeatures[i] = currentSelectedFeature;
	}
}

void BinaryTree::divideNode(Node* node)
{
	float *giniCoefficients = new float[randomlySelectedFeaturesNum];
	for (int i = 0; i < randomlySelectedFeaturesNum; i++)
	{
		giniCoefficients[i] = calcGiniCoefficient(node->childs[node->randomlySelectedFeatures[i]]);
	}

	float maxGiniCoefficient = 0;
	int maxGiniCoefficientNum = -1;
	for (int i = 0; i < randomlySelectedFeaturesNum; i++)
	{
		if (giniCoefficients[i] >= maxGiniCoefficient)
		{
			maxGiniCoefficient = giniCoefficients[i];
			maxGiniCoefficientNum = node->randomlySelectedFeatures[i];
		}
	}

	delete[] giniCoefficients;
	
	if (maxGiniCoefficientNum == -1 || maxGiniCoefficient < minGiniCoefficient)
		return;
		
	node->featureNumToDivide = maxGiniCoefficientNum;
	
	nodesNum++;
	node->left = new Node(featuresNum, randomlySelectedFeaturesNum);
	node->left->statistics[0] = node->childs[maxGiniCoefficientNum].statistics[0][0];
	node->left->statistics[1] = node->childs[maxGiniCoefficientNum].statistics[0][1];
	node->left->level = node->level + 1;
	node->left->num = nodesNum;
	
	makeRandomlySelectedFeatures(node, node->left);
	
	nodesNum++;
	node->right = new Node(featuresNum, randomlySelectedFeaturesNum);
	node->right->statistics[0] = node->childs[maxGiniCoefficientNum].statistics[1][0];
	node->right->statistics[1] = node->childs[maxGiniCoefficientNum].statistics[1][1];
	node->right->level = node->level + 1;
	node->right->num = nodesNum;
	
	makeRandomlySelectedFeatures(node, node->right);
	
	node->removeChilds();
}

void BinaryTree::buildTree(Node* node, Features* features)
{
	if (node->featureNumToDivide != -1)
	{
		if (features->values[node->featureNumToDivide] == 0)
		{
			features->values[node->featureNumToDivide] = -1;
			buildTree(node->left, features);
		}
		else
		{
			features->values[node->featureNumToDivide] = -1;
			buildTree(node->right, features);
		}		
	}
	else
	{
		buildNode(node, features);
	}
}

bool BinaryTree::classifyFeatures(Node* node, Features* features)
{
	if (node->featureNumToDivide != -1)
	{
		if (features->values[node->featureNumToDivide] == 0)
		{
			classifyFeatures(node->left, features);
		}
		else
		{
			classifyFeatures(node->right, features);
		}
	}
	else
	{
		return node->statistics[1] > node->statistics[0];
	}
}