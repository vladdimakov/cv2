#include "lib.h"

Detector::Detector()
{
	framesNum = 0;
	classifierNotFoundTargetNum = 0;
	classifierNotFoundTargetTotalNum = 0;

	needToInit = true;
	isTargetSelected = false;

	for (int i = 0; i < 4; i++)
		imgToDisplay.push_back(Mat(CAP_FRAME_HEIGHT, CAP_FRAME_WIDTH, CV_8U));

	frameWith0 = Mat(CAP_FRAME_HEIGHT, CAP_FRAME_WIDTH, CV_8U, Scalar(0));
	frameWith255 = Mat(CAP_FRAME_HEIGHT, CAP_FRAME_WIDTH, CV_8U, Scalar(255));
}

void Detector::displayWindow()
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

bool Detector::startCapture(string videoSource)
{
	if (videoSource.size() == 0)
		cap.open(0); // 0 - камера по умолчанию
	else
		cap.open(videoSource);
	
	cap.set(CV_CAP_PROP_FRAME_WIDTH, CAP_FRAME_WIDTH); // Установка параметров захватываемой картинки
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, CAP_FRAME_HEIGHT);
	cap.set(CV_CAP_PROP_FPS, CAP_FPS);

	if (cap.isOpened()) // Успешно ли открыта камера 
	{
		cout << "\n|Видеозахват успешно начался|\n" << endl;
		return true;
	}
	else
	{
		cout << "|Не удалось начать видеозахват|\n" << endl;
		return false;
	}
}

vector<Point2f> Detector::findCorners(Mat grayFrame, int maxCornersNum)
{
	// Shi-Tomasi Corner Detector
	double qualityLevel = 0.01; // Мера "качества" особых точек
	double minDistance = 10; // Минимальное расстояние между особыми точками (в евклидовой мере)
	int blockSize = 3; // Размер блока для вычисления производной ковариационной матрицы в окрестности каждого пикселя
	bool useHarrisDetector = false; // Параметр, указывающий, следует ли использовать детектор Харриса
	double k = 0.04; // Эмпирическая константа для подсчета собственных значений ([0.04; 0.06]) (для детектора Харриса)

	vector<Point2f> corners;
	goodFeaturesToTrack(grayFrame, corners, maxCornersNum, qualityLevel, minDistance, Mat(), blockSize, useHarrisDetector, k);

	return corners;
}

void Detector::calcOpticalFlow(Mat prevGrayFrame, Mat currentGrayFrame, vector<Point2f> prevPoints, vector<Point2f>& currentPoints, vector<uchar>& status)
{
	vector<float> err; // Вектор погрешностей. Тип меры погрешности может быть установлен соответсвующим флагом
	Size winSize(21, 21); // Размер окна при поиске
	int maxLevel = 3; // Максимальное число уровней пирамид
	TermCriteria criteria(TermCriteria::COUNT | TermCriteria::EPS, 30, 0.01); // Параметр, указывающий критерии завершения алгоритма итеративного поиска сдвига
	int flags = 0; // Флаги
	double minEigThreshold = 0.0001; // Пороговое значение градиента, ниже которого матрица считается вырожденной

	calcOpticalFlowPyrLK(prevGrayFrame, currentGrayFrame, prevPoints, currentPoints, status, err, winSize, maxLevel, criteria, flags, minEigThreshold);
}

void Detector::translateFrame(Mat inputFrame, Mat& outputFrame, Point2f offset)
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

Mat Detector::subPixTranslateFrameOpenCV(Mat inputFrame, Point2f subPixOffset)
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

float Detector::findMedian(vector<float> value)
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

Point2f Detector::findOffsetMedian(vector<Point2f> prevPoints, vector<Point2f> currentPoints)
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

void Detector::makeInitialFrame(Mat prevGrayFrame, vector<Point2f>& prevPoints)
{
	prevGrayFrame.convertTo(averageBackImg, CV_32F);
	
	deviationImg = Mat(CAP_FRAME_HEIGHT, CAP_FRAME_WIDTH, CV_32F, Scalar(deviationImgFillValue));

	prevPoints = findCorners(prevGrayFrame, MAX_CORNERS_NUM);
}

Point2f Detector::calcFrameOffset(Mat& currentGrayFrame)
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

void Detector::translateAverageBackAndDeviationImg(Mat currentFrame, Point2f currentOffset)
{
	Mat translatedAverageBackImg, translatedDeviationImg;

	currentFrame.copyTo(translatedAverageBackImg);
	translateFrame(averageBackImg, translatedAverageBackImg, currentOffset);

	translatedDeviationImg = Mat(CAP_FRAME_HEIGHT, CAP_FRAME_WIDTH, CV_32F, Scalar(deviationImgFillValue));
	translateFrame(deviationImg, translatedDeviationImg, currentOffset);

	translatedAverageBackImg.copyTo(averageBackImg);
	translatedDeviationImg.copyTo(deviationImg);
}

