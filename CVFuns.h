#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

vector<Point2f> DoCornerHarris(Mat frame)
{
	Mat corners = Mat::zeros(frame.size(), CV_32FC1); // Особые точки
	vector<Point2f> cornersPosition;

	// Параметры поиска
	float thresh = 170; // Пороговые значения для отсечения
	int blockSize = 2; // Размер окна
	int apertureSize = 3; // Параметр апертуры (размера расширенного ядра) оператора Собеля (1, 3, 5 или 7)
	// Оператор Собеля - дискретный дифференциальный оператор, вычисляющий приближённое значение 
	// градиента яркости изображения. Результатом применения оператора Собеля в каждой точке изображения является
	// либо вектор градиента яркости в этой точке, либо его норма.
	double k = 0.04; // Эмпирическая константа для подсчета собственных значений [0.04; 0.06]
	int maxCornersNum = 512; // Максимальное число особых точек на кадр
	double margin = 0.1; // Отступ от края кадра

	// Поиск особых точек
	cornerHarris(frame, corners, blockSize, apertureSize, k, BORDER_DEFAULT);

	// Нормализация
	normalize(corners, corners, 0, 255, NORM_MINMAX, CV_32FC1, Mat());

	// Окружности на месте особых точек
	int cornersNum = 0;
	for (int j = corners.rows*margin; j < corners.rows*(1 - margin); j++)
	{
		for (int i = corners.cols*margin; i < corners.cols*(1 - margin); i++)
		{
			if (corners.at<float>(j, i) > thresh && cornersNum < maxCornersNum
				&& corners.at<float>(j + 1, i) < thresh && corners.at<float>(j, i + 1) < thresh
				&& corners.at<float>(j - 1, i) < thresh && corners.at<float>(j, i - 1) < thresh)
			{
				cornersPosition.push_back(Point2f(i, j));
				cornersNum++;
			}
		}
	}

	return cornersPosition;
}

vector<Point2f> FindCorners(Mat grayFrame, int maxCornersNum)
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

void CalcOpticalFlow(Mat prevGrayFrame, Mat currentGrayFrame, vector<Point2f> prevPoints, vector<Point2f>& currentPoints, vector<uchar>& status)
{
	vector<float> err; // Вектор погрешностей. Тип меры погрешности может быть установлен соответсвующим флагом
	Size winSize(21, 21); // Размер окна при поиске
	int maxLevel = 3; // Максимальное число уровней пирамид
	TermCriteria criteria(TermCriteria::COUNT | TermCriteria::EPS, 30, 0.01); // Параметр, указывающий критерии завершения алгоритма итеративного поиска сдвига
	int flags = 0; // Флаги
	double minEigThreshold = 0.0001; // Пороговое значение градиента, ниже которого матрица считается вырожденной

	calcOpticalFlowPyrLK(prevGrayFrame, currentGrayFrame, prevPoints, currentPoints, status, err, winSize, maxLevel, criteria, flags, minEigThreshold);
}

Mat TranslateFrame(Mat frame, Point2i offset)
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

float FindMedian(vector<float> value)
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

Point2f FindOffsetMedian(vector<Point2f> prevPoints, vector<Point2f> currentPoints)
{
	vector<float> xOffset, yOffset;

	for (int i = 0; i < currentPoints.size(); i++)
	{
		xOffset.push_back(prevPoints[i].x - currentPoints[i].x);
		yOffset.push_back(prevPoints[i].y - currentPoints[i].y);
	}
	
	return Point2f(FindMedian(xOffset), FindMedian(yOffset));
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

/*
Point2f point;
bool addRemovePt = false;

static void onMouse(int event, int x, int y, int, void*)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		point = Point2f((float)x, (float)y);
		addRemovePt = true;
	}
}

int main(int argc, char** argv)
{
	VideoCapture cap;
	TermCriteria termcrit(TermCriteria::COUNT | TermCriteria::EPS, 20, 0.03);
	Size subPixWinSize(10, 10), winSize(31, 31);

	const int MAX_COUNT = 500;
	bool needToInit = false;
	bool nightMode = false;

	cv::CommandLineParser parser(argc, argv, "{@input||}{help h||}");
	string input = parser.get<string>("@input");
	//if (parser.has("help"))
	//{
	//	help();
	//	return 0;
	//}
	if (input.empty())
		cap.open(0);
	else if (input.size() == 1 && isdigit(input[0]))
		cap.open(input[0] - '0');
	else
		cap.open(input);

	if (!cap.isOpened())
	{
		cout << "Could not initialize capturing...\n";
		return 0;
	}

	namedWindow("LK Demo", 1);
	setMouseCallback("LK Demo", onMouse, 0);

	Mat gray, prevGray, image, frame;
	vector<Point2f> points[2];

	for (;;)
	{
		cap >> frame;
		if (frame.empty())
			break;

		frame.copyTo(image);
		cvtColor(image, gray, COLOR_BGR2GRAY);

		if (nightMode)
			image = Scalar::all(0);

		if (needToInit)
		{
			// automatic initialization
			goodFeaturesToTrack(gray, points[1], MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
			cornerSubPix(gray, points[1], subPixWinSize, Size(-1, -1), termcrit);
			addRemovePt = false;
		}
		else if (!points[0].empty())
		{
			vector<uchar> status;
			vector<float> err;
			if (prevGray.empty())
				gray.copyTo(prevGray);
			calcOpticalFlowPyrLK(prevGray, gray, points[0], points[1], status, err, winSize,
				3, termcrit, 0, 0.001);
			size_t i, k;
			for (i = k = 0; i < points[1].size(); i++)
			{
				if (addRemovePt)
				{
					if (norm(point - points[1][i]) <= 5)
					{
						addRemovePt = false;
						continue;
					}
				}

				if (!status[i])
					continue;

				points[1][k++] = points[1][i];
				circle(image, points[1][i], 3, Scalar(0, 255, 0), -1, 8);
			}
			points[1].resize(k);
		}

		if (addRemovePt && points[1].size() < (size_t)MAX_COUNT)
		{
			vector<Point2f> tmp;
			tmp.push_back(point);
			cornerSubPix(gray, tmp, winSize, Size(-1, -1), termcrit);
			points[1].push_back(tmp[0]);
			addRemovePt = false;
		}

		needToInit = false;
		imshow("LK Demo", image);

		char c = (char)waitKey(10);
		if (c == 27)
			break;
		switch (c)
		{
		case 'r':
			needToInit = true;
			break;
		case 'c':
			points[0].clear();
			points[1].clear();
			break;
		case 'n':
			nightMode = !nightMode;
			break;
		}

		std::swap(points[1], points[0]);
		cv::swap(prevGray, gray);
	}

	return 0;
}

*/
