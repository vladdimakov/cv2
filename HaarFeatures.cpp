#include "lib.h"

HaarFeatures::HaarFeatures()
{
	integralImg = Mat(CAP_FRAME_HEIGHT + 1, CAP_FRAME_WIDTH + 1, CV_32S);
}

void HaarFeatures::makeIntegralImg(Mat currentFrame)
{
	integral(currentFrame, integralImg);
}

int HaarFeatures::calcIntegralSumForRegion(Object region)
{
	return integralImg.at<int>(region.bottom + 1, region.right + 1) -
		integralImg.at<int>(region.top, region.right + 1) -
		integralImg.at<int>(region.bottom + 1, region.left) +
		integralImg.at<int>(region.top, region.left);
}

bool HaarFeatures::calcFeature0(Object region)
{
	Object partA = region;
	Object partB = region;

	int halfHeight = (region.bottom - region.top) / 2;

	partA.bottom = region.top + halfHeight;
	partB.top = region.top + halfHeight + 1;

	return calcIntegralSumForRegion(partA) > calcIntegralSumForRegion(partB);
}

bool HaarFeatures::calcFeature1(Object region)
{
	Object partA = region;
	Object partB = region;

	int halfHeight = (region.bottom - region.top) / 2;

	partA.bottom = region.top + halfHeight;
	partB.top = region.top + halfHeight + 1;

	return calcIntegralSumForRegion(partA) < calcIntegralSumForRegion(partB);
}

bool HaarFeatures::calcFeature2(Object region)
{
	Object partA = region;
	Object partB = region;

	int halfWidth = (region.right - region.left) / 2;

	partA.right = region.left + halfWidth;
	partB.left = region.left + halfWidth + 1;

	return calcIntegralSumForRegion(partA) > calcIntegralSumForRegion(partB);
}

bool HaarFeatures::calcFeature3(Object region)
{
	Object partA = region;
	Object partB = region;

	int halfWidth = (region.right - region.left) / 2;

	partA.right = region.left + halfWidth;
	partB.left = region.left + halfWidth + 1;

	return calcIntegralSumForRegion(partA) < calcIntegralSumForRegion(partB);
}

bool HaarFeatures::calcFeature4(Object region)
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

bool HaarFeatures::calcFeature5(Object region)
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

bool HaarFeatures::calcFeature6(Object region)
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

bool HaarFeatures::calcFeature7(Object region)
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

Object HaarFeatures::rescaleFeaturePosition(Object region, Object featurePosition)
{
	Object newFeaturePosition;

	int regionWidth = region.right - region.left;
	int regionHeight = region.bottom - region.top;

	newFeaturePosition.left = region.left + int(featurePosition.left / 100.0f * regionWidth);
	newFeaturePosition.right = region.left + int(featurePosition.right / 100.0f * regionWidth);
	newFeaturePosition.top = region.top + int(featurePosition.top / 100.0f * regionHeight);
	newFeaturePosition.bottom = region.top + int(featurePosition.bottom / 100.0f * regionHeight);

	return newFeaturePosition;
}

int HaarFeatures::calcFeatures(Object region, Object featurePosition, int featureType)
{
	switch (featureType)
	{
	case 0:
		return (int)calcFeature0(rescaleFeaturePosition(region, featurePosition));
		break;
	case 1:
		return (int)calcFeature1(rescaleFeaturePosition(region, featurePosition));
		break;
	case 2:
		return (int)calcFeature2(rescaleFeaturePosition(region, featurePosition));
		break;
	case 3:
		return (int)calcFeature3(rescaleFeaturePosition(region, featurePosition));
		break;
	case 4:
		return (int)calcFeature4(rescaleFeaturePosition(region, featurePosition));
		break;
	case 5:
		return (int)calcFeature5(rescaleFeaturePosition(region, featurePosition));
		break;
	case 6:
		return (int)calcFeature6(rescaleFeaturePosition(region, featurePosition));
		break;
	case 7:
		return (int)calcFeature7(rescaleFeaturePosition(region, featurePosition));
		break;
	}
}