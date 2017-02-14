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

bool CVFuns::startCapture(int argc, char* argv[])
{
	if (argc == 1)
		cap.open(0); // 0 - камера по умолчанию
	else if (argc == 2)
		cap.open(argv[1]);
	else
		cout << "Ќеверное число аргументов командной строки" << endl;

	cap.set(CV_CAP_PROP_FRAME_WIDTH, CAP_FRAME_WIDTH); // ”становка параметров захватываемой картинки
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, CAP_FRAME_HEIGHT);
	cap.set(CV_CAP_PROP_FPS, CAP_FPS);

	if (cap.isOpened()) // ”спешно ли открыта камера 
	{
		cout << "¬идеозахват успешно началс€" << endl;
		return true;
	}
	else
	{
		cout << "Ќе удалось начать видеозахват" << endl;
		return false;
	}
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
	Point2i intOffset;
	intOffset.x = (int)offset.x;
	intOffset.y = (int)offset.y;

	Point2f subPixOffset;
	subPixOffset.x = offset.x - intOffset.x;
	subPixOffset.y = offset.y - intOffset.y;

	if (subPixOffset.x != 0.0f || subPixOffset.y != 0.0f)
	{
		if (subPixOffset.x > 0.5f)
		{
			subPixOffset.x -= 1.0f;
			intOffset.x += 1;
		}
		if (subPixOffset.y > 0.5f)
		{
			subPixOffset.y -= 1.0f;
			intOffset.y += 1;
		}

		Point2f center;
		center.x = (float)CAP_FRAME_WIDTH / 2 - 0.5f - subPixOffset.x;
		center.y = (float)CAP_FRAME_HEIGHT / 2 - 0.5f - subPixOffset.y;

		getRectSubPix(inputFrame, Size(CAP_FRAME_WIDTH, CAP_FRAME_HEIGHT), center, inputFrame);

		/*
		Mat newFrame = Mat(CAP_FRAME_HEIGHT, CAP_FRAME_WIDTH, CV_8U);
		for (int y = 0; y < CAP_FRAME_HEIGHT - 1; y++)
		{
			for (int x = 0; x < CAP_FRAME_WIDTH - 1; x++)
			{
				newFrame.at<uchar>(y, x) = inputFrame.at<uchar>(y, x) * (1 - subPixOffset.x) * (1 - subPixOffset.y) + 
					inputFrame.at<uchar>(y, x + 1) * subPixOffset.x * (1 - subPixOffset.y) +
					inputFrame.at<uchar>(y + 1, x) * (1 - subPixOffset.x) * subPixOffset.y + 
					inputFrame.at<uchar>(y + 1, x + 1) * subPixOffset.x * subPixOffset.y;
			}
		}
		newFrame.copyTo(inputFrame);
		*/
	}

	if (intOffset.x != 0 || intOffset.y != 0)
	{
		int xOld[2] = { 0, CAP_FRAME_WIDTH };
		int yOld[2] = { 0, CAP_FRAME_HEIGHT };
		int xNew[2] = { 0, CAP_FRAME_WIDTH };
		int yNew[2] = { 0, CAP_FRAME_HEIGHT };

		if (intOffset.x < 0)
		{
			xOld[0] = -intOffset.x;
			xNew[1] = CAP_FRAME_WIDTH + intOffset.x;
		}
		else if (intOffset.x > 0)
		{
			xOld[1] = CAP_FRAME_WIDTH - intOffset.x;
			xNew[0] = intOffset.x;
		}

		if (intOffset.y < 0)
		{
			yOld[0] = -intOffset.y;
			yNew[1] = CAP_FRAME_HEIGHT + intOffset.y;
		}
		else if (intOffset.y > 0)
		{
			yOld[1] = CAP_FRAME_HEIGHT - intOffset.y;
			yNew[0] = intOffset.y;
		}

		inputFrame.rowRange(yOld[0], yOld[1]).colRange(xOld[0], xOld[1]).copyTo(outputFrame.rowRange(yNew[0], yNew[1]).colRange(xNew[0], xNew[1]));
	}
	else
	{
		inputFrame.copyTo(outputFrame);
	}
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
	if (currentPoints.size() != 0)
	{
		vector<float> xOffset, yOffset;

		for (int i = 0; i < currentPoints.size(); i++)
		{
			xOffset.push_back(prevPoints[i].x - currentPoints[i].x);
			yOffset.push_back(prevPoints[i].y - currentPoints[i].y);
		}

		return Point2f(findMedian(xOffset), findMedian(yOffset));
	}
	else
	{
		return Point2f(0, 0);		
	}	
}

