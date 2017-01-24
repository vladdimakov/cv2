#include "lib.h"

CVFuns::CVFuns()
{
	needToInit = true;
}

bool CVFuns::startCapture()
{
	cap.open(0); // ќткрытие камеры (0 - камера по умолчанию)

	if (cap.isOpened()) // ”спешно ли открыта камера 
		return true;
	else
		return false;

	cap.set(CV_CAP_PROP_FRAME_WIDTH, CAP_FRAME_WIDTH); // ”становка параметров захватываемой картинки
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, CAP_FRAME_HEIGHT);
	cap.set(CV_CAP_PROP_FPS, CAP_FPS);
}

vector<Point2f> CVFuns::findCorners(Mat grayFrame, int maxCornersNum)
{
	// Shi-Tomasi Corner Detector
	double qualityLevel = 0.01; // ћера "качества" особых точек
	double minDistance = 10; // ћинимальное рассто€ние между особыми точками (в евклидовой мере)
	int blockSize = 3; // –азмер блока дл€ вычислени€ производной ковариационной матрицы в окрестности каждого пиксел€
	bool useHarrisDetector = false; // ѕараметр, указывающий, следует ли использовать детектор ’арриса
	double k = 0.04; // Ёмпирическа€ константа дл€ подсчета собственных значений ([0.04; 0.06]) (дл€ детектора ’арриса)

	vector<Point2f> corners;
	goodFeaturesToTrack(grayFrame, corners, maxCornersNum, qualityLevel, minDistance, Mat(), blockSize, useHarrisDetector, k);

	return corners;
}

void CVFuns::calcOpticalFlow(Mat prevGrayFrame, Mat currentGrayFrame, vector<Point2f> prevPoints, vector<Point2f>& currentPoints, vector<uchar>& status)
{
	vector<float> err; // ¬ектор погрешностей. “ип меры погрешности может быть установлен соответсвующим флагом
	Size winSize(21, 21); // –азмер окна при поиске
	int maxLevel = 3; // ћаксимальное число уровней пирамид
	TermCriteria criteria(TermCriteria::COUNT | TermCriteria::EPS, 30, 0.01); // ѕараметр, указывающий критерии завершени€ алгоритма итеративного поиска сдвига
	int flags = 0; // ‘лаги
	double minEigThreshold = 0.0001; // ѕороговое значение градиента, ниже которого матрица считаетс€ вырожденной

	calcOpticalFlowPyrLK(prevGrayFrame, currentGrayFrame, prevPoints, currentPoints, status, err, winSize, maxLevel, criteria, flags, minEigThreshold);
}

Mat CVFuns::translateFrame(Mat frame, Point2i offset)
{
	Mat newFrame = Mat(frame.rows, frame.cols, CV_8U);

	for (int y = 0; y < frame.rows; y++)
	{
		for (int x = 0; x < frame.cols; x++)
		{
			if (y - offset.y < frame.rows && y - offset.y >= 0 && x - offset.x < frame.cols && x - offset.x >= 0)
				newFrame.at<uchar>(y, x) = frame.at<uchar>(y - offset.y, x - offset.x);
			else
				newFrame.at<uchar>(y, x) = 0;
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

	prevPoints = findCorners(prevGrayFrame, MAX_CORNERS_NUM);

	for (int i = 0; i < prevPoints.size(); i++)
		circle(prevGrayFrame, prevPoints[i], 3, Scalar(255), -1, 8);

	imshow("Initial frame", prevGrayFrame);
}

Mat CVFuns::stabilizeFrame(Mat& currentGrayFrame)
{
	vector<uchar> status;
	Mat stabilizedGrayFrame;

	if (needToInit)
	{
		currentGrayFrame.copyTo(prevGrayFrame);
		makeInitialFrame(prevGrayFrame, prevPoints);
		needToInit = false;
		return currentGrayFrame;
	}

	currentGrayFrame.copyTo(stabilizedGrayFrame);

	calcOpticalFlow(prevGrayFrame, currentGrayFrame, prevPoints, currentPoints, status);

	size_t k = 0;
	for (size_t i = 0; i < currentPoints.size(); i++)
	{
		if (status[i])
		{
			currentPoints[k] = currentPoints[i];
			prevPoints[k] = prevPoints[i];
			k++;

			circle(currentGrayFrame, currentPoints[i], 3, Scalar(255), -1, 8);
		}
	}
	currentPoints.resize(k);
	prevPoints.resize(k);

	if (currentPoints.size() < MIN_CORNERS_NUM)
		needToInit = true;

	imshow("Current frame", currentGrayFrame);

	offset += findOffsetMedian(prevPoints, currentPoints);

	Point2i offset2i;
	offset2i.x = offset.x;
	offset2i.y = offset.y;

	stabilizedGrayFrame = translateFrame(stabilizedGrayFrame, offset2i);

	std::swap(prevPoints, currentPoints);
	cv::swap(prevGrayFrame, currentGrayFrame);

	return stabilizedGrayFrame;
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
