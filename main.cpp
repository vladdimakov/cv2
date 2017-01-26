#include "lib.h"

int main(int argc, char** argv)
{
	setlocale(LC_ALL, "Russian");
	CVFuns cvFuns;
	Mat colorFrame, grayFrame, stabilizedGrayFrame;

	if (cvFuns.startCapture()) 
		cout << "Камера открыта" << endl;
	else
		return -1;

	Point2f currentOffset;
    const double refreshRate = 0.01;
	Mat C, translatedAverageBackImg;

	while (true)
	{
		cvFuns.cap >> colorFrame;
		cvtColor(colorFrame, grayFrame, CV_RGB2GRAY);

		currentOffset = cvFuns.calcFrameOffset(grayFrame);
		cvFuns.offset += currentOffset;

		cvFuns.imgToDisplay[2] = cvFuns.translateFrame(grayFrame, cvFuns.offset);

        currentOffset.x = -currentOffset.x;
        currentOffset.y = -currentOffset.y;
        //printf("%g %g\n",currentOffset.x, currentOffset.y);fflush(stdout);

        C = cvFuns.translateFrame(cvFuns.averageBackImage, grayFrame, currentOffset);

		for (int i = 0; i < CAP_FRAME_HEIGHT; i++)
		{
			for (int j = 0; j < CAP_FRAME_WIDTH; j++)
			{
				cvFuns.averageBackImage.at<uchar>(i, j) = (1 - refreshRate) * C.at<uchar>(i, j) + refreshRate * grayFrame.at<uchar>(i, j);
			}
		}

		cvFuns.imgToDisplay[3] = cvFuns.averageBackImage;

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
