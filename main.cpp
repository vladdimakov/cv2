#include "lib.h"

int main(int argc, char** argv)
{
	setlocale(LC_ALL, "Russian");
	CVFuns cvFuns;
	Mat colorFrame, grayFrame;

	if (cvFuns.startCapture()) 
		cout << "Камера открыта" << endl;
	else
		return -1;

	Point2f currentOffset;

	const float refreshRate = 0.01;
	const float scalingFactor = 2;

	while (true)
	{
		cvFuns.cap >> colorFrame;
		cvtColor(colorFrame, grayFrame, CV_RGB2GRAY);

		currentOffset = cvFuns.calcFrameOffset(grayFrame);
		cvFuns.offset += currentOffset;

		cvFuns.imgToDisplay[1] = Mat::zeros(CAP_FRAME_HEIGHT, CAP_FRAME_WIDTH, CV_8U);
		cvFuns.translateFrame(grayFrame, cvFuns.imgToDisplay[1], cvFuns.offset);
		cvFuns.imgToDisplayInfo[1] = "Stabilized frame";

		cvFuns.calcAverageBackImg(grayFrame, currentOffset, refreshRate);

		cvFuns.deviationFromAverageBackImg(grayFrame, scalingFactor);

		cvFuns.displayWindow();

		char key = (char)waitKey(1); // waitKey ждет события нажатия клавиши 1 мс
		if (key == 27) // Если нажат ESC - выходим
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