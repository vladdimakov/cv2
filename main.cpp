#include "CVFuns.h"
#include "suppFuns.h"
#include <vector>

const int CAP_FRAME_WIDTH = 641;
const int CAP_FRAME_HEIGHT = 480;
const int CAP_FPS = 20;

const int MAX_CORNERS_NUM = 128;
const int MIN_CORNERS_NUM = 32;

void MakeInitialFrame(VideoCapture cap, Mat& prevGrayFrame, vector<Point2f>& prevPoints)
{
	Mat colorFrame;

	cap >> colorFrame;
	cvtColor(colorFrame, prevGrayFrame, CV_RGB2GRAY);

	prevPoints = FindCorners(prevGrayFrame, MAX_CORNERS_NUM);

	for (int i = 0; i < prevPoints.size(); i++)
		circle(prevGrayFrame, prevPoints[i], 3, Scalar(255), -1, 8);

	imshow("Initial frame", prevGrayFrame);
}

int main(int argc, char** argv)
{
	setlocale(LC_ALL, "Russian");

	VideoCapture cap(0); // Открытие камеры (0 - камера по умолчанию)

	if (cap.isOpened()) // Успешно ли открыта камера 
		cout << "Камера открыта" << endl;
	else
		return -1;

	cap.set(CV_CAP_PROP_FRAME_WIDTH, CAP_FRAME_WIDTH); // Установка параметров захватываемой картинки
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, CAP_FRAME_HEIGHT);
	cap.set(CV_CAP_PROP_FPS, CAP_FPS);
		
	Mat colorFrame, prevGrayFrame, currentGrayFrame, stabilizedGrayFrame;
	vector<Point2f> prevPoints, currentPoints;
	Point2f offset = Point2f(0, 0);
	vector<uchar> status;
	
	MakeInitialFrame(cap, prevGrayFrame, prevPoints);
	
	while (true)
	{
		cap >> colorFrame;
		cvtColor(colorFrame, currentGrayFrame, CV_RGB2GRAY);
		currentGrayFrame.copyTo(stabilizedGrayFrame);		
		
        CalcOpticalFlow(prevGrayFrame, currentGrayFrame, prevPoints, currentPoints, status);

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
		{
			MakeInitialFrame(cap, prevGrayFrame, prevPoints);
			offset = Point2f(0, 0);
			continue;
		}

		imshow("Current frame", currentGrayFrame);
		
		offset += FindOffsetMedian(prevPoints, currentPoints);

        Point2i offset2i;
        offset2i.x = offset.x;
        offset2i.y = offset.y;

        stabilizedGrayFrame = TranslateFrame(stabilizedGrayFrame, offset2i);
		imshow("Stabilized frame", stabilizedGrayFrame);
		           
		char key = (char)waitKey(1); // waitKey ждет события нажатия клавиши 1 мс
		if (key == 27) // Если нажат ESC - выходим
		{
			break;
		}
		else if (key == 'c' || key == 'с')
		{
			MakeInitialFrame(cap, prevGrayFrame, prevPoints);
			offset = Point2f(0, 0);
			continue;
		}

		std::swap(prevPoints, currentPoints);
		cv::swap(prevGrayFrame, currentGrayFrame);
	}

	return 0;
}


/*
const int CAP_FRAME_WIDTH = 640;
const int CAP_FRAME_HEIGHT = 480;
const int CAP_FPS = 20;

int main(int argc, char** argv)
{
	setlocale(LC_ALL, "Russian");

	VideoCapture cap(0); // Открытие камеры (0 - камера по умолчанию)

	if (cap.isOpened()) // Успешно ли открыта камера 
		cout << "Камера открыта" << endl;
	else
		return -1;

	cap.set(CV_CAP_PROP_FRAME_WIDTH, CAP_FRAME_WIDTH); // Установка разрешения захватываемой картинки
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, CAP_FRAME_HEIGHT);
	cap.set(CV_CAP_PROP_FPS, CAP_FPS);

	Mat frame, grayFrame; // Кадр
	FPSCounter FPScnt;
	vector<Point2f> cornersPosition;

	while (true)
	{
		cap >> frame;
		cvtColor(frame, grayFrame, CV_RGB2GRAY); // RGB -> GRAY (Y = 0.299R + 0.587G + 0.114B)

		//calcOpticalFlowPyrLK(firstFrame, secondFrame,)

		//cornersPosition = DoCornerHarris(frame);
		cornersPosition = DoShiTomasi(grayFrame);

		for (int i = 0; i < cornersPosition.size(); i++)
		{
			circle(frame, cornersPosition[i], 3, Scalar(100, 255, 100), -1, 8);
		}

		string FPS = "FPS: " + to_string(FPScnt.value());

		putText(frame, FPS, Point(10, 20), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(100, 255, 100), 2.0);
		imshow("Result", frame); // Вывод кадра на экран

		if (waitKey(1) == 27) // waitKey ждет события нажатия клавиши 1 мс
			break;            // Если нажат ESC - выходим
	}

	return 0;
}
*/
