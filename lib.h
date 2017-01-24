#pragma once

#include "opencv2/opencv.hpp"
#include <time.h> 

using namespace cv;
using namespace std;

const int CAP_FRAME_WIDTH = 641;
const int CAP_FRAME_HEIGHT = 480;
const int CAP_FPS = 20;

const int MAX_CORNERS_NUM = 128;
const int MIN_CORNERS_NUM = 32;

class CVFuns
{
public:
	CVFuns();
	bool startCapture();
	vector<Point2f> findCorners(Mat, int);
	void calcOpticalFlow(Mat, Mat, vector<Point2f>, vector<Point2f>&, vector<uchar>&);
	Mat translateFrame(Mat, Point2i);
	float findMedian(vector<float>);
	Point2f findOffsetMedian(vector<Point2f>, vector<Point2f>);
	void makeInitialFrame(Mat, vector<Point2f>&);
	Mat stabilizeFrame(Mat&);
	//Mat CalcAverageBackground(Mat currentFrame, );

	VideoCapture cap;
	bool needToInit;

private:
	Mat prevGrayFrame;
	vector<Point2f> prevPoints, currentPoints;
	Point2f offset;
};

class FPSCounter
{
public:
	FPSCounter();
	size_t value();
private:
	clock_t oldTime;
	size_t FPSCnt, oldFPS;
};

