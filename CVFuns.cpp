#include "lib.h"

CVFuns::CVFuns()
{
	needToInit = true;
	isTargetSelected = false;

	tree = new BinaryTree(FEATURES_NUM, STATISTICS_NUM, DEPTH_OF_TREE);
	
	for (int i = 0; i < 4; i++)
		imgToDisplay.push_back(Mat(CAP_FRAME_HEIGHT, CAP_FRAME_WIDTH, CV_8U));

	frameWith0 = Mat(CAP_FRAME_HEIGHT, CAP_FRAME_WIDTH, CV_8U, Scalar(0));
	frameWith255 = Mat(CAP_FRAME_HEIGHT, CAP_FRAME_WIDTH, CV_8U, Scalar(255));
	integralImg = Mat(CAP_FRAME_HEIGHT + 1, CAP_FRAME_WIDTH + 1, CV_32S);
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
		putText(imgToDisplay[i - 1], to_string(i), Point(10, 30), 1, 1.5, Scalar(255), 2);

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

void CVFuns::calcTargetsBinaryFrame(Mat currentFrame, float targetsFactor)
{
	Mat backgroundBoundMask = Mat(CAP_FRAME_HEIGHT, CAP_FRAME_WIDTH, CV_8U, Scalar(255));

	currentDeviationImg = abs(currentFrame - averageBackImg);

	frameStaticPartMask = targetsFactor * deviationImg - currentDeviationImg;
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

	targetsBinaryFrame.setTo(Scalar(255));
	frameWith0.copyTo(targetsBinaryFrame, frameStaticPartMask);
	
	targetsBinaryFrame.copyTo(imgToDisplay[3]);
	imgToDisplayInfo[3] = "Moving target";
}

void CVFuns::findConnectedPoints(int x, int y, vector<Point2i> &connectedPoints)
{
	connectedPoints.push_back(Point2i(x, y));
	targetsBinaryFrame.at<uchar>(y, x) = 0;
	
	if (x + 1 < CAP_FRAME_WIDTH)
	{
		if (targetsBinaryFrame.at<uchar>(y, x + 1) == 255)
			findConnectedPoints(x + 1, y, connectedPoints);
	}
	
	if (x - 1 >= 0)
	{
		if (targetsBinaryFrame.at<uchar>(y, x - 1) == 255)
			findConnectedPoints(x - 1, y, connectedPoints);
	}

	if (y + 1 < CAP_FRAME_HEIGHT)
	{
		if (targetsBinaryFrame.at<uchar>(y + 1, x) == 255)
			findConnectedPoints(x, y + 1, connectedPoints);
	}
	
	if (y - 1 >= 0)
	{
		if (targetsBinaryFrame.at<uchar>(y - 1, x) == 255)
			findConnectedPoints(x, y - 1, connectedPoints);
	}
}

bool isInside(Object baseTarget, Object insideTarget)
{
	if (insideTarget.left >= baseTarget.left && insideTarget.top >= baseTarget.top
		&& insideTarget.right <= baseTarget.right && insideTarget.bottom <= baseTarget.bottom)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool isIntersect(Object baseTarget, Object intersectingTarget)
{
	if (intersectingTarget.right >= baseTarget.left && intersectingTarget.left <= baseTarget.right
		&& intersectingTarget.bottom >= baseTarget.top && intersectingTarget.top <= baseTarget.bottom)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool isNear(Object baseTarget, Object nearTarget, float distanceBetweenTargets)
{
	float currentDistance = (baseTarget.center.x - nearTarget.center.x) * (baseTarget.center.x - nearTarget.center.x) +
							(baseTarget.center.y - nearTarget.center.y) * (baseTarget.center.y - nearTarget.center.y);
	
	if (sqrt(currentDistance) <= distanceBetweenTargets)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CVFuns::makeSegmentation(float distanceBetweenTargets)
{
	vector<Point2i> connectedPoints;
	vector<vector<Point2i>> connectedPointsRegions;
	
	for (int x = 0; x < CAP_FRAME_WIDTH; x++)
	{
		for (int y = 0; y < CAP_FRAME_HEIGHT; y++)
		{
			if (targetsBinaryFrame.at<uchar>(y, x) == 255)
			{
				findConnectedPoints(x, y, connectedPoints);
				connectedPointsRegions.push_back(connectedPoints);
				connectedPoints.clear();
			}
		}
	}		

	objects.clear();
	int left, right, top, bottom;
	for (int i = 0; i < connectedPointsRegions.size(); i++)
	{
		left = CAP_FRAME_WIDTH;
		top = CAP_FRAME_HEIGHT;
		right = 0;
		bottom = 0;

		for (int j = 0; j < connectedPointsRegions[i].size(); j++)
		{
			if (connectedPointsRegions[i][j].x < left)
				left = connectedPointsRegions[i][j].x;

			if (connectedPointsRegions[i][j].y < top)
				top = connectedPointsRegions[i][j].y;

			if (connectedPointsRegions[i][j].x > right)
				right = connectedPointsRegions[i][j].x;

			if (connectedPointsRegions[i][j].y > bottom)
				bottom = connectedPointsRegions[i][j].y;
		}

		Point2f center;
		center.x = (float)(left + right) / 2;
		center.y = (float)(top + bottom) / 2;

		Object target = { left, right, top, bottom, center, true };
		objects.push_back(target);
	}

	for (int i = 0; i < objects.size(); i++)
	{
		for (int j = 0; j < objects.size(); j++)
		{
			if (i != j && objects[i].exist && objects[j].exist)
			{
				if (isInside(objects[i], objects[j]) || isIntersect(objects[i], objects[j]) || isNear(objects[i], objects[j], distanceBetweenTargets))
				{
					if (objects[i].left > objects[j].left)
						objects[i].left = objects[j].left;

					if (objects[i].right < objects[j].right)
						objects[i].right = objects[j].right;

					if (objects[i].top > objects[j].top)
						objects[i].top = objects[j].top;

					if (objects[i].bottom < objects[j].bottom)
						objects[i].bottom = objects[j].bottom;

					objects[j].exist = false;
				}
			}
		}
	}

	vector<Object> targetsTmp;
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i].exist)
		{
			targetsTmp.push_back(objects[i]);
		}
	}

	objects = targetsTmp;
	targetsTmp.clear();
	/*
	for (int i = 0; i < objects.size(); i++)
	{
		rectangle(imgToDisplay[0], Point2i(objects[i].left, objects[i].top), Point2i(objects[i].right, objects[i].bottom), Scalar(255), 2);
	}
	*/
}

void CVFuns::selectTarget(Point2i clickedPoint)
{
	for (int i = 0; i < objects.size(); i++)
	{
		if (clickedPoint.x >= objects[i].left && clickedPoint.x <= objects[i].right &&
			clickedPoint.y >= objects[i].top && clickedPoint.y <= objects[i].bottom)
		{
			selectedTarget = objects[i];
			isTargetSelected = true;
			break;
		}
	}

	if (isTargetSelected)
	{
		cout << "÷ель выбрана, началось обучение классификатора" << endl;
	}
}

void CVFuns::findSelectedTarget(float distanceBetweenTargetsOnTwoFrames)
{
	for (int i = 0; i < objects.size(); i++)
	{
		if (isInside(objects[i], selectedTarget) || isIntersect(objects[i], selectedTarget) || isNear(objects[i], selectedTarget, distanceBetweenTargetsOnTwoFrames))
		{
			selectedTarget = objects[i];
			break;
		}
	}
}

void CVFuns::displaySelectedTarget()
{
	rectangle(imgToDisplay[0], Point2i(selectedTarget.left, selectedTarget.top), Point2i(selectedTarget.right, selectedTarget.bottom), Scalar(0), 2);
}

void CVFuns::makeIntegralImg(Mat currentFrame)
{
	integral(currentFrame, integralImg);
}

int CVFuns::calcIntegralSumForRectangle(Object rectangle)
{
	return integralImg.at<int>(rectangle.bottom + 1, rectangle.right + 1) - 
			integralImg.at<int>(rectangle.top, rectangle.right + 1) - 
			integralImg.at<int>(rectangle.bottom + 1, rectangle.left) + 
			integralImg.at<int>(rectangle.top, rectangle.left);
}

bool CVFuns::haarFeature1(Object rectangle)
{
	Object halfA = rectangle;
	Object halfB = rectangle;
	
	int halfHeight = (rectangle.bottom - rectangle.top) / 2;
	
	halfA.bottom = rectangle.top + halfHeight;
	halfB.top = rectangle.top + halfHeight + 1;
	
	return calcIntegralSumForRectangle(halfA) > calcIntegralSumForRectangle(halfB);
}

bool CVFuns::haarFeature2(Object rectangle)
{
	Object halfA = rectangle;
	Object halfB = rectangle;

	int halfWidth = (rectangle.right - rectangle.left) / 2;

	halfA.right = rectangle.left + halfWidth;
	halfB.left = rectangle.left + halfWidth + 1;

	return calcIntegralSumForRectangle(halfA) > calcIntegralSumForRectangle(halfB);
}

Object CVFuns::rescaleFeaturePosition(Object featurePosition, Object featuresWindow)
{
	Object newFeaturePosition;
	
	int featuresWindowWidth = featuresWindow.right - featuresWindow.left;
	int featuresWindowHeight = featuresWindow.bottom - featuresWindow.top;

	newFeaturePosition.left = featuresWindow.left + int(featurePosition.left / 100.0f * featuresWindowWidth);
	newFeaturePosition.right = featuresWindow.left + int(featurePosition.right / 100.0f * featuresWindowWidth);
	newFeaturePosition.top = featuresWindow.top + int(featurePosition.top / 100.0f * featuresWindowHeight);
	newFeaturePosition.bottom = featuresWindow.top + int(featurePosition.bottom / 100.0f * featuresWindowHeight);

	return newFeaturePosition;
}

void CVFuns::makeFeaturesForWindow(Object featuresWindow, int isTarget)
{
	Features features(tree->_featuresNum);
	features.isTarget = isTarget;

	for (int i = 0; i < tree->_featuresNum; i++)
	{
		features.values[i] = haarFeature1(rescaleFeaturePosition(tree->featuresPositions[i], featuresWindow));  // —делать признак прив€занным к tree
	}

	tree->buildTree(features);
}

void CVFuns::calcFeatures()
{
	int maxWindowWidth = 200;
	int minWindowWidth = 10;
	int maxWindowHeight = 300;
	int minWindowHeight = 10;
	int featuresWindowsNum = 10;

	Object featuresWindow;

	for (int i = 0; i < featuresWindowsNum; i++)
	{
		do
		{
			featuresWindow.top = rand() % (CAP_FRAME_HEIGHT - minWindowHeight);
			featuresWindow.left = rand() % (CAP_FRAME_WIDTH - minWindowWidth);

			featuresWindow.bottom = featuresWindow.top + minWindowHeight + rand() % (maxWindowHeight - minWindowHeight);
			if (featuresWindow.bottom >= CAP_FRAME_HEIGHT)
				featuresWindow.bottom = CAP_FRAME_HEIGHT - 1;

			featuresWindow.right = featuresWindow.left + minWindowWidth + rand() % (maxWindowWidth - minWindowWidth);
			if (featuresWindow.right >= CAP_FRAME_WIDTH)
				featuresWindow.right = CAP_FRAME_WIDTH - 1; 
		} while (isInside(selectedTarget, featuresWindow) || isIntersect(selectedTarget, featuresWindow));

		//rectangle(imgToDisplay[0], Point2i(featuresWindow.left, featuresWindow.top), Point2i(featuresWindow.right, featuresWindow.bottom), Scalar(255), 2);
		makeFeaturesForWindow(featuresWindow, 0);
	}

	makeFeaturesForWindow(selectedTarget, 1);
}