void Detector::calcFrameStaticPartMask(Mat currentFrame, float deviationFactor)
{
	currentDeviationImg = abs(currentFrame - averageBackImg);

	frameStaticPartMask = deviationFactor * deviationImg - currentDeviationImg;
	frameStaticPartMask.convertTo(frameStaticPartMask, CV_8U);
}

void Detector::showFrameStaticPartMask()
{
	imgToDisplay[3].setTo(Scalar(0));
	frameWith255.copyTo(imgToDisplay[3], frameStaticPartMask);
	imgToDisplayInfo[3] = "Mask";
}

void Detector::calcAverageBackAndDeviationImg(Mat currentFrame, float refreshRate)
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

void Detector::brightestScaling(Mat frame, float scalingFactor)
{
	Mat scaledFrame;

	scaledFrame = frame * scalingFactor;

	scaledFrame.convertTo(imgToDisplay[1], CV_8U);
	imgToDisplayInfo[1] = "Deviation image";
}

int Detector::getBackgroundBoundOpenCV(Mat frame)
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

void Detector::calcTargetsBinaryFrame(Mat currentFrame, float targetsFactor)
{
	Mat backgroundBoundMask = Mat(CAP_FRAME_HEIGHT, CAP_FRAME_WIDTH, CV_8U, Scalar(255));

	currentDeviationImg = abs(currentFrame - averageBackImg);

	frameStaticPartMask = targetsFactor * deviationImg - currentDeviationImg;
	frameStaticPartMask.convertTo(frameStaticPartMask, CV_8U);
	
	currentDeviationImg.convertTo(currentDeviationImg, CV_8U);
	int backgroundBound = getBackgroundBoundOpenCV(currentDeviationImg);

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

void Detector::findConnectedPoints(int x, int y, vector<Point2i> &connectedPoints)
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

bool isSameSize(Object baseTarget, Object sameSizeTarget, float scalingFactor)
{
	float baseWidth = float(baseTarget.right - baseTarget.left);
	float baseHeight = float(baseTarget.bottom - baseTarget.top);
	float sameSizeWidth = float(sameSizeTarget.right - sameSizeTarget.left);
	float sameSizeHeight = float(sameSizeTarget.bottom - sameSizeTarget.top);

//	if (baseWidth < 10 || baseHeight < 10)
	//	return false;

	if (baseWidth <= sameSizeWidth * scalingFactor  && sameSizeWidth <= baseWidth * scalingFactor &&
		baseHeight <= sameSizeHeight * scalingFactor  && sameSizeHeight <= baseHeight * scalingFactor)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Detector::makeSegmentation(float distanceBetweenTargets)
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

void Detector::selectTarget(Point2i clickedPoint)
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
		cout << "|Цель выбрана, предварительное обучение классификатора началось|\n" << endl;
	}
}

void Detector::findSelectedTarget(float distanceBetweenTargetsOnTwoFrames, float scalingFactorBetweenTargetsOnTwoFrames)
{
	if (selectedTarget.exist)
	{
		for (int i = 0; i < objects.size(); i++)
		{
			if ((isInside(objects[i], selectedTarget) || isIntersect(objects[i], selectedTarget) || isNear(objects[i], selectedTarget, distanceBetweenTargetsOnTwoFrames))
				&& isSameSize(objects[i], selectedTarget, scalingFactorBetweenTargetsOnTwoFrames))
			{
				selectedTarget = objects[i];
				selectedTarget.exist = true;
				break;
			}
			else
			{
				selectedTarget.exist = false;
			}
		}
	}
}

void Detector::displaySelectedTarget()
{
	if (selectedTarget.exist)
	{
		rectangle(imgToDisplay[0], Point2i(selectedTarget.left, selectedTarget.top), Point2i(selectedTarget.right, selectedTarget.bottom), Scalar(255), 2);
	}
}

Object Detector::makeBackgroundRegion()
{
	int regionWidth = selectedTarget.right - selectedTarget.left;
	int regionHeight = selectedTarget.bottom - selectedTarget.top;

	Object backgroundRegion;

	do
	{
		backgroundRegion.top = rand() % (CAP_FRAME_HEIGHT - regionHeight);
		backgroundRegion.left = rand() % (CAP_FRAME_WIDTH - regionWidth);
		backgroundRegion.bottom = backgroundRegion.top + regionHeight;
		backgroundRegion.right = backgroundRegion.left + regionWidth;

	} while (isInside(selectedTarget, backgroundRegion) || isIntersect(selectedTarget, backgroundRegion));

	return backgroundRegion;
}

int poissonRand()
{
	random_device randomDevice;
	mt19937 engine(randomDevice());
	poisson_distribution<> poissonDistribution(1);

	return (int)poissonDistribution(engine);
}

void Detector::discardTreesRandomly()
{
	const int randomGain = 20000;

	for (int i = 0; i < forest.treesNum; i++)
	{
		if (!forest.trees[i]->isDiscarded)
		{
			if (rand() % (int)((1.01 - forest.trees[i]->OOBE) * randomGain) == 0)
			{
				forest.trees[i]->isDiscarded = true;
				forest.currentTreesNum--;
			}
		}
	}
}

