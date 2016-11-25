#include <time.h> 

using namespace std;

class FPSCounter
{
public:
	FPSCounter()
	{
		oldTime = clock();
		FPSCnt = 0;
		oldFPS = 0;
	}
	size_t value()
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
private:
	clock_t oldTime;
	size_t FPSCnt, oldFPS;
};