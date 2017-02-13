#include "lib.h"

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	
	const float refreshRate = 0.05f;
	const float deviationFactor = 2.0f;
	const float scalingFactor = 20.0f;

	CVFuns cvFuns;
	Mat colorFrame, grayFrame;
	Mat grayFrameStaticPartMask = Mat(CAP_FRAME_HEIGHT, CAP_FRAME_WIDTH, CV_8U, Scalar(255));
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

		cvFuns.calcAverageBackImg(grayFrame, currentOffset, refreshRate);

		cvFuns.calcAverageDeviationImg(grayFrame, currentOffset, refreshRate);
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