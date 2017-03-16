#pragma once

#include "opencv2/opencv.hpp"
#include <time.h> 

using namespace cv;
using namespace std;

const int CAP_FRAME_WIDTH = 640;
const int CAP_FRAME_HEIGHT = 480;
const int CAP_FPS = 20;

const int MAX_CORNERS_NUM = 64;
const int MIN_CORNERS_NUM = 16;

const int WINDOW_WIDTH = 853;
const int WINDOW_HEIGHT = 660;

const int FEATURES_NUM = 10;
const int STATISTICS_NUM = 100;
const int DEPTH_OF_TREE = 10;

class FPSCounter
{
public:
	FPSCounter();
	size_t value();
private:
	clock_t oldTime;
	size_t FPSCnt, oldFPS;
};

struct Object
{
	int left, right, top, bottom;
	Point2f center;
	bool exist;
};

class Features
{
public:
	Features(int featuresNum);
	~Features();
	int *values, isTarget;
};

class Child
{
public:
	Child();

	int statistics[2][2];
};

class Node
{
public:
	Node(int childsNum);
	void removeChilds();

	int statistics[2];
	int level, featureNumToDivide;
	Node *left, *right;
	Child *childs;
};

class BinaryTree
{
public:
	BinaryTree(int featuresNum, int statisticsNum, int depthOfTree);
	void buildLeafsForCurrentNode(Node* node, Features features);
	float calcGiniCoefficient(Child child);
	bool isUsedFeature(int featureNum, vector<int> usedFeatures);
	void divideNode(Node* node);
	void buildLeafs(Node* node, Features features);
	void buildTree(Features features);
	void test();

	Node *root;
	Object *_features;
	int builtLeafsNum;

	int _featuresNum, _statisticsNum, _depthOfTree;
};

class CVFuns
{
public:
	CVFuns();
	void displayWindow();
	bool startCapture(int argc, char* argv[]);
	vector<Point2f> findCorners(Mat grayFrame, int maxCornersNum);
	void calcOpticalFlow(Mat prevGrayFrame, Mat currentGrayFrame, vector<Point2f> prevPoints, vector<Point2f>& currentPoints, vector<uchar>& status);
	void translateFrame(Mat inputFrame, Mat& outputFrame, Point2f offset);
	Mat subPixTranslateFrame(Mat inputFrame, Point2f subPixOffset);
	Mat subPixTranslateFrameOpenCV(Mat inputFrame, Point2f subPixOffset);
	float findMedian(vector<float> value);
	Point2f findOffsetMedian(vector<Point2f> prevPoints, vector<Point2f> currentPoints);
	void makeInitialFrame(Mat prevGrayFrame, vector<Point2f>& prevPoints);
	Point2f calcFrameOffset(Mat& currentGrayFrame);
	void translateAverageBackAndDeviationImg(Mat currentFrame, Point2f currentOffset);
	void calcFrameStaticPartMask(Mat currentFrame, float deviationFactor);
	void showFrameStaticPartMask();
	void calcAverageBackAndDeviationImg(Mat currentFrame, float refreshRate);
	void brightestScaling(Mat frame, float scalingFactor);
	int getBackgroundBound(Mat frame);
	int getBackgroundBoundOpenCV(Mat frame);
	void calcTargetsBinaryFrame(Mat currentFrame, float targetsFactor);
	void findConnectedPoints(int x, int y, vector<Point2i>& connectedPoints);
	void makeSegmentation(float distanceBetweenTargets);
	void selectTarget(Point2i clickedPoint);
	void findSelectedTarget(float distanceBetweenTargetsOnTwoFrames);
	void displaySelectedTarget();

	void makeIntegralImg(Mat currentFrame);
	int calcIntegralSumForRectangle(Object rectangle);
	bool haarFeature1(Object rectangle);
	bool haarFeature2(Object rectangle);
	void makeFeaturesForWindow(Object featuresWindow, int isTarget);
	void calcFeatures();

	VideoCapture cap;
	vector<Mat> imgToDisplay;
	string imgToDisplayInfo[4];
	Point2f offset;
	Mat frameStaticPartMask, averageBackImg, deviationImg, targetsBinaryFrame, integralImg;
	bool needToInit;
	float deviationImgFillValue;
	
	vector<Object> objects;
	bool isTargetSelected;
	Object selectedTarget;

	BinaryTree *tree;

private:
	Mat prevGrayFrame, currentDeviationImg, frameWith0, frameWith255;
	vector<Point2f> prevPoints, currentPoints;
	FPSCounter FPScounter;
};