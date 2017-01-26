#include "lib.h"

int main(int argc, char** argv)
{
	setlocale(LC_ALL, "Russian");
	CVFuns cvFuns;
	FPSCounter FPScounter;
	Mat colorFrame, grayFrame, stabilizedGrayFrame;

	if (cvFuns.startCapture()) 
		cout << "Камера открыта" << endl;
	else
		return -1;

	Point2f currentOffset;

	while (true)
	{
		cvFuns.cap >> colorFrame;
		cvtColor(colorFrame, grayFrame, CV_RGB2GRAY);

		currentOffset = cvFuns.calcFrameOffset(grayFrame);
		cvFuns.offset += currentOffset;

		cvFuns.imgToDisplay[2] = cvFuns.translateFrame(grayFrame, cvFuns.offset);

		cvFuns.calcAverageBackImg(grayFrame, currentOffset);       

		cvFuns.displayWindow();
		
		cout << "FPS: " << FPScounter.value() << endl;

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