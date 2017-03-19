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
	Mat colorFrame, grayFrame8U, grayFrame32F;
	Point2f currentOffset;

	const float refreshRate = 0.02f;
	const float deviationFactor = 5.5f;
	const float targetsFactor = 15.0f;
	const float scalingFactor = 20.0f;
	cvFuns.deviationImgFillValue = 256.0f / targetsFactor;
	const float distanceBetweenTargets = 50.0f;
	const float distanceBetweenTargetsOnTwoFrames = 50.0f;
	
	string videoSource;
	string classificatorMode;
	if (argc == 2 || argc == 3)
	{
		if (argc == 2)
		{
			videoSource = "";
		}
		else
		{
			videoSource = argv[2];
		}

		classificatorMode = argv[1];
		if (classificatorMode == "t")
		{
			cout << "Режим обучения классификатора" << endl;
		}
		else if (classificatorMode == "c")
		{
			cout << "Режим классификации" << endl;
		}
		else
		{
			cout << "Неверный режим работы классификатора" << endl;
			return -1;
		}
	}
	else
	{
		cout << "Неверное число аргументов командной строки" << endl;
		return -1;
	}

	//binaryTreeTest();

	if (!cvFuns.startCapture(videoSource))
		return -1;

	if (classificatorMode == "c")
		cvFuns.tree->readTree("1.txt");

	while (true)
	{
		cvFuns.cap >> colorFrame;
		cvtColor(colorFrame, grayFrame8U, CV_RGB2GRAY);
		grayFrame8U.convertTo(grayFrame32F, CV_32F);

		if (grayFrame8U.empty())
		{
			if (classificatorMode == "t")
			{
				cvFuns.tree->isTrained = true;
				cvFuns.tree->writeTree("1.txt");
			}

			cvFuns.startCapture(videoSource);

			continue;
		}

		if (classificatorMode == "t")
		{
			currentOffset = cvFuns.calcFrameOffset(grayFrame8U);
			cvFuns.translateAverageBackAndDeviationImg(grayFrame32F, currentOffset);
			cvFuns.calcFrameStaticPartMask(grayFrame32F, deviationFactor);
			cvFuns.calcAverageBackAndDeviationImg(grayFrame32F, refreshRate);
			cvFuns.brightestScaling(cvFuns.deviationImg, scalingFactor);
			cvFuns.calcTargetsBinaryFrame(grayFrame32F, targetsFactor);
			cvFuns.makeSegmentation(distanceBetweenTargets);
		}

		cvFuns.makeIntegralImg(grayFrame8U);

		if (classificatorMode == "c")
		{
			cvFuns.imgToDisplay[0] = grayFrame8U;
			cvFuns.calcFeaturesForClassification();
		}

		if (classificatorMode == "t" && cvFuns.isTargetSelected && !cvFuns.tree->isTrained)
		{
			cvFuns.findSelectedTarget(distanceBetweenTargetsOnTwoFrames);
			cvFuns.calcFeaturesForTraining();
			cvFuns.displaySelectedTarget();
		}
		
		cvFuns.displayWindow();

		if (classificatorMode == "t")
		{
			setMouseCallback("Display window", mouseCallBackFunc, NULL);

			if (isClicked && !cvFuns.isTargetSelected && !cvFuns.tree->isTrained)
			{
				cvFuns.selectTarget(clickedPoint);
				isClicked = false;
			}
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
