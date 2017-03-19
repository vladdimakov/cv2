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

	nodesNum = 1;

	root = new Node(featuresNum);	
	root->level = 0;
	root->num = nodesNum;
	root->prevStatisticsNum = 0;

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

		if (node->statistics[0] + node->statistics[1] - node->prevStatisticsNum >= statisticsNum)
		{
			divideNode(node);
		}
	}
}

float BinaryTree::calcGiniCoefficient(Child child)
{
	if (child.statistics[0][0] == 0 && child.statistics[0][1] == 0 || // Нельзя, чтобы M0 = 0 и M1 = 0
		child.statistics[1][0] == 0 && child.statistics[1][1] == 0) // Нельзя, чтобы T0 = 0 и T1 = 0
	{
		return -1.0f;
	}
	
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

	nodesNum++;
	node->left = new Node(featuresNum);
	node->left->statistics[0] = node->childs[maxGiniCoefficientNum].statistics[0][0];
	node->left->statistics[1] = node->childs[maxGiniCoefficientNum].statistics[0][1];
	node->left->prevStatisticsNum = node->left->statistics[0] + node->left->statistics[1];
	node->left->level = node->level + 1;
	node->left->num = nodesNum;
	
	nodesNum++;
	node->right = new Node(featuresNum);
	node->right->statistics[0] = node->childs[maxGiniCoefficientNum].statistics[1][0];
	node->right->statistics[1] = node->childs[maxGiniCoefficientNum].statistics[1][1];
	node->right->prevStatisticsNum = node->right->statistics[0] + node->right->statistics[1];
	node->right->level = node->level + 1;
	node->right->num = nodesNum;
	
	//cout << node->level + 1 << endl; ///
	//cout << node->level + 1 << endl;

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

void BinaryTree::writeNodes(Node* node, ofstream &file)
{
	if (node == root && node->featureNumToDivide != -1)
	{
		file << node->num << " 0 " << node->featureNumToDivide << " " << node->left->num << " " << node->right->num << endl;

		writeNodes(node->left, file);
		writeNodes(node->right, file);
	}
	else if (node->featureNumToDivide == -1)
	{
		file << node->num << " 2 " << node->statistics[0] << " " << node->statistics[1] << " 0 " << endl;
	}
	else
	{
		file << node->num << " 1 " << node->featureNumToDivide << " " << node->left->num << " " << node->right->num << endl;

		writeNodes(node->left, file);
		writeNodes(node->right, file);
	}
}

void BinaryTree::writeTree(string fileName)
{
	cout << "Началась запись дерева №" << featureType << " в файл" << endl;

	ofstream file(fileName);

	for (int i = 0; i < featuresNum; i++)
	{
		file << featuresPositions[i].left << " " << featuresPositions[i].right << " " << featuresPositions[i].top << " " << featuresPositions[i].bottom << endl;
	}

	file << nodesNum << endl;

	writeNodes(root, file);

	file.close();

	cout << "Дерево №" << featureType << " записано в файл (" << nodesNum << " вершин)" << endl;
}

void BinaryTree::buildNodesFromFile(Node* node, int nodesTmp[])
{
	for (int i = 0; i < nodesNum; i++)
	{
		if (node->num == nodesTmp[i])
		{
			if (nodesTmp[i + nodesNum] == 0 || nodesTmp[i + nodesNum] == 1)
			{
				node->featureNumToDivide = nodesTmp[i + nodesNum * 2];

				node->left = new Node(featuresNum);
				node->left->num = nodesTmp[i + nodesNum * 3];
				buildNodesFromFile(node->left, nodesTmp);

				node->right = new Node(featuresNum);
				node->right->num = nodesTmp[i + nodesNum * 4];
				buildNodesFromFile(node->right, nodesTmp);
			}
			else
			{
				node->statistics[0] = nodesTmp[i + nodesNum * 2];
				node->statistics[1] = nodesTmp[i + nodesNum * 3];
			}
		}
	}
}

void BinaryTree::readTree(string fileName)
{
	cout << "Началось чтение дерева №" << featureType << " из файла" << endl;

	ifstream file(fileName);

	for (int i = 0; i < featuresNum; i++)
	{
		file >> featuresPositions[i].left >> featuresPositions[i].right >> featuresPositions[i].top >> featuresPositions[i].bottom;
	}
	
	file >> nodesNum;
	
	int *nodesTmp = new int[5 * nodesNum];
	
	for (int i = 0; i < nodesNum; i++)
	{
		file >> nodesTmp[i] >> nodesTmp[i + nodesNum] >> nodesTmp[i + nodesNum * 2] >> nodesTmp[i + nodesNum * 3] >> nodesTmp[i + nodesNum * 4];
	}

	buildNodesFromFile(root, nodesTmp);

	delete[] nodesTmp;

	file.close();

	cout << "Дерево №" << featureType << " прочитано из файла (" << nodesNum << " вершин)" << endl;
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

Forest::Forest()
{
	treesNum = 2;

	isTrained = false;

	trees = new BinaryTree*[treesNum];
	trees[0] = new BinaryTree(0, FEATURES_NUM, STATISTICS_NUM, DEPTH_OF_TREE);
	trees[1] = new BinaryTree(1, FEATURES_NUM, STATISTICS_NUM + 20, DEPTH_OF_TREE);
}

void Forest::buildForest(Features** features)
{
	for (int i = 0; i < treesNum; i++)
	{
		trees[i]->buildTree(trees[i]->root, features[i]);
	}
}

void Forest::writeForest()
{
	string fileName;
	for (int i = 0; i < treesNum; i++)
	{
		fileName = to_string(i) + ".txt";
		trees[i]->writeTree(fileName);
	}
}

void Forest::readForest()
{
	string fileName;
	for (int i = 0; i < treesNum; i++)
	{
		fileName = to_string(i) + ".txt";
		trees[i]->readTree(fileName);
	}
}

bool Forest::classifyFeatures(Features **features)
{
	int voteYesNum = 0;
	int voteNoNum = 0;

	for (int i = 0; i < treesNum; i++)
	{
		if (trees[i]->classifyFeatures(trees[i]->root, features[i]))
		{
			voteYesNum++;
		}
		else
		{
			voteNoNum++;
		}
	}

	return voteYesNum > voteNoNum;
}