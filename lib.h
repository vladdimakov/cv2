#pragma once

#include "opencv2/opencv.hpp"
#include <time.h> 
#include <fstream>
#include <random>

using namespace cv;
using namespace std;

const int CAP_FRAME_WIDTH = 1280;
const int CAP_FRAME_HEIGHT = 960;
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

	Mat prevGrayFrame, currentDeviationImg, frameWith0, frameWith255;
	vector<Point2f> prevPoints, currentPoints;
	FPSCounter FPScounter;

	int frameNum;
	ifstream targetPositionsFile;
	//ofstream targetPositionsFile;
	vector<Object> targetPositions;
	ofstream OOBEFile, methodErrorFile;
};