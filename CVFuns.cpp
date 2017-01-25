#include "lib.h"

CVFuns::CVFuns()
{
	needToInit = true;

	for (int i = 0; i < 4; i++)
		imgToDisplay.push_back(Mat(CAP_FRAME_HEIGHT, CAP_FRAME_WIDTH, CV_8U));
}

void CVFuns::displayWindow()
{
	Mat imageToDisplay = Mat(CAP_FRAME_HEIGHT * 2, CAP_FRAME_WIDTH * 2, CV_8U);

	namedWindow("Display window", CV_WINDOW_NORMAL);
	resizeWindow("Display window", WINDOW_WIDTH, WINDOW_HEIGHT);

	for (int i = 0; i < CAP_FRAME_HEIGHT; i++)
	{
		for (int j = 0; j < CAP_FRAME_WIDTH; j++)
		{
			imageToDisplay.at<uchar>(i, j) = imgToDisplay[0].at<uchar>(i, j);
			imageToDisplay.at<uchar>(i, j + CAP_FRAME_WIDTH - 1) = imgToDisplay[1].at<uchar>(i, j);
			imageToDisplay.at<uchar>(i + CAP_FRAME_HEIGHT - 1, j) = imgToDisplay[2].at<uchar>(i, j);
			imageToDisplay.at<uchar>(i + CAP_FRAME_HEIGHT - 1, j + CAP_FRAME_WIDTH - 1) = imgToDisplay[3].at<uchar>(i, j);
		}			
	}

	imshow("Display window", imageToDisplay); 
}

bool CVFuns::startCapture()
{
	cap.open(0); // �������� ������ (0 - ������ �� ���������)

	if (cap.isOpened()) // ������� �� ������� ������ 
		return true;
	else
		return false;

	cap.set(CV_CAP_PROP_FRAME_WIDTH, CAP_FRAME_WIDTH); // ��������� ���������� ������������� ��������
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, CAP_FRAME_HEIGHT);
	cap.set(CV_CAP_PROP_FPS, CAP_FPS);
}

vector<Point2f> CVFuns::findCorners(Mat grayFrame, int maxCornersNum)
{
	// Shi-Tomasi Corner Detector
	double qualityLevel = 0.01; // ���� "��������" ������ �����
	double minDistance = 10; // ����������� ���������� ����� ������� ������� (� ���������� ����)
	int blockSize = 3; // ������ ����� ��� ���������� ����������� �������������� ������� � ����������� ������� �������
	bool useHarrisDetector = false; // ��������, �����������, ������� �� ������������ �������� �������
	double k = 0.04; // ������������ ��������� ��� �������� ����������� �������� ([0.04; 0.06]) (��� ��������� �������)

	vector<Point2f> corners;
	goodFeaturesToTrack(grayFrame, corners, maxCornersNum, qualityLevel, minDistance, Mat(), blockSize, useHarrisDetector, k);

	return corners;
}

void CVFuns::calcOpticalFlow(Mat prevGrayFrame, Mat currentGrayFrame, vector<Point2f> prevPoints, vector<Point2f>& currentPoints, vector<uchar>& status)
{
	vector<float> err; // ������ ������������. ��� ���� ����������� ����� ���� ���������� �������������� ������
	Size winSize(21, 21); // ������ ���� ��� ������
	int maxLevel = 3; // ������������ ����� ������� �������
	TermCriteria criteria(TermCriteria::COUNT | TermCriteria::EPS, 30, 0.01); // ��������, ����������� �������� ���������� ��������� ������������ ������ ������
	int flags = 0; // �����
	double minEigThreshold = 0.0001; // ��������� �������� ���������, ���� �������� ������� ��������� �����������

	calcOpticalFlowPyrLK(prevGrayFrame, currentGrayFrame, prevPoints, currentPoints, status, err, winSize, maxLevel, criteria, flags, minEigThreshold);
}

Mat CVFuns::translateFrame(Mat frame, Point2f offset)
{
	Mat newFrame = Mat(frame.rows, frame.cols, CV_8U);

	Point2i offset2i;
	offset2i.x = offset.x;
	offset2i.y = offset.y;

	for (int y = 0; y < frame.rows; y++)
	{
		for (int x = 0; x < frame.cols; x++)
		{
			if (y - offset2i.y < frame.rows && y - offset2i.y >= 0 && x - offset2i.x < frame.cols && x - offset2i.x >= 0)
				newFrame.at<uchar>(y, x) = frame.at<uchar>(y - offset2i.y, x - offset2i.x);
			else
				newFrame.at<uchar>(y, x) = 0;
		}
	}

	return newFrame;
}

