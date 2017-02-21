#include "lib.h"

CVFuns::CVFuns()
{
	needToInit = true;

	for (int i = 0; i < 4; i++)
		imgToDisplay.push_back(Mat(CAP_FRAME_HEIGHT, CAP_FRAME_WIDTH, CV_8U));

	frameWith0 = Mat(CAP_FRAME_HEIGHT, CAP_FRAME_WIDTH, CV_8U, Scalar(0));
	frameWith255 = Mat(CAP_FRAME_HEIGHT, CAP_FRAME_WIDTH, CV_8U, Scalar(255));
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
	//namedWindow("Display window", CV_WINDOW_AUTOSIZE);

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
	subPixOffset.x = offset.x - (float)intOffset.x;
	subPixOffset.y = offset.y - (float)intOffset.y;

	if (subPixOffset.x != 0.0f || subPixOffset.y != 0.0f)
	{
		if (subPixOffset.x > 0.5f)
		{
			subPixOffset.x -= 1.0f;
			intOffset.x += 1;
		}
		else if (subPixOffset.x < -0.5f)
		{
			subPixOffset.x += 1.0f;
			intOffset.x -= 1;
		}
		if (subPixOffset.y > 0.5f)
		{
			subPixOffset.y -= 1.0f;
			intOffset.y += 1;
		}
		else if (subPixOffset.y < -0.5f)
		{
			subPixOffset.y += 1.0f;
			intOffset.y -= 1;
		}
				
		inputFrame = subPixTranslateFrameOpenCV(inputFrame, subPixOffset);		
		//inputFrame = subPixTranslateFrame(inputFrame, subPixOffset);
	}

	if (intOffset.x != 0 || intOffset.y != 0)
	{
		int xOld[2] = { 0, CAP_FRAME_WIDTH };
		int yOld[2] = { 0, CAP_FRAME_HEIGHT };
		int xNew[2] = { 0, CAP_FRAME_WIDTH };
		int yNew[2] = { 0, CAP_FRAME_HEIGHT };

		if (intOffset.x > 0)
		{
			xOld[0] = intOffset.x;
			xNew[1] = CAP_FRAME_WIDTH - intOffset.x;
		}
		else if (intOffset.x < 0)
		{
			xOld[1] = CAP_FRAME_WIDTH + intOffset.x;
			xNew[0] = -intOffset.x;
		}

		if (intOffset.y > 0)
		{
			yOld[0] = intOffset.y;
			yNew[1] = CAP_FRAME_HEIGHT - intOffset.y;
		}
		else if (intOffset.y < 0)
		{
			yOld[1] = CAP_FRAME_HEIGHT + intOffset.y;
			yNew[0] = -intOffset.y;
		}

		inputFrame.rowRange(yOld[0], yOld[1]).colRange(xOld[0], xOld[1]).copyTo(outputFrame.rowRange(yNew[0], yNew[1]).colRange(xNew[0], xNew[1]));
	}
	else
	{
		inputFrame.copyTo(outputFrame);
	}
}

