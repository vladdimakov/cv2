#include "lib.h"

int main(int argc, char** argv)
{
	setlocale(LC_ALL, "Russian");
	CVFuns cvFuns;
	Mat colorFrame, grayFrame;

	if (!cvFuns.startCapture()) 
		return -1;

	Point2f currentOffset;

    const float refreshRate = 0.05f;
    const float deviationFactor = 2.0f;
	const float scalingFactor = 2.0f;

	while (true)
	{
		cvFuns.cap >> colorFrame;
		cvtColor(colorFrame, grayFrame, CV_RGB2GRAY);

		if (grayFrame.empty())
		{
			return -1;
		}

		currentOffset = cvFuns.calcFrameOffset(grayFrame);
		cvFuns.offset += currentOffset;

		//cvFuns.imgToDisplay[1] = Mat::zeros(CAP_FRAME_HEIGHT, CAP_FRAME_WIDTH, CV_8U);
		//cvFuns.translateFrame(grayFrame, cvFuns.imgToDisplay[1], cvFuns.offset);
		//cvFuns.imgToDisplayInfo[1] = "Stabilized frame";

		cvFuns.deviationFromAverageBackImg(grayFrame, refreshRate);
		cvFuns.brightestScaling(cvFuns.deviationImg, scalingFactor);

		cvFuns.calcAverageBackImg(grayFrame, currentOffset, refreshRate, deviationFactor);

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