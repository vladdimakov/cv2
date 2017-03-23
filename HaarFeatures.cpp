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

	int halfHeight = (region.bottom - region.top) / 2;

	partA.bottom = region.top + halfHeight;
	partB.top = region.top + halfHeight + 1;

	return calcIntegralSumForRegion(partA) < calcIntegralSumForRegion(partB);
}

bool CVFuns::calcHaarFeature2(Object region)
{
	Object partA = region;
	Object partB = region;

	int halfWidth = (region.right - region.left) / 2;

	partA.right = region.left + halfWidth;
	partB.left = region.left + halfWidth + 1;

	return calcIntegralSumForRegion(partA) > calcIntegralSumForRegion(partB);
}

bool CVFuns::calcHaarFeature3(Object region)
{
	Object partA = region;
	Object partB = region;

	int halfWidth = (region.right - region.left) / 2;

	partA.right = region.left + halfWidth;
	partB.left = region.left + halfWidth + 1;

	return calcIntegralSumForRegion(partA) < calcIntegralSumForRegion(partB);
}

bool CVFuns::calcHaarFeature4(Object region)
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

bool CVFuns::calcHaarFeature5(Object region)
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

	return A + B > C + D && A < B;
}

bool CVFuns::calcHaarFeature6(Object region)
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

	return C + D > A + B && C > D;
}

bool CVFuns::calcHaarFeature7(Object region)
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

	return C + D > A + B && C < D;
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
	case 7:
		return (int)calcHaarFeature7(region);
		break;
	}
}