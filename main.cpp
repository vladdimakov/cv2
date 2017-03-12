#include "lib.h"

void binaryTreeTest()
{
	BinaryTree binaryTree;
	Feature feature;

	if (binaryTree.root->left == NULL && binaryTree.root->right == NULL)
		cout << "NULL" << endl;

	for (int i = 0; i < 10; i++)
	{
		feature.num = 0;
		feature.value = false;
		feature.isTarget = false;
		binaryTree.buildTree(feature);
	}
	for (int i = 0; i < 20; i++)
	{
		feature.num = 0;
		feature.value = false;
		feature.isTarget = true;
		binaryTree.buildTree(feature);
	}
	for (int i = 0; i < 9; i++)
	{
		feature.num = 0;
		feature.value = true;
		feature.isTarget = false;
		binaryTree.buildTree(feature);
	}
	for (int i = 0; i < 21; i++)
	{
		feature.num = 0;
		feature.value = true;
		feature.isTarget = true;
		binaryTree.buildTree(feature);
	}
	for (int i = 0; i < 10; i++)
	{
		feature.num = 1;
		feature.value = false;
		feature.isTarget = false;
		binaryTree.buildTree(feature);
	}
	for (int i = 0; i < 10; i++)
	{
		feature.num = 1;
		feature.value = false;
		feature.isTarget = true;
		binaryTree.buildTree(feature);
	}
	for (int i = 0; i < 10; i++)
	{
		feature.num = 1;
		feature.value = true;
		feature.isTarget = false;
		binaryTree.buildTree(feature);
	}
	for (int i = 0; i < 10; i++)
	{
		feature.num = 1;
		feature.value = true;
		feature.isTarget = true;
		binaryTree.buildTree(feature);
	}


	if (binaryTree.root->left != NULL && binaryTree.root->right != NULL)
		cout << "NOT NULL" << endl;

	cout << binaryTree.root->left->data[0] << " " << binaryTree.root->left->data[1] << " " << binaryTree.root->right->data[0] << " " << binaryTree.root->right->data[1] << endl;


	//cout << binaryTree.root->childs[0].M[0] << " " << binaryTree.root->childs[0].M[1] << " " << binaryTree.root->childs[0].T[0] << " " << binaryTree.root->childs[0].T[1] << endl;

	//cout << binaryTree.root->childs[1].M[0] << " " << binaryTree.root->childs[1].M[1] << " " << binaryTree.root->childs[1].T[0] << " " << binaryTree.root->childs[1].T[1] << endl;
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

		char key = (char)waitKey(1); // waitKey ���� ������� ������� ������� 1 ��
		if (key == 27) // ���� ����� ESC - �������
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
