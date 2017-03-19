#include "lib.h"

bool CVFuns::calcHaarFeature0(Object rectangle)
{
	Object partA = rectangle;
	Object partB = rectangle;

	int halfHeight = (rectangle.bottom - rectangle.top) / 2;

	partA.bottom = rectangle.top + halfHeight;
	partB.top = rectangle.top + halfHeight + 1;

	return calcIntegralSumForRectangle(partA) > calcIntegralSumForRectangle(partB);
}

bool CVFuns::calcHaarFeature1(Object rectangle)
{
	Object partA = rectangle;
	Object partB = rectangle;

	int halfWidth = (rectangle.right - rectangle.left) / 2;

	partA.right = rectangle.left + halfWidth;
	partB.left = rectangle.left + halfWidth + 1;

	return calcIntegralSumForRectangle(partA) > calcIntegralSumForRectangle(partB);
}

bool CVFuns::calcHaarFeature2(Object rectangle)
{
	Object partA = rectangle;
	Object partB = rectangle;
	Object partC = rectangle;
	Object partD = rectangle;

	int halfWidth = (rectangle.right - rectangle.left) / 2;
	int halfHeight = (rectangle.bottom - rectangle.top) / 2;

	partA.right = rectangle.left + halfWidth;
	partA.bottom = rectangle.top + halfHeight;

	partB.left = partA.right + 1;
	partB.bottom = partA.bottom;

	partC.right = partA.right;
	partC.top = partA.bottom + 1;

	partD.left = partB.left;
	partD.top = partC.top;

	int A = calcIntegralSumForRectangle(partA);
	int B = calcIntegralSumForRectangle(partB);
	int C = calcIntegralSumForRectangle(partC);
	int D = calcIntegralSumForRectangle(partD);

	return A + B > C + D && A > B;
}

bool CVFuns::calcHaarFeature3(Object rectangle)
{
	Object partA = rectangle;
	Object partB = rectangle;
	Object partC = rectangle;
	Object partD = rectangle;

	int halfWidth = (rectangle.right - rectangle.left) / 2;
	int halfHeight = (rectangle.bottom - rectangle.top) / 2;

	partA.right = rectangle.left + halfWidth;
	partA.bottom = rectangle.top + halfHeight;

	partB.left = partA.right + 1;
	partB.bottom = partA.bottom;

	partC.right = partA.right;
	partC.top = partA.bottom + 1;

	partD.left = partB.left;
	partD.top = partC.top;

	int A = calcIntegralSumForRectangle(partA);
	int B = calcIntegralSumForRectangle(partB);
	int C = calcIntegralSumForRectangle(partC);
	int D = calcIntegralSumForRectangle(partD);

	return A + D > B + C;
}

bool CVFuns::calcHaarFeature4(Object rectangle)
{
	Object partA = rectangle;
	Object partB = rectangle;
	Object partC = rectangle;
	
	int thirdWidth = (rectangle.right - rectangle.left) / 3;

	partA.left = rectangle.left + thirdWidth;
	partA.right = rectangle.right - thirdWidth;
	
	partB.right = partA.left - 1;

	partC.left = partA.right + 1;

	return calcIntegralSumForRectangle(partA) > calcIntegralSumForRectangle(partB) + calcIntegralSumForRectangle(partC);
}

bool CVFuns::calcHaarFeature5(Object rectangle)
{
	Object partA = rectangle;
	Object partB = rectangle;
	Object partC = rectangle;

	int thirdHeight = (rectangle.bottom - rectangle.top) / 3;

	partA.top = rectangle.top + thirdHeight;
	partA.bottom = rectangle.bottom - thirdHeight;

	partB.bottom = partA.top - 1;

	partC.top = partA.bottom + 1;

	return calcIntegralSumForRectangle(partA) > calcIntegralSumForRectangle(partB) + calcIntegralSumForRectangle(partC);
}

bool CVFuns::calcHaarFeature6(Object rectangle)
{
	Object partA = rectangle;
	Object partB = rectangle;
	Object partC = rectangle;
	Object partD = rectangle;
	Object partE = rectangle;
	Object partF = rectangle;
	Object partG = rectangle;
	Object partH = rectangle;
	Object partI = rectangle;

	int thirdWidth = (rectangle.right - rectangle.left) / 3;
	int thirdHeight = (rectangle.bottom - rectangle.top) / 3;

	partA.left = rectangle.left + thirdWidth;
	partA.right = rectangle.right - thirdWidth;
	partA.top = rectangle.top + thirdWidth;
	partA.bottom = rectangle.bottom - thirdWidth;

	partB.right = partA.left - 1;
	partB.bottom = partA.top - 1;

	partC.left = partA.left;
	partC.right = partA.right;
	partC.bottom = partB.bottom;

	partD.left = partA.right + 1;
	partD.bottom = partB.bottom;

	partE.right = partB.right;
	partE.top = partA.top;
	partE.bottom = partA.bottom;

	partF.left = partD.left;
	partF.top = partA.top;
	partF.bottom = partA.bottom;

	partG.right = partB.right;
	partG.top = partA.bottom + 1;
	
	partH.left = partC.left;
	partH.right = partC.right;
	partH.top = partG.top;

	partI.left = partD.left;
	partI.top = partG.top;

	int A = calcIntegralSumForRectangle(partA);
	int B = calcIntegralSumForRectangle(partB);
	int C = calcIntegralSumForRectangle(partC);
	int D = calcIntegralSumForRectangle(partD);
	int E = calcIntegralSumForRectangle(partE);
	int F = calcIntegralSumForRectangle(partF);
	int G = calcIntegralSumForRectangle(partG);
	int H = calcIntegralSumForRectangle(partH);
	int I = calcIntegralSumForRectangle(partI);
	
	return A > B + C + D + E + F + G + H + I;
}

bool CVFuns::calcHaarFeatures(Object rectangle, int featureType)
{
	switch (featureType)
	{
	case 0:
		return calcHaarFeature0(rectangle);
		break;
	case 1:
		return calcHaarFeature1(rectangle);
		break;
	case 2:
		return calcHaarFeature2(rectangle);
		break;
	case 3:
		return calcHaarFeature3(rectangle);
		break;
	case 4:
		return calcHaarFeature4(rectangle);
		break;
	case 5:
		return calcHaarFeature5(rectangle);
		break;
	case 6:
		return calcHaarFeature6(rectangle);
		break;
	}
}