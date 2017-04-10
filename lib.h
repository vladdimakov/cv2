#pragma once

#include "opencv2/opencv.hpp"
#include <time.h> 
#include <fstream>
#include <random>

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
	Node(int level, int childsNum, int randomlySelectedFeaturesNum);
	void removeOldData();

	int statistics[2];
	int num, level, featureNumToDivide;
	Node *left, *right;
	Child *childs;
	int *randomlySelectedFeatures, *previousSelectedFeatures;
};

class BinaryTree
{
public:
	BinaryTree(int depthOfTree, int featuresNum, int randomlySelectedFeaturesNum, int minStatisticsNumForDivide, float minGiniCoefficient);
	void buildNode(Node* node, Features* features);
	float calcGiniCoefficient(Child child);
	void makeRandomlySelectedFeatures(Node* parent, Node* child);
	void divideNode(Node* node);
	void buildTree(Node* node, Features* features);
	bool classifyFeatures(Node* node, Features* features);

	Node *root;
	Object *featuresPositions;
	int *featuresTypes;
	int nodesNum;
	int depthOfTree, featuresNum, randomlySelectedFeaturesNum, minStatisticsNumForDivide;
	float minGiniCoefficient;
	
	int correctly—lassifiedOOB, incorrectly—lassifiedOOB;
	float OOBE;
	bool isDiscarded;
};

class Forest
{
public:
	Forest();
	void buildTree(int treeNum, Features* features);
	bool classifyFeaturesByTree(int treeNum, Features* features);

	BinaryTree **trees;
	int treesNum, currentTreesNum;
};

class CVFuns
{
public:
	CVFuns();
	void displayWindow();
	bool startCapture(string videoSource);
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
	void findSelectedTarget(float distanceBetweenTargetsOnTwoFrames, float scalingFactorBetweenTargetsOnTwoFrames);
	void displaySelectedTarget();

	void makeIntegralImg(Mat currentFrame);
	int calcIntegralSumForRegion(Object region);

	bool calcHaarFeature0(Object region);
	bool calcHaarFeature1(Object region);
	bool calcHaarFeature2(Object region);
	bool calcHaarFeature3(Object region);
	bool calcHaarFeature4(Object region);
	bool calcHaarFeature5(Object region);
	bool calcHaarFeature6(Object region);
	bool calcHaarFeature7(Object region);
	int calcHaarFeatures(Object region, int featureType);
	
	Object rescaleFeaturePosition(Object featurePosition, Object region);
	Object makeBackgroundRegion();
	void trainTreeByRegion(int treeNum, Object region, int isTarget);
    void trainClassifier();

	void discardTreesRandomly();

	void classifyAndTrain(float distanceBetweenTargetsOnTwoFrames, float scalingFactorBetweenTargetsOnTwoFrames);
	bool classifyRegion(Object region);
	bool classifyRegionByTree(int treeNum, Object region);

	float calcForestOOBE();
	void showStats();

	VideoCapture cap;
	int framesNum, preliminaryTrainingFramesNum, classifierNotFoundTargetNum, classifierNotFoundTargetMaxNum, classifierNotFoundTargetTotalNum;
	vector<Mat> imgToDisplay;
	string imgToDisplayInfo[4];
	Point2f offset;
	Mat frameStaticPartMask, averageBackImg, deviationImg, targetsBinaryFrame, integralImg;
	bool needToInit;
	float deviationImgFillValue;
	
	vector<Object> objects;
	bool isTargetSelected;
	Object selectedTarget;

	Forest forest;

private:
	Mat prevGrayFrame, currentDeviationImg, frameWith0, frameWith255;
	vector<Point2f> prevPoints, currentPoints;
	FPSCounter FPScounter;
};
