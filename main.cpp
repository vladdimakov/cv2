#include "lib.h"

Point2i clickedPoint;
bool isClicked = false;

void mouseCallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	if (event == EVENT_LBUTTONDOWN && x < CAP_FRAME_WIDTH && y < CAP_FRAME_HEIGHT)
	{
		clickedPoint = Point2i(x, y);
		isClicked = true;
	}
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	srand(time(NULL));

	Detector detector;
	Mat colorFrame, grayFrame8U, grayFrame32F;
	Point2f currentOffset;

	const float refreshRate = 0.02f;
	const float deviationFactor = 5.5f;
	const float targetsFactor = 15.0f;
	const float scalingFactor = 20.0f;
	detector.deviationImgFillValue = 256.0f / targetsFactor;
	const float distanceBetweenTargets = 50.0f;
	
	const float distanceBetweenTargetsOnTwoFrames = 50.0f;
	const float scalingFactorBetweenTargetsOnTwoFrames = 4.0f;

	string videoSource;
	if (argc == 1 || argc == 2)
	{
		if (argc == 1)
		{
			videoSource = "";
		}
		else
		{
			videoSource = argv[1];
		}
	}
	else
	{
		cout << "Ќеверное число аргументов командной строки" << endl;
		return -1;
	}

	if (!detector.startCapture(videoSource))
		return -1;

	int frameNum = 0;
	while (true)
	{
		detector.cap >> colorFrame;

		cout << frameNum << endl;

		cvtColor(colorFrame, grayFrame8U, CV_RGB2GRAY);
		grayFrame8U.convertTo(grayFrame32F, CV_32F);

		if (grayFrame8U.empty())
		{
			detector.startCapture(videoSource);
			continue;
		}

		frameNum++;

		currentOffset = detector.calcFrameOffset(grayFrame8U);
		//detector.translateAverageBackAndDeviationImg(grayFrame32F, currentOffset);
		detector.calcFrameStaticPartMask(grayFrame32F, deviationFactor);
		detector.calcAverageBackAndDeviationImg(grayFrame32F, refreshRate);
		detector.brightestScaling(detector.deviationImg, scalingFactor);
		detector.calcTargetsBinaryFrame(grayFrame32F, targetsFactor);
		detector.makeSegmentation(distanceBetweenTargets);
		detector.haarFeatures.makeIntegralImg(grayFrame8U);

		if (detector.isTargetSelected)
		{
			detector.trekSelectedTarget(distanceBetweenTargetsOnTwoFrames, scalingFactorBetweenTargetsOnTwoFrames);

			if (detector.selectedTarget.exist)
				detector.trainClassifier();
			else
				detector.detectSelectedTarget();
			
            detector.displaySelectedTarget();
			detector.framesNum++;
		}

		for (int i = 0; i < detector.prevPoints.size(); i++)
			circle(detector.imgToDisplay[0], detector.prevPoints[i], 2, Scalar(255),5);

		detector.displayWindow();

		if (frameNum == 2)
		{
			imwrite("current_frame.jpg", detector.imgToDisplay[0]);
			//imwrite("deviation_image.jpg", detector.imgToDisplay[1]);
			//imwrite("average_backgroung.jpg", detector.imgToDisplay[2]);
			//imwrite("moving_target.jpg", detector.imgToDisplay[3]);
			break;
		}

		if (!detector.isTargetSelected)
		{
			setMouseCallback("Display window", mouseCallBackFunc, NULL);

			if (isClicked)
			{
				detector.selectTarget(clickedPoint);
				isClicked = false;
			}
		}

		detector.showStats();

		char key = (char)waitKey(1); // waitKey ждет событи€ нажати€ клавиши 1 мс
		if (key == 27) // ≈сли нажат ESC - выходим
		{
			break;
		}
		else if (key == ' ')
		{
			detector.needToInit = true;
			continue;
		}
	}

	return 0;
}