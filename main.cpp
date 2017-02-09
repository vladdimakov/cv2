#include "lib.h"

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	CVFuns cvFuns;
	Mat colorFrame, grayFrame;

	if (!cvFuns.startCapture(argc, argv)) 
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
			cvFuns.startCapture(argc, argv);
			continue;
		}

		currentOffset = cvFuns.calcFrameOffset(grayFrame);
		currentOffset = -currentOffset;

		cvFuns.deviationFromAverageBackImg(grayFrame, currentOffset, refreshRate);
		cvFuns.brightestScaling(cvFuns.deviationImg, scalingFactor);

		cvFuns.calcAverageBackImg(grayFrame, currentOffset, refreshRate, deviationFactor);

		cvFuns.displayWindow();

		char key = (char)waitKey(1); // waitKey ���� ������� ������� ������� 1 ��
		if (key == 27) // ���� ����� ESC - �������
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