#include "lib.h"

FPSCounter::FPSCounter()
{
	oldTime = clock();
	FPSCnt = 0;
	oldFPS = 0;
}

float FPSCounter::value()
{
	FPSCnt++;

	time_t nowTime = clock();
	float elapsedTime = (float)(nowTime - oldTime) / CLOCKS_PER_SEC;

	if (elapsedTime >= 1)
	{
		oldFPS = FPSCnt / elapsedTime;
		oldTime = nowTime;
		FPSCnt = 0;
	}

	return oldFPS;
}