void CVFuns::makeInitialFrame(Mat prevGrayFrame, vector<Point2f>& prevPoints)
{
	offset = Point2f(0, 0);

	prevGrayFrame.convertTo(averageBackImg, CV_32F);
	
	//prevGrayFrame.convertTo(deviationImg, CV_32F);
	deviationImg = Mat(CAP_FRAME_HEIGHT, CAP_FRAME_WIDTH, CV_32F, Scalar(255));

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

void CVFuns::translateAverageBackAndDeviationImg(Mat currentFrame, Point2f currentOffset)
{
	Mat translatedAverageBackImg, translatedDeviationImg;

	currentFrame.convertTo(currentFrame, CV_32F);

	currentFrame.copyTo(translatedAverageBackImg);
	translateFrame(averageBackImg, translatedAverageBackImg, currentOffset);

	//currentFrame.copyTo(translatedDeviationImg);
	translatedDeviationImg = Mat(CAP_FRAME_HEIGHT, CAP_FRAME_WIDTH, CV_32F, Scalar(255));
	translateFrame(deviationImg, translatedDeviationImg, currentOffset);

	translatedAverageBackImg.copyTo(averageBackImg);
	translatedDeviationImg.copyTo(deviationImg);
}

void CVFuns::calcFrameStaticPartMask(Mat currentFrame, float deviationFactor)
{
	currentFrame.convertTo(currentFrame, CV_32F);

	frameStaticPartMask = deviationFactor * deviationImg - abs(currentFrame - averageBackImg);
	frameStaticPartMask.convertTo(frameStaticPartMask, CV_8U);

	Mat matWith255 = Mat(CAP_FRAME_HEIGHT, CAP_FRAME_WIDTH, CV_8U, Scalar(255));
	imgToDisplay[3].setTo(Scalar(0));
	matWith255.copyTo(imgToDisplay[3], frameStaticPartMask);
	imgToDisplayInfo[3] = "Mask";
}

void CVFuns::calcAverageBackAndDeviationImg(Mat currentFrame, float refreshRate)
{
	Mat currentDeviationImg, currentDeviationImgStaticPart, currentFrameStaticPart;

	currentFrame.convertTo(currentFrame, CV_32F);

	currentDeviationImg = abs(currentFrame - averageBackImg);

	averageBackImg.copyTo(currentFrameStaticPart);
	currentFrame.copyTo(currentFrameStaticPart, frameStaticPartMask);

	averageBackImg = (1 - refreshRate) * averageBackImg + refreshRate * currentFrameStaticPart;

	deviationImg.copyTo(currentDeviationImgStaticPart);
	currentDeviationImg.copyTo(currentDeviationImgStaticPart, frameStaticPartMask);

	deviationImg = (1 - refreshRate) * deviationImg + refreshRate * currentDeviationImgStaticPart;

	averageBackImg.convertTo(imgToDisplay[2], CV_8U);
	imgToDisplayInfo[2] = "Average backgroung";
}

void CVFuns::brightestScaling(Mat frame, float scalingFactor)
{
	Mat scaledFrame;

	scaledFrame = frame * scalingFactor;

	scaledFrame.convertTo(imgToDisplay[1], CV_8U);
	imgToDisplayInfo[1] = "Deviation image";
}