#include "lib.h"

int main(int argc, char** argv)
{
	setlocale(LC_ALL, "Russian");
	CVFuns cvFuns;
	Mat colorFrame, grayFrame;

	if (cvFuns.startCapture()) 
		cout << "������ �������" << endl;
	else
		return -1;

	Point2f currentOffset;

	while (true)
	{
		cvFuns.cap >> colorFrame;
		cvtColor(colorFrame, grayFrame, CV_RGB2GRAY);

		currentOffset = cvFuns.calcFrameOffset(grayFrame);
		cvFuns.offset += currentOffset;

		cvFuns.imgToDisplay[2] = Mat::zeros(CAP_FRAME_HEIGHT, CAP_FRAME_WIDTH, CV_8U);
		cvFuns.translateFrame(grayFrame, cvFuns.imgToDisplay[2], cvFuns.offset);

		cvFuns.calcAverageBackImg(grayFrame, currentOffset);       

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