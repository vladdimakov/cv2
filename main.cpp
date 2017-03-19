#include "lib.h"

Point2i clickedPoint;
bool isClicked = false;

void mouseCallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	if (event == EVENT_LBUTTONDOWN && x < CAP_FRAME_WIDTH && y < CAP_FRAME_HEIGHT)
	{
		clickedPoint = Point2i(x, y);
		isClicked = true;
	}
}

void binaryTreeTest()
{
	const int featuresNum = 50;
	const int statisticsNum = 100;
	const int depthOfTree = 3;

	BinaryTree binaryTree(1, featuresNum, statisticsNum, depthOfTree);
	Features features(featuresNum);
	
	for (int k = 0; k < 5000; k++)
	{
		for (int i = 0; i < featuresNum; i++)
		{
			features.values[i] = rand() % 2;
		}
		features.isTarget = rand() % 2;
		binaryTree.buildTree(binaryTree.root, features);
	}
	
	//cout << binaryTree.root->childs[0].statistics[0][0] << " " << binaryTree.root->childs[0].statistics[0][1] << " " << binaryTree.root->childs[0].statistics[1][0] << " " << binaryTree.root->childs[0].statistics[1][1] << endl;
	//cout << binaryTree.root->childs[1].statistics[0][0] << " " << binaryTree.root->childs[1].statistics[0][1] << " " << binaryTree.root->childs[1].statistics[1][0] << " " << binaryTree.root->childs[1].statistics[1][1] << endl;
	
	Node* currentNode = binaryTree.root->left->left->left;

	//cout << currentNode->level << endl;

	cout << currentNode->statistics[0] << " " << currentNode->statistics[1] << endl;
	
	//cout << currentNode->childs[49].statistics[0][0] << " " << currentNode->childs[49].statistics[0][1] << " " << currentNode->childs[49].statistics[1][0] << " " << binaryTree.root->childs[49].statistics[1][1] << endl;
	
	//cout << currentNode->featureNumToDivide << endl;
	//cout << currentNode->left->statistics[0] << " " << currentNode->left->statistics[1] << "        " << currentNode->right->statistics[0] << " " << currentNode->right->statistics[1] << endl;
	//cout << currentNode->right->childs[49].statistics[1][1] << endl;
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	srand(time(NULL));

	CVFuns cvFuns;

	cvFuns.tree->readTree("1.txt");
	cvFuns.tree->writeTree("2.txt");

	const float refreshRate = 0.02f;
    const float deviationFactor = 5.5f;
	const float targetsFactor = 15.0f;
	const float scalingFactor = 20.0f;
	cvFuns.deviationImgFillValue = 256.0f / targetsFactor;
	const float distanceBetweenTargets = 50.0f;
	const float distanceBetweenTargetsOnTwoFrames = 50.0f;

	Mat colorFrame, grayFrame8U, grayFrame32F;
	Point2f currentOffset;

	//binaryTreeTest();

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
			
			cvFuns.tree->isTrained = true;
			cout << "Классификатор обучен, началась запись в файл" << endl;
			cvFuns.tree->writeTree("1.txt");

			continue;
		}

		currentOffset = cvFuns.calcFrameOffset(grayFrame8U);
		
		cvFuns.translateAverageBackAndDeviationImg(grayFrame32F, currentOffset);

		cvFuns.calcFrameStaticPartMask(grayFrame32F, deviationFactor);

		cvFuns.calcAverageBackAndDeviationImg(grayFrame32F, refreshRate);

		cvFuns.brightestScaling(cvFuns.deviationImg, scalingFactor);

		cvFuns.calcTargetsBinaryFrame(grayFrame32F, targetsFactor);

		cvFuns.makeSegmentation(distanceBetweenTargets);

		if (cvFuns.isTargetSelected && !cvFuns.tree->isTrained)
		{
			cvFuns.findSelectedTarget(distanceBetweenTargetsOnTwoFrames);
			cvFuns.calcFeatures();
			cvFuns.displaySelectedTarget();
		}
				
		cvFuns.displayWindow();

		cvFuns.makeIntegralImg(grayFrame8U);

		setMouseCallback("Display window", mouseCallBackFunc, NULL);

		if (isClicked && !cvFuns.isTargetSelected && !cvFuns.tree->isTrained)
		{
			cvFuns.selectTarget(clickedPoint);
			isClicked = false;
		}

		char key = (char)waitKey(1); // waitKey ждет события нажатия клавиши 1 мс
		if (key == 27) // Если нажат ESC - выходим
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