void Detector::trainClassifier()
{
	discardTreesRandomly();

	const int backgroundRegionsNum = 5;
	Object backgroundRegion;

	int samplesNum;
	for (int i = 0; i < forest.treesNum; i++)
	{
		if (!forest.trees[i]->isDiscarded && selectedTarget.exist)
		{
			samplesNum = poissonRand();
			if (samplesNum > 0)
			{
				for (int j = 0; j < samplesNum; j++)
				{
					for (int k = 0; k < backgroundRegionsNum; k++)
					{
						backgroundRegion = makeBackgroundRegion();
						trainTreeByRegion(i, backgroundRegion, 0);
					}

					trainTreeByRegion(i, selectedTarget, 1);
				}
			}
			else
			{
				if (forest.trees[i]->nodesNum >= 3)
				{
					for (int k = 0; k < backgroundRegionsNum; k++)
					{
						backgroundRegion = makeBackgroundRegion();
						if (!classifyRegionByTree(i, backgroundRegion))
							forest.trees[i]->correctlyClassifiedOOB++;
						else
							forest.trees[i]->incorrectlyClassifiedOOB++;
					}

					if (classifyRegionByTree(i, selectedTarget))
						forest.trees[i]->correctlyClassifiedOOB++;
					else
						forest.trees[i]->incorrectlyClassifiedOOB++;

					forest.trees[i]->OOBE = (float)forest.trees[i]->incorrectlyClassifiedOOB / forest.trees[i]->correctlyClassifiedOOB;
				}
			}
		}
	}
}

void Detector::trainTreeByRegion(int treeNum, Object region, int isTarget)
{
	Features *features = new Features(forest.trees[treeNum]->featuresNum);
	features->isTarget = isTarget;

	for (int i = 0; i < forest.trees[treeNum]->featuresNum; i++)
	{
		features->values[i] = haarFeatures.calcFeatures(region, forest.trees[treeNum]->featuresPositions[i], forest.trees[treeNum]->featuresTypes[i]);
	}

	forest.buildTree(treeNum, features);
}

bool Detector::classifyRegionByTree(int treeNum, Object region)
{
	Features *features = new Features(forest.trees[treeNum]->featuresNum);

	for (int i = 0; i < forest.trees[treeNum]->featuresNum; i++)
	{
		features->values[i] = haarFeatures.calcFeatures(region, forest.trees[treeNum]->featuresPositions[i], forest.trees[treeNum]->featuresTypes[i]);
	}

	return forest.classifyFeaturesByTree(treeNum, features);
}

bool Detector::classifyRegion(Object region)
{
	int voteYesNum = 0;
	int voteNoNum = 0;

	for (int i = 0; i < forest.treesNum; i++)
	{
		if (!forest.trees[i]->isDiscarded)
		{
			if (classifyRegionByTree(i, region))
			{
				voteYesNum++;
			}
			else
			{
				voteNoNum++;
			}
		}
	}

	return voteYesNum > voteNoNum;
}

void Detector::classifyAndTrain(float distanceBetweenTargetsOnTwoFrames, float scalingFactorBetweenTargetsOnTwoFrames)
{
	selectedTarget.exist = false;

	for (int i = 0; i < objects.size(); i++)
	{
		if (classifyRegion(objects[i]))
		{
			selectedTarget = objects[i];
			selectedTarget.exist = true;

			classifierNotFoundTargetNum = 0;

			trainClassifier();
			return;
		}
	}
	
	if (!selectedTarget.exist && classifierNotFoundTargetNum < classifierNotFoundTargetMaxNum)
	{
		selectedTarget.exist = true;
		findSelectedTarget(distanceBetweenTargetsOnTwoFrames / 2, scalingFactorBetweenTargetsOnTwoFrames / 2);
		
		if (selectedTarget.exist)
		{
			classifierNotFoundTargetNum++;
			classifierNotFoundTargetTotalNum++;

			trainClassifier();
		}
	}
}

float Detector::calcForestOOBE()
{
	vector<float> treesOOBE;

	for (int i = 0; i < forest.treesNum; i++)
	{
		if (!forest.trees[i]->isDiscarded)
			treesOOBE.push_back(forest.trees[i]->OOBE);
	}

	return findMedian(treesOOBE);
}

void Detector::showStats()
{
	if (framesNum == 1)
	{
		cout << framesNum - 1 << " | " << "Число деревьев: " << forest.currentTreesNum << endl;
	}
	else if (framesNum % 25 == 0 && framesNum != 0)
	{
		cout << framesNum << " | " << "Число деревьев: " << forest.currentTreesNum << " | OOBE: " << calcForestOOBE();
		if (framesNum > preliminaryTrainingFramesNum)
		{
			cout << " | Ошибка классификатора: " << (float)classifierNotFoundTargetTotalNum / 25 << endl;
			classifierNotFoundTargetTotalNum = 0;
		}
		else
		{
			cout << endl;
		}
	}
}