Mat CVFuns::translateFrame(Mat inputFrame, Mat outputFrame, Point2f offset)
{
	Mat newFrame;// = Mat(inputFrame.rows, inputFrame.cols, CV_8U);

	outputFrame.copyTo(newFrame);
	
	Point2i offset2i;
	offset2i.x = offset.x;
	offset2i.y = offset.y;

	for (int y = 0; y < inputFrame.rows; y++)
	{
		for (int x = 0; x < inputFrame.cols; x++)
		{
			if (y - offset2i.y < inputFrame.rows && y - offset2i.y >= 0 && x - offset2i.x < inputFrame.cols && x - offset2i.x >= 0)
				newFrame.at<uchar>(y, x) = inputFrame.at<uchar>(y - offset2i.y, x - offset2i.x);
		}
	}

	return newFrame;
}

float CVFuns::findMedian(vector<float> value)
{
	bool exit = false;
	size_t size = value.size();

	while (!exit)
	{
		exit = true;
		for (size_t i = 0; i < size - 1; i++)
		{
			if (value[i] > value[i + 1])
			{
				std::swap(value[i], value[i + 1]);
				exit = false;
			}
		}
	}

	if (size % 2 == 1)
	{
		return value[int(size / 2)];
	}
	else
	{
		return (value[size / 2 - 1] + value[size / 2]) / 2;
	}
}

Point2f CVFuns::findOffsetMedian(vector<Point2f> prevPoints, vector<Point2f> currentPoints)
{
	vector<float> xOffset, yOffset;

	for (int i = 0; i < currentPoints.size(); i++)
	{
		xOffset.push_back(prevPoints[i].x - currentPoints[i].x);
		yOffset.push_back(prevPoints[i].y - currentPoints[i].y);
	}

	return Point2f(findMedian(xOffset), findMedian(yOffset));
}

void CVFuns::makeInitialFrame(Mat prevGrayFrame, vector<Point2f>& prevPoints)
{
	offset = Point2f(0, 0);

	prevGrayFrame.copyTo(averageBackImage);

	prevPoints = findCorners(prevGrayFrame, MAX_CORNERS_NUM);

	prevGrayFrame.copyTo(imgToDisplay[0]);

	for (int i = 0; i < prevPoints.size(); i++)
		circle(imgToDisplay[0], prevPoints[i], 3, Scalar(255), -1, 8);
}

Point2f CVFuns::calcFrameOffset(Mat& currentGrayFrame)
{
	vector<uchar> status;
	Point2f frameOffset;

	if (needToInit)
	{
		currentGrayFrame.copyTo(prevGrayFrame);
		makeInitialFrame(prevGrayFrame, prevPoints);
		needToInit = false;
		return Point2f(0, 0);
	}

	calcOpticalFlow(prevGrayFrame, currentGrayFrame, prevPoints, currentPoints, status);

	currentGrayFrame.copyTo(imgToDisplay[1]);

	size_t k = 0;
	for (size_t i = 0; i < currentPoints.size(); i++)
	{
		if (status[i])
		{
			currentPoints[k] = currentPoints[i];
			prevPoints[k] = prevPoints[i];
			k++;

			circle(imgToDisplay[1], currentPoints[i], 3, Scalar(255), -1, 8);
		}
	}
	currentPoints.resize(k);
	prevPoints.resize(k);

	if (currentPoints.size() < MIN_CORNERS_NUM)
		needToInit = true;

	frameOffset = findOffsetMedian(prevPoints, currentPoints);

	std::swap(prevPoints, currentPoints);
	cv::swap(prevGrayFrame, currentGrayFrame);

	return frameOffset;
}

/*
Mat RGB2Gray(Mat frame)
{
Mat grayFrame = Mat(FRAME_HEIGHT, FRAME_WIDTH, CV_8U);

uchar R, G, B;

for (int i = 0; i < FRAME_HEIGHT; i++)
for (int j = 0; j < FRAME_WIDTH; j++)
{
R = frame.at<cv::Vec3b>(i, j)[2];
G = frame.at<cv::Vec3b>(i, j)[1];
B = frame.at<cv::Vec3b>(i, j)[0];

grayFrame.at<uchar>(i, j) = 0.299*R + 0.587*G + 0.114*B;
}

return grayFrame;
}

*/