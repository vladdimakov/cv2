#include "lib.h"

Forest::Forest()
{
	treesNum = 3;

	trees = new BinaryTree*[treesNum];
	
	//trees[0] = new BinaryTree(0, 10, 50);
	//trees[1] = new BinaryTree(1, 10, 50);

	//trees[2] = new BinaryTree(2, 10, 20);
	//trees[3] = new BinaryTree(3, 10, 20);

	//trees[4] = new BinaryTree(4, 25, 50); // Очень хороший вариант
	//trees[5] = new BinaryTree(5, 25, 50);
	//trees[6] = new BinaryTree(6, 25, 50);
	//trees[7] = new BinaryTree(7, 25, 50);

	//trees[8] = new BinaryTree(8, 25, 100); // Плохой вариант

	//trees[9] = new BinaryTree(9, 50, 50); // Неплохой вариант
	//trees[10] = new BinaryTree(10, 50, 50);
	//trees[11] = new BinaryTree(11, 50, 50);

	//trees[12] = new BinaryTree(12, 50, 100); // Неплохой вариант
	//trees[13] = new BinaryTree(13, 50, 100);
	//trees[14] = new BinaryTree(14, 50, 100);

	//trees[0] = new BinaryTree(15, 50, 150); // Очень хороший вариант
	//trees[1] = new BinaryTree(16, 50, 150);
	//trees[2] = new BinaryTree(17, 50, 150);
	//trees[11] = new BinaryTree(18, 50, 150);

	trees[0] = new BinaryTree(0, 50, 150); 
	trees[1] = new BinaryTree(1, 50, 100);
	trees[2] = new BinaryTree(2, 25, 50);
}

void Forest::buildTree(int treeNum, Features* features)
{
	trees[treeNum]->buildTree(trees[treeNum]->root, features);
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

	return voteYesNum  > voteNoNum;
}