Mat CVFuns::subPixTranslateFrame(Mat inputFrame, Point2f subPixOffset)
{
	if (subPixOffset.x == 0.0f && subPixOffset.y == 0.0f)
		return inputFrame;

	Mat outputFrame, translatedFrame1, translatedFrame2, translatedFrame3;

	inputFrame.copyTo(translatedFrame1);
	inputFrame.copyTo(translatedFrame2);
	inputFrame.copyTo(translatedFrame3);

	if (subPixOffset.x >= 0.0f && subPixOffset.y >= 0.0f)
	{
		inputFrame.rowRange(0, CAP_FRAME_HEIGHT).colRange(1, CAP_FRAME_WIDTH).copyTo(translatedFrame1.rowRange(0, CAP_FRAME_HEIGHT).colRange(0, CAP_FRAME_WIDTH - 1)); // y, x + 1
		inputFrame.rowRange(1, CAP_FRAME_HEIGHT).colRange(0, CAP_FRAME_WIDTH).copyTo(translatedFrame2.rowRange(0, CAP_FRAME_HEIGHT - 1).colRange(0, CAP_FRAME_WIDTH)); // y + 1, x
		inputFrame.rowRange(1, CAP_FRAME_HEIGHT).colRange(1, CAP_FRAME_WIDTH).copyTo(translatedFrame3.rowRange(0, CAP_FRAME_HEIGHT - 1).colRange(0, CAP_FRAME_WIDTH - 1)); // y + 1, x + 1
	}
	else if (subPixOffset.x < 0.0f && subPixOffset.y > 0.0f)
	{
		inputFrame.rowRange(0, CAP_FRAME_HEIGHT).colRange(0, CAP_FRAME_WIDTH - 1).copyTo(translatedFrame1.rowRange(0, CAP_FRAME_HEIGHT).colRange(1, CAP_FRAME_WIDTH)); // y, x - 1
		inputFrame.rowRange(1, CAP_FRAME_HEIGHT).colRange(0, CAP_FRAME_WIDTH).copyTo(translatedFrame2.rowRange(0, CAP_FRAME_HEIGHT - 1).colRange(0, CAP_FRAME_WIDTH)); // y + 1, x
		inputFrame.rowRange(1, CAP_FRAME_HEIGHT).colRange(0, CAP_FRAME_WIDTH - 1).copyTo(translatedFrame3.rowRange(0, CAP_FRAME_HEIGHT - 1).colRange(1, CAP_FRAME_WIDTH)); // y + 1, x - 1
	}
	else if (subPixOffset.x > 0.0f && subPixOffset.y < 0.0f)
	{
		inputFrame.rowRange(0, CAP_FRAME_HEIGHT).colRange(1, CAP_FRAME_WIDTH).copyTo(translatedFrame1.rowRange(0, CAP_FRAME_HEIGHT).colRange(0, CAP_FRAME_WIDTH - 1)); // y, x + 1
		inputFrame.rowRange(0, CAP_FRAME_HEIGHT - 1).colRange(0, CAP_FRAME_WIDTH).copyTo(translatedFrame2.rowRange(1, CAP_FRAME_HEIGHT).colRange(0, CAP_FRAME_WIDTH)); // y - 1, x
		inputFrame.rowRange(0, CAP_FRAME_HEIGHT - 1).colRange(1, CAP_FRAME_WIDTH).copyTo(translatedFrame3.rowRange(1, CAP_FRAME_HEIGHT).colRange(0, CAP_FRAME_WIDTH - 1)); // y - 1, x + 1
	}
	else if (subPixOffset.x <= 0.0f && subPixOffset.y <= 0.0f)
	{
		inputFrame.rowRange(0, CAP_FRAME_HEIGHT).colRange(0, CAP_FRAME_WIDTH - 1).copyTo(translatedFrame1.rowRange(0, CAP_FRAME_HEIGHT).colRange(1, CAP_FRAME_WIDTH)); // y, x - 1
		inputFrame.rowRange(0, CAP_FRAME_HEIGHT - 1).colRange(0, CAP_FRAME_WIDTH).copyTo(translatedFrame2.rowRange(1, CAP_FRAME_HEIGHT).colRange(0, CAP_FRAME_WIDTH)); // y - 1, x
		inputFrame.rowRange(0, CAP_FRAME_HEIGHT - 1).colRange(0, CAP_FRAME_WIDTH - 1).copyTo(translatedFrame3.rowRange(1, CAP_FRAME_HEIGHT).colRange(1, CAP_FRAME_WIDTH)); // y - 1, x - 1
	}

	subPixOffset.x = abs(subPixOffset.x);
	subPixOffset.y = abs(subPixOffset.y);

	outputFrame = inputFrame * (1 - subPixOffset.x) * (1 - subPixOffset.y) + translatedFrame1 * subPixOffset.x * (1 - subPixOffset.y) +
		translatedFrame2 * (1 - subPixOffset.x) * subPixOffset.y + translatedFrame3 * subPixOffset.x * subPixOffset.y;

	return outputFrame;
}

Mat CVFuns::subPixTranslateFrameOpenCV(Mat inputFrame, Point2f subPixOffset)
{
	if (subPixOffset.x == 0.0f && subPixOffset.y == 0.0f)
		return inputFrame;

	Mat outputFrame;
	Point2f center;

	center.x = (float)CAP_FRAME_WIDTH / 2 - 0.5f + subPixOffset.x;
	center.y = (float)CAP_FRAME_HEIGHT / 2 - 0.5f + subPixOffset.y;

	getRectSubPix(inputFrame, Size(CAP_FRAME_WIDTH, CAP_FRAME_HEIGHT), center, outputFrame);

	return outputFrame;
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
	
	deviationImg = Mat(CAP_FRAME_HEIGHT, CAP_FRAME_WIDTH, CV_32F, Scalar(deviationImgFillValue));

	prevPoints = findCorners(prevGrayFrame, MAX_CORNERS_NUM);
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

//			circle(imgToDisplay[0], currentPoints[i], 3, Scalar(255), -1, 8);
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

	currentFrame.copyTo(translatedAverageBackImg);
	translateFrame(averageBackImg, translatedAverageBackImg, currentOffset);

	translatedDeviationImg = Mat(CAP_FRAME_HEIGHT, CAP_FRAME_WIDTH, CV_32F, Scalar(deviationImgFillValue));
	translateFrame(deviationImg, translatedDeviationImg, currentOffset);

	translatedAverageBackImg.copyTo(averageBackImg);
	translatedDeviationImg.copyTo(deviationImg);
}

