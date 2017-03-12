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

class FPSCounter
{
public:
	FPSCounter();
	size_t value();
private:
	clock_t oldTime;
	size_t FPSCnt, oldFPS;
};

struct Rectangle
{
	int left, right, top, bottom;
};

struct Target
{
	int left, right, top, bottom;
	Point2f center;
	bool exist;
};

struct Feature
{
	bool value;
	int num;
	bool isTarget;
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
	void makeIntegralImg(Mat currentFrame);
	int calcIntegralSumForRectangle(Rectangle rectangle);

	bool haarFeature1(Rectangle rectangle);
	bool haarFeature2(Rectangle rectangle);

	void calcFeatures();

	Feature features[50];
	VideoCapture cap;
	vector<Mat> imgToDisplay;
	string imgToDisplayInfo[4];
	Point2f offset;
	Mat frameStaticPartMask, averageBackImg, deviationImg, targetsBinaryFrame, integralImg;
	bool needToInit;
	float deviationImgFillValue;

private:
	Mat prevGrayFrame, currentDeviationImg, frameWith0, frameWith255;
	vector<Point2f> prevPoints, currentPoints;
	FPSCounter FPScounter;
};

class Child
{
public:
	Child();

	int leftStatistics[2], rightStatistics[2];
};

class Node
{
public:
	Node(int childsNum);
	void removeChilds();

	int statistics[2];
	Node *left, *right;
	Child *childs;
	vector<int> usedFeatures;
};

class BinaryTree
{
public:
	BinaryTree(int featuresNum, int statisticsNum);
	void buildLeafsForCurrentNode(Node* node, Feature feature);
	float calcGiniCoefficient(Child child);
	bool isUsedFeature(int featureNum, vector<int> usedFeatures);
	void divideNode(Node* node);
	void buildLeafs(Node* node, Feature feature);
	void buildTree(Feature feature);

	Node *root;
	int _featuresNum, _statisticsNum;
};