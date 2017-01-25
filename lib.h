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

const int WINDOW_WIDTH = 854;
const int WINDOW_HEIGHT = 640;

class CVFuns
{
public:
	CVFuns();
	void displayWindow();
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
	vector<Mat> imgToDisplay;
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

