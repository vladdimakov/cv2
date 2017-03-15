#include "lib.h"

void binaryTreeTest()
{
	const int featuresNum = 50;
	const int statisticsNum = 100;

	BinaryTree binaryTree(featuresNum, statisticsNum);
	Features features(featuresNum);
	
	for (int k = 0; k < 10; k++)
	{
		for (int i = 0; i < featuresNum; i++)
		{
			features.values[i] = false;
		}
		features.values[0] = false;
		features.isTarget = false;
		binaryTree.buildTree(features);
	}
	for (int k = 0; k < 20; k++)
	{
		for (int i = 0; i < featuresNum; i++)
		{
			features.values[i] = false;
		}
		features.values[0] = false;
		features.isTarget = true;
		binaryTree.buildTree(features);
	}
	for (int k = 0; k < 9; k++)
	{
		for (int i = 0; i < featuresNum; i++)
		{
			features.values[i] = false;
		}
		features.values[0] = true;
		features.isTarget = false;
		binaryTree.buildTree(features);
	}
	for (int k = 0; k < 21; k++)
	{
		for (int i = 0; i < featuresNum; i++)
		{
			features.values[i] = false;
		}
		features.values[0] = true;
		features.isTarget = true;
		binaryTree.buildTree(features);
	}

	for (int k = 0; k < 10; k++)
	{
		for (int i = 0; i < featuresNum; i++)
		{
			features.values[i] = false;
		}
		features.values[1] = false;
		features.isTarget = false;
		binaryTree.buildTree(features);
	}
	for (int k = 0; k < 10; k++)
	{
		for (int i = 0; i < featuresNum; i++)
		{
			features.values[i] = false;
		}
		features.values[1] = false;
		features.isTarget = true;
		binaryTree.buildTree(features);
	}
	for (int k = 0; k < 10; k++)
	{
		for (int i = 0; i < featuresNum; i++)
		{
			features.values[i] = false;
		}
		features.values[1] = true;
		features.isTarget = false;
		binaryTree.buildTree(features);
	}
	for (int k = 0; k < 10; k++)
	{
		for (int i = 0; i < featuresNum; i++)
		{
			features.values[i] = false;
		}
		features.values[1] = true;
		features.isTarget = true;
		binaryTree.buildTree(features);
	}






		for (int i = 0; i < featuresNum; i++)
		{
			features.values[i] = false;
		}
		features.values[49] = false;
		features.values[0] = false;
		features.isTarget = true;
		binaryTree.buildTree(features);


	
	//cout << binaryTree.root->childs[0].statistics[0][0] << " " << binaryTree.root->childs[0].statistics[0][1] << " " << binaryTree.root->childs[0].statistics[1][0] << " " << binaryTree.root->childs[0].statistics[1][1] << endl;
	//cout << binaryTree.root->childs[1].statistics[0][0] << " " << binaryTree.root->childs[1].statistics[0][1] << " " << binaryTree.root->childs[1].statistics[1][0] << " " << binaryTree.root->childs[1].statistics[1][1] << endl;
	
	Node* currentNode = binaryTree.root;
	//cout << currentNode->statistics[0] << " " << currentNode->statistics[1] << endl;
	
	//cout << currentNode->childs[49].statistics[0][0] << " " << currentNode->childs[49].statistics[0][1] << " " << currentNode->childs[49].statistics[1][0] << " " << binaryTree.root->childs[49].statistics[1][1] << endl;
	
	//cout << currentNode->featureNumToDivide << endl;
	cout << currentNode->left->statistics[0] << " " << currentNode->left->statistics[1] << "        " << currentNode->right->statistics[0] << " " << currentNode->right->statistics[1] << endl;
	//cout << currentNode->right->childs[49].statistics[1][1] << endl;
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	CVFuns cvFuns;

	const float refreshRate = 0.02f;
    const float deviationFactor = 5.5f;
	const float targetsFactor = 15.0f;
	const float scalingFactor = 20.0f;
	cvFuns.deviationImgFillValue = 256.0f / targetsFactor;
	const float distanceBetweenTargets = 50;

	Mat colorFrame, grayFrame8U, grayFrame32F;
	Point2f currentOffset;

	binaryTreeTest();

	if (!cvFuns.startCapture(argc, argv)) 
		return -1;

	while (true)
	{
		cvFuns.cap >> colorFrame;
		cvtColor(colorFrame, grayFrame8U, CV_RGB2GRAY);
		grayFrame8U.convertTo(grayFrame32F, CV_32F);

		if (grayFrame8U.empty())
		{
			cvFuns.startCapture(argc, argv);
			continue;
		}

		currentOffset = cvFuns.calcFrameOffset(grayFrame8U);
		
		cvFuns.translateAverageBackAndDeviationImg(grayFrame32F, currentOffset);

		cvFuns.calcFrameStaticPartMask(grayFrame32F, deviationFactor);

		cvFuns.calcAverageBackAndDeviationImg(grayFrame32F, refreshRate);

		cvFuns.brightestScaling(cvFuns.deviationImg, scalingFactor);

		cvFuns.calcTargetsBinaryFrame(grayFrame32F, targetsFactor);

		cvFuns.makeSegmentation(distanceBetweenTargets);
		
		cvFuns.makeIntegralImg(grayFrame8U);

		cvFuns.displayWindow();

		char key = (char)waitKey(1); // waitKey ждет событи€ нажати€ клавиши 1 мс
		if (key == 27) // ≈сли нажат ESC - выходим
		{
			break;
		}
		else if (key == ' ')
		{
			cvFuns.needToInit = true;
			continue;
		}
	}

	return 0;
}
