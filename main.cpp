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

	CVFuns cvFuns;
	Mat colorFrame, grayFrame8U, grayFrame32F;
	Point2f currentOffset;

	const float refreshRate = 0.02f;
	const float deviationFactor = 5.5f;
	const float targetsFactor = 15.0f;
	const float scalingFactor = 20.0f;
	cvFuns.deviationImgFillValue = 256.0f / targetsFactor;
	const float distanceBetweenTargets = 50.0f;
	
	const float distanceBetweenTargetsOnTwoFrames = 50.0f;
	const float scalingFactorBetweenTargetsOnTwoFrames = 4.0f;
	
	cvFuns.preliminaryTrainingFramesNum = 300;
	cvFuns.classifierNotFoundTargetMaxNum = 10;

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

	if (!cvFuns.startCapture(videoSource))
		return -1;

	int classificatorMode = 0;
	
	int frameNum = 0;
	while (true)
	{
		cvFuns.cap >> colorFrame;

		cout << frameNum << endl;

		cvtColor(colorFrame, grayFrame8U, CV_RGB2GRAY);
		grayFrame8U.convertTo(grayFrame32F, CV_32F);

		if (grayFrame8U.empty())
		{
			cvFuns.startCapture(videoSource);
			continue;
		}

		frameNum++;

		currentOffset = cvFuns.calcFrameOffset(grayFrame8U);
		cvFuns.translateAverageBackAndDeviationImg(grayFrame32F, currentOffset);
		cvFuns.calcFrameStaticPartMask(grayFrame32F, deviationFactor);
		cvFuns.calcAverageBackAndDeviationImg(grayFrame32F, refreshRate);
		cvFuns.brightestScaling(cvFuns.deviationImg, scalingFactor);
		cvFuns.calcTargetsBinaryFrame(grayFrame32F, targetsFactor);
		cvFuns.makeSegmentation(distanceBetweenTargets);
		cvFuns.makeIntegralImg(grayFrame8U);

		if (frameNum == 1022)
		{
			imwrite("current_frame.jpg", cvFuns.imgToDisplay[0]);
			imwrite("deviation_image.jpg", cvFuns.imgToDisplay[1]);
			imwrite("average_backgroung.jpg", cvFuns.imgToDisplay[2]);
			imwrite("moving_target.jpg", cvFuns.imgToDisplay[3]);
			
			Mat currentDeviationImage = abs(grayFrame32F - cvFuns.averageBackImg) * scalingFactor;
			currentDeviationImage.convertTo(currentDeviationImage, CV_8U);
			imwrite("current_deviation_image.jpg", currentDeviationImage);

			currentDeviationImage = abs(grayFrame32F - cvFuns.averageBackImg);
			currentDeviationImage.convertTo(currentDeviationImage, CV_8U);
			imwrite("current_deviation_image.bmp", currentDeviationImage);
			break;
		}

		if (classificatorMode == 0 && cvFuns.isTargetSelected)
		{
			cvFuns.findSelectedTarget(distanceBetweenTargetsOnTwoFrames, scalingFactorBetweenTargetsOnTwoFrames);
            cvFuns.displaySelectedTarget();
			cvFuns.trainClassifier();
		}

		if (classificatorMode == 1 && cvFuns.isTargetSelected)
		{
			cvFuns.classifyAndTrain(distanceBetweenTargetsOnTwoFrames, scalingFactorBetweenTargetsOnTwoFrames);
			cvFuns.displaySelectedTarget();
		}
		
		cvFuns.displayWindow();

		if (classificatorMode == 0 && !cvFuns.isTargetSelected)
		{
			setMouseCallback("Display window", mouseCallBackFunc, NULL);

			if (isClicked)
			{
				cvFuns.selectTarget(clickedPoint);
				isClicked = false;
			}
		}

		if (cvFuns.isTargetSelected)
		{
			cvFuns.framesNum++;
			
			if (cvFuns.framesNum == cvFuns.preliminaryTrainingFramesNum)
			{
				cout << "\n|ѕредварительное обучение классификатора закончено|\n" << endl;
				classificatorMode = 1;
			}
		}

		cvFuns.showStats();

		char key = (char)waitKey(1); // waitKey ждет событи€ нажати€ клавиши 1 мс
		if (key == 27) // ≈сли нажат ESC - выходим
		{
			break;
		}
		else if (key == ' ')
		{
			cvFuns.needToInit = true;
			continue;
		}
	}

	return 0;
}