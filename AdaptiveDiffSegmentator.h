#ifndef ADAPTIVE_DIFF_SEGMENTATOR
#define ADAPTIVE_DIFF_SEGMENTATOR

#include <ippdefs.h>
#include <ippcore.h>
#include <ipps.h>
#include <ippi.h>
#include <ippcv.h>

#include <boost/shared_array.hpp>

#include "../diffImageFilter/DiffImageFilter.h"

#include "../../../auxx/sizeFilter/SizeFilter.h"
#include "../../../auxx/ippHelper/IppiImage8u.h"
#include "../../../auxx/ippHelper/IppiImage32f.h"

class AdaptiveDiffSegmentator
{
public:
    AdaptiveDiffSegmentator():
      _updateSpeedFactor(50),
      _minUpdateCoefficient(0.05), _updateCoefficient(_minUpdateCoefficient),
      _diffSumFilter(0.01,0,100),_updateSpeedSmoother(0.1,0,100),
      _workCounter(0),
      _maskThreshold(5.5), _maskThresholdMul(5),
      _outputThreshold(4.0),
      _globalFilterAdjust(0.3),
      _startDiffFill(256.0/_outputThreshold)
    {}

	void Init(unsigned char *source, int sourceStep, int xStart, int yStart, int width, int height);
    void Reset(unsigned char *source, int sourceStep);
	bool Process(unsigned char *source, int sourceStep, int xStart, int yStart, double dx, double dy, unsigned char *result, int resultStep, int &globalThreshold);
    void GetRawDiffImage(unsigned char *result, int resultStep);
    void GetBackground(unsigned char *result, int resultStep);

private:
    void calculateUpdateSpeed(double dx, double dy);
    void getIntersectionArea(double _dx, double _dy);
    void formBackgroundAndDiffImage(unsigned char *source, int sourceStep);
    void formOutputImage(unsigned char *dst, int dstStep, int &globalThreshold);
    void maskBuilder(double factor, unsigned char *mask, int maskStep, bool backgroundSelection);
    void updateBackgroundModel();
    bool modelValidator(unsigned char *result, int resultStep, unsigned char *source, int sourceStep);

	IppiImage32f _ipp32fEtalon;
	
	IppiImage32f _ipp32fMeanDiffEtalon;
	IppiImage32f _ipp32fDiffUpdateArea;
    IppiImage32f _ipp32fDiffImage;
	IppiImage32f _ipp32fMeanDiff;
	IppiImage32f _ipp32fMask;

	IppiImage32f _ipp32fBackground;
    IppiImage32f _ipp32fUpdateArea;

    IppiImage8u _u8Diff;
    IppiImage8u _u8Mask;
    IppiImage8u _rawDiff;

    double _updateSpeedFactor;
    double _minUpdateCoefficient;
    double _updateCoefficient;

	SizeFilter _diffSumFilter;
	SizeFilter _updateSpeedSmoother;

	DiffImageFilter _globalFilter;

	int _workCounter;

    double _maskThreshold;
    double _maskThresholdMul;
    double _outputThreshold;
    double _globalFilterAdjust;
    float  _startDiffFill;

    IppiSize roi;//Рабочая зонаж
    IppiSize intersectionRoi;//Зона перекрытия
    int backgroundX, backgroundY, frameX, frameY;
    double fracDx, fracDy;
};

#endif
