#include "lib.h"

CVFuns::CVFuns()
{
	needToInit = true;

	for (int i = 0; i < 4; i++)
		imgToDisplay.push_back(Mat(CAP_FRAME_HEIGHT, CAP_FRAME_WIDTH, CV_8U));
}

void CVFuns::displayWindow()
{
	Mat imageToDisplay = Mat(CAP_FRAME_HEIGHT * 2 + 30, CAP_FRAME_WIDTH * 2, CV_8U);
	Mat infoImg = Mat(30, CAP_FRAME_WIDTH * 2, CV_8U);
	infoImg.setTo(Scalar(255));

	string FPSStr = to_string(FPScounter.value());
	if (FPSStr.length() < 2)
		FPSStr = "0" + FPSStr;

	string infoStr = "FPS: " + FPSStr + " | 1." + imgToDisplayInfo[0] + " | 2." + imgToDisplayInfo[1] + " | 3." + imgToDisplayInfo[2] + " | 4." + imgToDisplayInfo[3];
	putText(infoImg, infoStr, Point(10, 20), 1, 1.5, Scalar(0), 2);

	namedWindow("Display window", CV_WINDOW_NORMAL);
	resizeWindow("Display window", WINDOW_WIDTH, WINDOW_HEIGHT);

	for (int i = 1; i < 5; i++)
		putText(imgToDisplay[i-1], to_string(i), Point(10, 30), 1, 1.5, Scalar(255), 2);
	
	imgToDisplay[0].copyTo(imageToDisplay.rowRange(0, CAP_FRAME_HEIGHT).colRange(0, CAP_FRAME_WIDTH));
	imgToDisplay[1].copyTo(imageToDisplay.rowRange(0, CAP_FRAME_HEIGHT).colRange(CAP_FRAME_WIDTH, CAP_FRAME_WIDTH * 2));
	imgToDisplay[2].copyTo(imageToDisplay.rowRange(CAP_FRAME_HEIGHT, CAP_FRAME_HEIGHT * 2).colRange(0, CAP_FRAME_WIDTH));
	imgToDisplay[3].copyTo(imageToDisplay.rowRange(CAP_FRAME_HEIGHT, CAP_FRAME_HEIGHT * 2).colRange(CAP_FRAME_WIDTH, CAP_FRAME_WIDTH * 2));
	infoImg.copyTo(imageToDisplay.rowRange(CAP_FRAME_HEIGHT * 2, CAP_FRAME_HEIGHT * 2 + 30).colRange(0, CAP_FRAME_WIDTH * 2));

	imshow("Display window", imageToDisplay); 
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

void CVFuns::translateFrame(Mat inputFrame, Mat& outputFrame, Point2f offset)
{
	Point2i offset2i;
	offset2i.x = offset.x;
	offset2i.y = offset.y;

	if (offset2i.x == 0 && offset2i.y == 0)
		inputFrame.copyTo(outputFrame);
	
	int xOld[2] = { 0, CAP_FRAME_WIDTH };
	int yOld[2] = { 0, CAP_FRAME_HEIGHT };
	int xNew[2] = { 0, CAP_FRAME_WIDTH };
	int yNew[2] = { 0, CAP_FRAME_HEIGHT };
	
	if (offset2i.x < 0)
	{
		xOld[0] = -offset2i.x;
		xNew[1] = CAP_FRAME_WIDTH + offset2i.x;
	}
	if (offset2i.x > 0)
	{
		xOld[1] = CAP_FRAME_WIDTH - offset2i.x;
		xNew[0] = offset2i.x;
	}
	if (offset2i.y < 0)
	{
		yOld[0] = -offset2i.y;
		yNew[1] = CAP_FRAME_HEIGHT + offset2i.y;
	}
	if (offset2i.y > 0)
	{
		yOld[1] = CAP_FRAME_HEIGHT - offset2i.y;
		yNew[0] = offset2i.y;
	}
		
	inputFrame.rowRange(yOld[0], yOld[1]).colRange(xOld[0], xOld[1]).copyTo(outputFrame.rowRange(yNew[0], yNew[1]).colRange(xNew[0], xNew[1])); 
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

	prevGrayFrame.convertTo(averageBackImg, CV_32F);

	deviationImg = Mat::zeros(CAP_FRAME_HEIGHT, CAP_FRAME_WIDTH, CV_32F);

	prevPoints = findCorners(prevGrayFrame, MAX_CORNERS_NUM);
	/*
	prevGrayFrame.copyTo(imgToDisplay[0]);

	for (int i = 0; i < prevPoints.size(); i++)
		circle(imgToDisplay[0], prevPoints[i], 3, Scalar(255), -1, 8);

	imgToDisplayInfo[0] = "Initialization frame";
	*/
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

	currentGrayFrame.copyTo(imgToDisplay[0]);
	imgToDisplayInfo[0] = "Current frame";

	size_t k = 0;
	for (size_t i = 0; i < currentPoints.size(); i++)
	{
		if (status[i])
		{
			currentPoints[k] = currentPoints[i];
			prevPoints[k] = prevPoints[i];
			k++;

			circle(imgToDisplay[0], currentPoints[i], 3, Scalar(255), -1, 8);
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

void CVFuns::calcAverageBackImg(Mat currentFrame, Point2f currentOffset, float refreshRate)
{
	Mat newAverageBackImg, part2;
	currentFrame.convertTo(newAverageBackImg, CV_32F);
	
	part2 = refreshRate * newAverageBackImg;

	currentOffset.x = -currentOffset.x;
	currentOffset.y = -currentOffset.y;
	translateFrame(averageBackImg, newAverageBackImg, currentOffset);

	averageBackImg = (1 - refreshRate) * newAverageBackImg + part2;

	averageBackImg.convertTo(imgToDisplay[2], CV_8U);

	imgToDisplayInfo[2] = "Average backgroung";
}

void CVFuns::deviationFromAverageBackImg(Mat currentFrame, float refreshRate)
{
	Mat currentDeviationImg;
	currentFrame.convertTo(currentDeviationImg, CV_32F);

	currentDeviationImg = abs(currentDeviationImg - averageBackImg);
	
	deviationImg = (1 - refreshRate) * deviationImg + refreshRate * currentDeviationImg;	
}

void CVFuns::brightestScaling(Mat frame, float scalingFactor)
{
	Mat scaledFrame;

	scaledFrame = frame * scalingFactor;

	scaledFrame.convertTo(imgToDisplay[3], CV_8U);
	imgToDisplayInfo[3] = "Deviation image";
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
