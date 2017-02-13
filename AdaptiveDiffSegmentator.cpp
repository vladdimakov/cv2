#include "AdaptiveDiffSegmentator.h"
#include <cmath>
#include <boost/chrono.hpp>

#include "../../../Debug.h"


void AdaptiveDiffSegmentator::Init(unsigned char *source, int sourceStep, int xStart, int yStart, int width, int height)
{
	_updateSpeedSmoother.Add(1.0);
	_diffSumFilter.SetWatchDogPeriod(10);

    _ipp32fDiffUpdateArea.Allocate(width, height);
    _ipp32fDiffImage.Allocate(width, height);
    _ipp32fMeanDiff.Allocate(width, height);
    _ipp32fMeanDiffEtalon.Allocate(width, height);

	_ipp32fEtalon.Allocate(width, height);
    _ipp32fBackground.Allocate(width, height);
	_ipp32fUpdateArea.Allocate(width, height);

	_ipp32fMask.Allocate(width, height);

	_u8Diff.Allocate(width, height);
	_u8Mask.Allocate(width, height);
    _rawDiff.Allocate(width, height);

    //Рабочая зона
    roi.width = width;
    roi.height = height;

    //Начальная инициализация _ipp32fEtalon - модельного изображения фона в рабочей зоне
    ippiConvert_8u32f_C1R(source + yStart * sourceStep + xStart, sourceStep, _ipp32fEtalon.get(), _ipp32fEtalon.GetPitch(), roi);

    //Начальная инициализация _ipp32fMeanDiffEtalon - модели среднего абсолютного разностного изображения
    ippiSet_32f_C1R(_startDiffFill, _ipp32fMeanDiffEtalon.get(), _ipp32fMeanDiffEtalon.GetPitch(), roi);
}

void AdaptiveDiffSegmentator::Reset(unsigned char *source, int sourceStep)
{
    printf("RESET\n");

    //Начальная инициализация _ipp32fEtalon - модельного изображения фона в рабочей зоне
    ippiConvert_8u32f_C1R(source, sourceStep, _ipp32fEtalon.get(), _ipp32fEtalon.GetPitch(), roi);

    //Начальная инициализация _ipp32fMeanDiffEtalon - модели среднего абсолютного разностного изображения
    ippiSet_32f_C1R(_startDiffFill, _ipp32fMeanDiffEtalon.get(), _ipp32fMeanDiffEtalon.GetPitch(), roi);

    //Сброс сглаживающего фильтра скорости обновления модели, установка максимальной скорости обновления
    _updateSpeedSmoother.Reset();_updateSpeedSmoother.Add(1.0);
}

void AdaptiveDiffSegmentator::getIntersectionArea(double _dx, double _dy)
{
    //Определение зоны перекрытия модели фона и текущего изображения
	int dx = (_dx > 0) ? int(_dx):int(_dx)-1;
	int dy = (_dy > 0) ? int(_dy):int(_dy)-1;
	fracDx = _dx - dx; 
	fracDy = _dy - dy;

    intersectionRoi.width = roi.width - abs(dx) - 1;
    intersectionRoi.height = roi.height - abs(dy) - 1;

	backgroundX = (dx > 0) ? dx: 0;
	backgroundY = (dy > 0) ? dy: 0;

	frameX = (dx > 0) ? 0: -dx;
	frameY = (dy > 0) ? 0: -dy;
}

