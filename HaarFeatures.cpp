#include "lib.h"

bool CVFuns::calcHaarFeature0(Object region)
{
	Object partA = region;
	Object partB = region;

	int halfHeight = (region.bottom - region.top) / 2;

	partA.bottom = region.top + halfHeight;
	partB.top = region.top + halfHeight + 1;

	return calcIntegralSumForRegion(partA) > calcIntegralSumForRegion(partB);
}

bool CVFuns::calcHaarFeature1(Object region)
{
	Object partA = region;
	Object partB = region;

	int halfWidth = (region.right - region.left) / 2;

	partA.right = region.left + halfWidth;
	partB.left = region.left + halfWidth + 1;

	return calcIntegralSumForRegion(partA) > calcIntegralSumForRegion(partB);
}

bool CVFuns::calcHaarFeature2(Object region)
{
	Object partA = region;
	Object partB = region;
	Object partC = region;
	Object partD = region;

	int halfWidth = (region.right - region.left) / 2;
	int halfHeight = (region.bottom - region.top) / 2;

	partA.right = region.left + halfWidth;
	partA.bottom = region.top + halfHeight;

	partB.left = partA.right + 1;
	partB.bottom = partA.bottom;

	partC.right = partA.right;
	partC.top = partA.bottom + 1;

	partD.left = partB.left;
	partD.top = partC.top;

	int A = calcIntegralSumForRegion(partA);
	int B = calcIntegralSumForRegion(partB);
	int C = calcIntegralSumForRegion(partC);
	int D = calcIntegralSumForRegion(partD);

	return A + B > C + D && A > B;
}

bool CVFuns::calcHaarFeature3(Object region)
{
	Object partA = region;
	Object partB = region;
	Object partC = region;
	Object partD = region;

	int halfWidth = (region.right - region.left) / 2;
	int halfHeight = (region.bottom - region.top) / 2;

	partA.right = region.left + halfWidth;
	partA.bottom = region.top + halfHeight;

	partB.left = partA.right + 1;
	partB.bottom = partA.bottom;

	partC.right = partA.right;
	partC.top = partA.bottom + 1;

	partD.left = partB.left;
	partD.top = partC.top;

	int A = calcIntegralSumForRegion(partA);
	int B = calcIntegralSumForRegion(partB);
	int C = calcIntegralSumForRegion(partC);
	int D = calcIntegralSumForRegion(partD);

	return A + D > B + C;
}

bool CVFuns::calcHaarFeature4(Object region)
{
	Object partA = region;
	Object partB = region;
	Object partC = region;
	
	int thirdWidth = (region.right - region.left) / 3;

	partA.left = region.left + thirdWidth;
	partA.right = region.right - thirdWidth;
	
	partB.right = partA.left - 1;

	partC.left = partA.right + 1;

	return calcIntegralSumForRegion(partA) > calcIntegralSumForRegion(partB) + calcIntegralSumForRegion(partC);
}

bool CVFuns::calcHaarFeature5(Object region)
{
	Object partA = region;
	Object partB = region;
	Object partC = region;

	int thirdHeight = (region.bottom - region.top) / 3;

	partA.top = region.top + thirdHeight;
	partA.bottom = region.bottom - thirdHeight;

	partB.bottom = partA.top - 1;

	partC.top = partA.bottom + 1;

	return calcIntegralSumForRegion(partA) > calcIntegralSumForRegion(partB) + calcIntegralSumForRegion(partC);
}

bool CVFuns::calcHaarFeature6(Object region)
{
	Object partA = region;
	Object partB = region;
	Object partC = region;
	Object partD = region;
	Object partE = region;
	Object partF = region;
	Object partG = region;
	Object partH = region;
	Object partI = region;

	int thirdWidth = (region.right - region.left) / 3;
	int thirdHeight = (region.bottom - region.top) / 3;

	partA.left = region.left + thirdWidth;
	partA.right = region.right - thirdWidth;
	partA.top = region.top + thirdWidth;
	partA.bottom = region.bottom - thirdWidth;

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

	int A = calcIntegralSumForRegion(partA);
	int B = calcIntegralSumForRegion(partB);
	int C = calcIntegralSumForRegion(partC);
	int D = calcIntegralSumForRegion(partD);
	int E = calcIntegralSumForRegion(partE);
	int F = calcIntegralSumForRegion(partF);
	int G = calcIntegralSumForRegion(partG);
	int H = calcIntegralSumForRegion(partH);
	int I = calcIntegralSumForRegion(partI);
	
	return A > B + C + D + E + F + G + H + I;
}

int CVFuns::calcHaarFeatures(Object region, int featureType)
{
	switch (featureType)
	{
	case 0:
		return (int)calcHaarFeature0(region);
		break;
	case 1:
		return (int)calcHaarFeature1(region);
		break;
	case 2:
		return (int)calcHaarFeature2(region);
		break;
	case 3:
		return (int)calcHaarFeature3(region);
		break;
	case 4:
		return (int)calcHaarFeature4(region);
		break;
	case 5:
		return (int)calcHaarFeature5(region);
		break;
	case 6:
		return (int)calcHaarFeature6(region);
		break;
	}
}