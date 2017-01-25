#include "lib.h"

int main(int argc, char** argv)
{
	setlocale(LC_ALL, "Russian");
	CVFuns cvFuns;
	Mat colorFrame, GrayFrame, stabilizedGrayFrame;

	if (cvFuns.startCapture()) 
		cout << "������ �������" << endl;
	else
		return -1;
	
	while (true)
	{
		cvFuns.cap >> colorFrame;
		cvtColor(colorFrame, GrayFrame, CV_RGB2GRAY);

		stabilizedGrayFrame = cvFuns.stabilizeFrame(GrayFrame);
		stabilizedGrayFrame.copyTo(cvFuns.imgToDisplay[2]);

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