void AdaptiveDiffSegmentator::formBackgroundAndDiffImage(unsigned char *source, int sourceStep)
{
    ippiConvert_8u32f_C1R(source, sourceStep, _ipp32fBackground.get(),  _ipp32fBackground.GetPitch(), roi);
    //_ipp32fBackground теперь содержит float версию рабочей зоны текущего кадра

    ippiCopy_32f_C1R(_ipp32fBackground.get(frameX, frameY), _ipp32fBackground.GetPitch(), _ipp32fUpdateArea.get(), _ipp32fUpdateArea.GetPitch(), intersectionRoi);
    //_ipp32fUpdateArea - зона перекрытия текущего кадра во float

    ippiCopySubpix_32f_C1R(_ipp32fEtalon.get(backgroundX, backgroundY), _ipp32fEtalon.GetPitch(), _ipp32fBackground.get(frameX, frameY), _ipp32fBackground.GetPitch(), intersectionRoi, fracDx, fracDy);
    //_ipp32fBackground - модельное изображение фона c учетом текущего смещения кадра (рабочий средний фон на данном кадре)

    ippiAbsDiff_32f_C1R(_ipp32fUpdateArea.get(), _ipp32fUpdateArea.GetPitch(), _ipp32fBackground.get(frameX, frameY), _ipp32fBackground.GetPitch(), _ipp32fDiffUpdateArea.get(), _ipp32fDiffUpdateArea.GetPitch(), intersectionRoi);
    //_ipp32fDiffUpdateArea - разность между текущим изображением и моделью фона в зоне перекрытия

    ippiSet_32f_C1R(_startDiffFill, _ipp32fMeanDiff.get(), _ipp32fMeanDiff.GetPitch(), roi);
    ippiCopySubpix_32f_C1R(_ipp32fMeanDiffEtalon.get(backgroundX, backgroundY), _ipp32fMeanDiffEtalon.GetPitch(), _ipp32fMeanDiff.get(frameX, frameY), _ipp32fMeanDiff.GetPitch(), intersectionRoi, fracDx, fracDy);
    //_ipp32fMeanDiff - модельное значение средней абсолютной разности с учетом смещения кадра

}


void AdaptiveDiffSegmentator::maskBuilder(double factor, unsigned char *mask, int maskStep, bool backgroundSelection)
{
    ippiSet_8u_C1R(0, mask, maskStep, roi);
    ippiMulC_32f_C1R(_ipp32fMeanDiff.get(), _ipp32fMeanDiff.GetPitch(), factor, _ipp32fMask.get(), _ipp32fMask.GetPitch(), roi);

    if(backgroundSelection)
    {
        //Селекция фона
        //Ставим 0 там, где разность больше средней по модели в данном месте не менее чем в factor раз, иначе ставим не 0
        ippiCompare_32f_C1R(_ipp32fDiffUpdateArea.get(), _ipp32fDiffUpdateArea.GetPitch(), _ipp32fMask.get(frameX, frameY), _ipp32fMask.GetPitch(), mask + maskStep * frameY + frameX, maskStep, intersectionRoi, ippCmpLessEq);
    }
    else
    {
        //Селекция объекта
        //Ставим 1 там, где разность больше средней по модели в данном месте не менее чем в factor раз, иначе ставим 0
        ippiCompare_32f_C1R(_ipp32fDiffUpdateArea.get(), _ipp32fDiffUpdateArea.GetPitch(), _ipp32fMask.get(frameX, frameY), _ipp32fMask.GetPitch(), mask + maskStep * frameY + frameX, maskStep, intersectionRoi, ippCmpGreater);
        ippiThreshold_GTVal_8u_C1IR(mask, maskStep, roi, 0, 1);
    }
}

void AdaptiveDiffSegmentator::updateBackgroundModel()
{

    //Порог обновления модели; чем порог больше, тем меньше обнаруживается объектов и тем
    //большая площадь фона включается в обновление
    //Чем быстрее обновляется сцена, тем больше фона надо включать в обновление
    double finalMaskThreshold = _maskThreshold + (_updateCoefficient - _minUpdateCoefficient) * _maskThresholdMul;

    //формируется маска для обновления модели (Не 0 - фон, 0 - объект)
    maskBuilder(finalMaskThreshold, _u8Mask.get(), _u8Mask.GetPitch(), true);

    //Обновляем по маске среднюю абсолютную разность в фоновой области
    ippiAddWeighted_32f_C1IMR(_ipp32fDiffUpdateArea.get(), _ipp32fDiffUpdateArea.GetPitch(), _u8Mask.get(frameX, frameY), _u8Mask.GetPitch(),
                              _ipp32fMeanDiff.get(frameX, frameY), _ipp32fMeanDiff.GetPitch(), intersectionRoi, _updateCoefficient);

    //Формируем эталон средней абсолютной разности
    ippiCopy_32f_C1R(_ipp32fMeanDiff.get(), _ipp32fMeanDiff.GetPitch(), _ipp32fMeanDiffEtalon.get(), _ipp32fMeanDiffEtalon.GetPitch(), roi);

    //Формируем эталон среднего фона в области, задаваемой фоновой маской
    ippiAddWeighted_32f_C1IMR(_ipp32fUpdateArea.get(), _ipp32fUpdateArea.GetPitch(), _u8Mask.get(frameX, frameY), _u8Mask.GetPitch(), _ipp32fBackground.get(frameX, frameY), _ipp32fBackground.GetPitch(), intersectionRoi, _updateCoefficient);
    ippiCopy_32f_C1R(_ipp32fBackground.get(), _ipp32fBackground.GetPitch(), _ipp32fEtalon.get(), _ipp32fEtalon.GetPitch(), roi);
}

