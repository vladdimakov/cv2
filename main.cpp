#include "lib.h"

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	
	const float refreshRate = 0.02f;
	const float deviationFactor = 3.5f;
	const float scalingFactor = 20.0f;

	CVFuns cvFuns;
	Mat colorFrame, grayFrame;
	Point2f currentOffset;

	if (!cvFuns.startCapture(argc, argv)) 
		return -1;

	while (true)
	{
		cvFuns.cap >> colorFrame;
		cvtColor(colorFrame, grayFrame, CV_RGB2GRAY);

		if (grayFrame.empty())
		{
			cvFuns.startCapture(argc, argv);
			continue;
		}

		currentOffset = cvFuns.calcFrameOffset(grayFrame);
		currentOffset = -currentOffset;

		cvFuns.translateAverageBackAndDeviationImg(grayFrame, currentOffset);

		cvFuns.calcFrameStaticPartMask(grayFrame, deviationFactor);

		cvFuns.calcAverageBackAndDeviationImg(grayFrame, refreshRate);

		cvFuns.brightestScaling(cvFuns.deviationImg, scalingFactor);

		cvFuns.displayWindow();

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