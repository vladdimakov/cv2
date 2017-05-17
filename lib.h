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
	float value();
private:
	time_t oldTime;
	size_t FPSCnt;
	float oldFPS;
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
	Node(int level, int childsNum);
	void removeOldData();

	int statistics[2];
	int num, level, featureNumToDivide;
	Node *left, *right;
	Child *childs;
	int /**randomlySelectedFeatures,*/ *previousSelectedFeatures;
	vector<int> randomlySelectedFeatures;
};

class Tree
{
public:
	Tree(int depthOfTree, int featuresNum, int randomlySelectedFeaturesNum, int minStatisticsNumForDivide, float minGiniCoefficient);
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
	
	int correctlyClassifiedOOB, incorrectlyClassifiedOOB;
	float OOBE;
};

class Forest
{
public:
	Forest();
	void buildTree(int treeNum, Features* features);
	bool classifyFeaturesByTree(int treeNum, Features* features);
	void discardTree(int treeNum);

	Tree **trees;
	int treesNum;

	int correctlyClassifiedOOB, incorrectlyClassifiedOOB;
	float OOBE;
};

class HaarFeatures
{
public:
	HaarFeatures();
	void makeIntegralImg(Mat currentFrame);
	int calcIntegralSumForRegion(Object region);

	bool calcFeature0(Object region);
	bool calcFeature1(Object region);
	bool calcFeature2(Object region);
	bool calcFeature3(Object region);
	bool calcFeature4(Object region);
	bool calcFeature5(Object region);
	bool calcFeature6(Object region);
	bool calcFeature7(Object region);

	Object rescaleFeaturePosition(Object featurePosition, Object region);
	int calcFeatures(Object region, Object featurePosition, int featureType);

	Mat integralImg;
};
class Detector
{
public:
	Detector();
	void displayWindow();
	bool startCapture(string videoSource);
	vector<Point2f> findCorners(Mat grayFrame, int maxCornersNum);
	void calcOpticalFlow(Mat prevGrayFrame, Mat currentGrayFrame, vector<Point2f> prevPoints, vector<Point2f>& currentPoints, vector<uchar>& status);
	void translateFrame(Mat inputFrame, Mat& outputFrame, Point2f offset);
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
	int getBackgroundBoundOpenCV(Mat frame);
	void calcTargetsBinaryFrame(Mat currentFrame, float targetsFactor);
	void findConnectedPoints(int x, int y, vector<Point2i>& connectedPoints);
	void makeSegmentation(float distanceBetweenTargets);
	void selectTarget(Point2i clickedPoint);
	void trekSelectedTarget(float distanceBetweenTargetsOnTwoFrames, float scalingFactorBetweenTargetsOnTwoFrames);
	void displaySelectedTarget();
	
	Object makeBackgroundRegion();
	void trainTreeByRegion(int treeNum, Object region, int isTarget);
    void trainClassifier();
	void calcTreeOOBE(int treeNum, Object region, int isTarget);
	void calcForestOOBE(vector<int> notUsedTrees, Object region, int isTarget);

	void discardTreesRandomly();

	void detectSelectedTarget();
	int classifyRegion(Object region);
	bool classifyRegionByTree(int treeNum, Object region);

	void writeTargetPositions();
	void readTargetPositions();
	int calcMethodError();
	void makeStats();
	int methodErrorSum;

	VideoCapture cap;
	vector<Mat> imgToDisplay;
	string imgToDisplayInfo[4];
	Mat frameStaticPartMask, averageBackImg, deviationImg, targetsBinaryFrame;
	bool needToInit;
	float deviationImgFillValue;
	
	vector<Object> objects;
	bool isTargetSelected;
	Object selectedTarget;

	HaarFeatures haarFeatures;
	Forest forest;

	Mat prevGrayFrame, currentDeviationImg, frameWith0, frameWith255;
	vector<Point2f> prevPoints, currentPoints;
	FPSCounter FPScounter;

	int frameNum;
	ifstream targetPositionsFile;
	//ofstream targetPositionsFile;
	vector<Object> targetPositions;
	ofstream OOBEFile, methodErrorFile;
};