void AdaptiveDiffSegmentator::formOutputImage(unsigned char *dst, int dstStep, int &globalThreshold)
{
    //Удаление шума из разностного изображения с преобразованием float -> uunsigned char
    //_rawDiff - необработанное разностное изображение

    ippiSet_32f_C1R(0, _ipp32fDiffImage.get(), _ipp32fDiffImage.GetPitch(), roi);
    ippiCopy_32f_C1R(_ipp32fDiffUpdateArea.get(), _ipp32fDiffUpdateArea.GetPitch(), _ipp32fDiffImage.get(frameX, frameY), _ipp32fDiffImage.GetPitch(), intersectionRoi);
    //_ipp32fDiffImage - модуль разности между текущим изображением и моделью фона в рабочей зоне

    ippiConvert_32f8u_C1R(_ipp32fDiffImage.get(), _ipp32fDiffImage.GetPitch(), _u8Diff.get(), _u8Diff.GetPitch(), roi, ippRndZero);
    ippiCopy_8u_C1R(_u8Diff.get(), _u8Diff.GetPitch(), _rawDiff.get(), _rawDiff.GetPitch(), roi);
    globalThreshold = _globalFilter.Process(_u8Diff.get(), _u8Diff.GetPitch(), roi.width, roi.height, _globalFilterAdjust);

    maskBuilder(_outputThreshold, dst, dstStep, false);
    //Только что выделили места, где может быть объект ... и записали 1 в маску, где выделили, иначе 0
    ippiMul_8u_C1IRSfs(_u8Diff.get(), _u8Diff.GetPitch(), dst, dstStep, roi, 0);
    //Здесь появляется окончательный результат: вырезаем из _u8Diff те места, где нет объекта согласно маске,
    //формируемой в предыдущей строке
}

void AdaptiveDiffSegmentator::GetRawDiffImage(unsigned char *result, int resultStep)
{
    ippiCopy_8u_C1R(_rawDiff.get(), _rawDiff.GetPitch(), result, resultStep, roi);
}

void AdaptiveDiffSegmentator::GetBackground(unsigned char *result, int resultStep)
{
    ippiConvert_32f8u_C1R(_ipp32fBackground.get(), _ipp32fBackground.GetPitch(), result, resultStep, roi, ippRndNear);
}

void AdaptiveDiffSegmentator::calculateUpdateSpeed(double dx, double dy)
{
    double updateSpeed = std::max(_updateSpeedFactor * fabs(dx) / roi.width, _updateSpeedFactor * fabs(dy) / roi.height);
    if(updateSpeed < _minUpdateCoefficient) updateSpeed  = _minUpdateCoefficient;
    if(updateSpeed > 1) updateSpeed = 1;

    _updateSpeedSmoother.Add(updateSpeed);
    _updateCoefficient = _updateSpeedSmoother.GetAverage();
}

bool AdaptiveDiffSegmentator::modelValidator(unsigned char *result, int resultStep, unsigned char *source, int sourceStep)
{
    double sum;
    ippiSum_8u_C1R(result, resultStep, roi, &sum);
    if(sum <= 0) return false;

    _diffSumFilter.Add(sum / (roi.width * roi.height));

    //Сбрасываем модель при слишком большой средней яркости разностного изображения
    if(!_diffSumFilter.Status())
    {
        Reset(source, sourceStep);
        ippiSet_8u_C1R(0, result, resultStep, roi);
        return false;
    }

    MSG(102,"ADSum: %g", sum);

    return true;
}

bool AdaptiveDiffSegmentator::Process(unsigned char *source, int sourceStep, int xStart, int yStart, double _dx, double _dy, unsigned char *result, int resultStep, int &globalThreshold)
{
    _workCounter++;
    MSG(102,"WC: %d",_workCounter)

    calculateUpdateSpeed(_dx, _dy);
    MSG(102,"Update speed: %g",_updateCoefficient);

    getIntersectionArea(_dx, _dy);

    unsigned char* activeSourceZone = source + yStart * sourceStep + xStart;

    formBackgroundAndDiffImage(activeSourceZone, sourceStep);
    formOutputImage(result, resultStep, globalThreshold);
    updateBackgroundModel();

    return modelValidator(result, resultStep, activeSourceZone, sourceStep);
}