void CVFuns::calcFrameStaticPartMask(Mat currentFrame, float deviationFactor)
{
	currentDeviationImg = abs(currentFrame - averageBackImg);

	frameStaticPartMask = deviationFactor * deviationImg - currentDeviationImg;
	frameStaticPartMask.convertTo(frameStaticPartMask, CV_8U);
}

void CVFuns::showFrameStaticPartMask()
{
	imgToDisplay[3].setTo(Scalar(0));
	frameWith255.copyTo(imgToDisplay[3], frameStaticPartMask);
	imgToDisplayInfo[3] = "Mask";
}

void CVFuns::calcAverageBackAndDeviationImg(Mat currentFrame, float refreshRate)
{
	Mat currentDeviationImgStaticPart, currentFrameStaticPart;
	
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

int CVFuns::getBackgroundBound(Mat frame)
{
	int histogram[256];

	for (int i = 0; i < 256; i++)
		histogram[i] = 0;

	for (int i = 0; i < CAP_FRAME_HEIGHT; i++)
	{
		for (int j = 0; j < CAP_FRAME_WIDTH; j++)
		{
			histogram[frame.at<uchar>(i, j)]++;
		}
	}

	int startInd = 1;
	if (histogram[0] <= histogram[1])
	{
		for (startInd = 1; startInd < 256; startInd++)
		{
			if (histogram[startInd + 1] < histogram[startInd])
			{
				break;
			}
		}
	}

	int endInd = 1;
	for (endInd = startInd; endInd < 256; endInd++)
	{
		if (histogram[endInd + 1] >= histogram[endInd])
		{
			break;
		}
	}

	return endInd;
}

int CVFuns::getBackgroundBoundOpenCV(Mat frame)
{
	Mat histogram;

	int histSize = 256;
	float range[] = { 0, 256};
	const float* histRange = { range };
	
	calcHist(&frame, 1, 0, Mat(), histogram, 1, &histSize, &histRange);
	
	int startInd = 1;
	if (histogram.at<float>(0) <= histogram.at<float>(1))
	{
		for (startInd = 1; startInd < 256; startInd++)
		{
			if (histogram.at<float>(startInd + 1) < histogram.at<float>(startInd))
			{
				break;
			}
		}
	}

	int endInd = 1;
	for (endInd = startInd; endInd < 256; endInd++)
	{
		if (histogram.at<float>(endInd + 1) >= histogram.at<float>(endInd))
		{
			break;
		}
	}

	return endInd;
}

void makeOkr(Mat &frame, int j, int i, vector<Point2i> &points)
{
	frame.at<uchar>(j, i) = 0;
	points.push_back(Point2i(j, i));

	if (j + 1 < CAP_FRAME_HEIGHT)
	{
		if (frame.at<uchar>(j+1,i) == 255)
			makeOkr(frame, j+1,i, points);
	}

	if (i+1 < CAP_FRAME_WIDTH)
	{
		if (frame.at<uchar>(j,i+1) == 255)
			makeOkr(frame, j,i+1, points);
	}

	if (j-1 >= 0)
	{
		if (frame.at<uchar>(j-1,i) == 255)
			makeOkr(frame, j-1,i, points);
	}
	
	if (i -1 >=0)
	{
		if (frame.at<uchar>(j, i-1) == 255)
			makeOkr(frame, j,i-1, points);
	}

}

vector<Point2i> some(Mat &frame, int j, int i)
{
	vector<Point2i> points;
	makeOkr(frame, j, i, points);

	return points;
}

void CVFuns::drawRect(vector<vector<Point2i>> points)
{
	vector<target> targets;
	for (int i = 0; i < points.size(); i++)
	{
		if (points[i].size() > 10)
		{
			int xMin = CAP_FRAME_WIDTH, yMin = CAP_FRAME_HEIGHT, xMax = 0, yMax = 0;
			for (int j = 0; j < points[i].size(); j++)
			{
				int xCurrent = (points[i])[j].y;
				int yCurrent = (points[i])[j].x;

				if (xCurrent < xMin)
					xMin = xCurrent;

				if (yCurrent < yMin)
					yMin = yCurrent;

				if (xCurrent > xMax)
					xMax = xCurrent;

				if (yCurrent > yMax)
					yMax = yCurrent;
			}
			Point2f center;
			center.x = xMax - (float)(xMax - xMin) / 2;
			center.y = yMax - (float)(yMax - yMin) / 2;
			target tmp = { Point2i(xMin, yMin), Point2i(xMax, yMax), center, true };
			targets.push_back(tmp);
			//rectangle(imgToDisplay[0], tmp.min, tmp.max, Scalar(255), 2);
			//rectangle(imgToDisplay[0], Point2i(xMin, yMin), Point2i(xMax, yMax), Scalar(255), 2);
			//	circle(imgToDisplay[0], tmp.center, 3, Scalar(255), -1, 8);
		}
	}

	
	float dist = 100;
	for (int i = 0; i < targets.size(); i++)
	{
		for (int j = 1; j < targets.size(); j++)
		{
			if (targets[i].flag != false && targets[j].flag != false && i != j)
			{
				if (sqrt((targets[i].center.x - targets[j].center.x) * (targets[i].center.x - targets[j].center.x) + (targets[i].center.y - targets[j].center.y) * (targets[i].center.y - targets[j].center.y)) < dist)
				{
					targets[j].flag = false;

					if (targets[i].min.x > targets[j].min.x)
						targets[i].min.x = targets[j].min.x;

					if (targets[i].max.x < targets[j].max.x)
						targets[i].max.x = targets[j].max.x;

					if (targets[i].min.y > targets[j].min.y)
						targets[i].min.y = targets[j].min.y;

					if (targets[i].max.y < targets[j].max.y)
						targets[i].max.y = targets[j].max.y;
				}
			}
		}
	}

	
	int cnt = 0;
	for (int i = 0; i < targets.size(); i++)
	{
		if (targets[i].flag != false)
		{
			rectangle(imgToDisplay[0], targets[i].min, targets[i].max, Scalar(255), 2);
			cnt++;
		}
	}

	cout << cnt << endl;
}

void CVFuns::displayMovingTarget(Mat currentFrame, float movingTargetFactor)
{
	Mat backgroundBoundMask = Mat(CAP_FRAME_HEIGHT, CAP_FRAME_WIDTH, CV_8U, Scalar(255));

	currentDeviationImg = abs(currentFrame - averageBackImg);

	frameStaticPartMask = movingTargetFactor * deviationImg - currentDeviationImg;
	frameStaticPartMask.convertTo(frameStaticPartMask, CV_8U);
	
	currentDeviationImg.convertTo(currentDeviationImg, CV_8U);
	int backgroundBound = getBackgroundBoundOpenCV(currentDeviationImg);
	//int backgroundBound = getBackgroundBound(currentDeviationImg);

	frameWith0.copyTo(backgroundBoundMask, currentDeviationImg - backgroundBound);
	frameWith255.copyTo(frameStaticPartMask, backgroundBoundMask);
	/*
	for (int i = 0; i < CAP_FRAME_HEIGHT; i++)
	{
		for (int j = 0; j < CAP_FRAME_WIDTH; j++)
		{
			if (currentDeviationImg.at<uchar>(i, j) < backgroundBound)
				frameStaticPartMask.at<uchar>(i, j) = 255;
		}
	}
	*/

	Mat targetFrame = Mat(CAP_FRAME_HEIGHT, CAP_FRAME_WIDTH, CV_8U, Scalar(255));
	frameWith0.copyTo(targetFrame, frameStaticPartMask);
	//targetFrame.copyTo(imgToDisplay[3]);

	imgToDisplayInfo[3] = "Moving target";
	/*
	int cnt1 = 0;
	for (int j = 0; j < CAP_FRAME_HEIGHT; j++)
	{
		for (int i = 0; i < CAP_FRAME_WIDTH; i++)
		{
			if (targetFrame.at<uchar>(j, i) == 255)
			{
				cnt1++;
			}
		}
	}
	*/
	targetFrame.copyTo(imgToDisplay[3]);
	vector<vector<Point2i>> targetPoints;
	for (int j = 0; j < CAP_FRAME_HEIGHT; j++)
	{
		for (int i = 0; i < CAP_FRAME_WIDTH; i++)
		{
			if (targetFrame.at<uchar>(j, i) == 255)
			{
				vector<Point2i> tmp = some(targetFrame, j, i); 
				targetPoints.push_back(tmp);
			}
		}
	}
	

	/*
	targetFrame.copyTo(imgToDisplay[3]);
	int cnt = 0;
	Mat fff = Mat(CAP_FRAME_HEIGHT, CAP_FRAME_WIDTH, CV_8U, Scalar(255));
	for (int i = 0; i < targetPoints.size(); i++)
	{
		for (int j = 0; j < targetPoints[i].size(); j++)
		{
			Point2i pt = (targetPoints[i])[j];
			fff.at<uchar>(pt.x, pt.y) = cnt;
			
		}
		cnt += 10;
	}
	fff.copyTo(imgToDisplay[0]);
	*/
	/*
	int cnt2 = 0;
	for (int i = 0; i < targetPoints.size(); i++)
	{
		cnt2 += targetPoints[i].size();
	}

	cout << cnt2 - cnt1 << endl;
	*/

	drawRect(targetPoints);

}