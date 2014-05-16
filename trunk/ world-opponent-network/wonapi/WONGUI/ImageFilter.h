#ifndef __IMAGEFILTER_H__
#define __IMAGEFILTER_H__

#include "RawImage.h"
#include <list>

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ImageFilter : public RefCount
{
public:
	virtual RawImagePtr Filter(RawImage *theImage) { return theImage; }
};
typedef SmartPtr<ImageFilter> ImageFilterPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MultiImageFilter : public ImageFilter
{
protected:
	typedef std::list<ImageFilterPtr> FilterList;
	FilterList mFilterList;

public:
	MultiImageFilter() { }
	MultiImageFilter(ImageFilter *f1) { Add(f1); }
	MultiImageFilter(ImageFilter *f1, ImageFilter *f2) { Add(f1); Add(f2); }
	MultiImageFilter(ImageFilter *f1, ImageFilter *f2, ImageFilter *f3) { Add(f1); Add(f2); Add(f3); }
	MultiImageFilter(ImageFilter *f1, ImageFilter *f2, ImageFilter *f3, ImageFilter *f4) { Add(f1); Add(f2); Add(f3); Add(f4); }
	MultiImageFilter* Add(ImageFilter *theFilter);

	virtual RawImagePtr Filter(RawImage *theImage);
};
typedef SmartPtr<MultiImageFilter> MultiImageFilterPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class CropImageFilter : public ImageFilter
{
protected:
	int mx,my,mWidth,mHeight;

public:
	CropImageFilter(int x = 0, int y = 0, int width = 0, int height = 0) : mx(x), my(y), mWidth(width), mHeight(height) { }
	void SetPos(int x, int y) { mx = x; my = y; }
	void SetSize(int width, int height) { mWidth = width; mHeight = height; }

	virtual RawImagePtr Filter(RawImage *theImage);

	int GetX() { return mx; }
	int GetY() { return my; }
	int GetWidth() { return mWidth; }
	int GetHeight() { return mHeight; }
};
typedef SmartPtr<CropImageFilter> CropImageFilterPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TrimImageFilter : public CropImageFilter
{
public:
	TrimImageFilter() {}

	virtual RawImagePtr Filter(RawImage *theImage);
};
typedef SmartPtr<TrimImageFilter> TrimImageFilterPtr;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ScaleImageFilter : public ImageFilter
{
protected:
	int mWidth, mHeight;
	int mPercentWidth, mPercentHeight;
	bool mIsPercent;

	bool CheckSize(Image *theOldImage);

public:
	ScaleImageFilter(int thePercent = 100)  { SetPercentSize(thePercent, thePercent); }
	ScaleImageFilter(int theWidth, int theHeight) { SetAbsSize(theWidth, theHeight); }
	void SetAbsSize(int theWidth, int theHeight); 
	void SetPercentSize(int thePercentWidth, int thePercentHeight);

	virtual RawImagePtr Filter(RawImage *theImage); 
};
typedef SmartPtr<ScaleImageFilter> ScaleImageFilterPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TintImageFilter : public ImageFilter
{
protected:
	int mRed, mGreen, mBlue;

public:
	TintImageFilter(int theRedPercent, int theGreenPercent, int theBluePercent) : mRed(theRedPercent), mGreen(theGreenPercent), mBlue(theBluePercent) { }
	
	virtual RawImagePtr Filter(RawImage *theImage); 
};
typedef SmartPtr<TintImageFilter> TintImageFilterPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class TransparentImageFilter : public ImageFilter
{
public:
	enum PixelSample
	{
		UpperLeft,
		UpperRight,
		LowerLeft,
		LowerRight
	};

protected:
	DWORD mTransparentColor;
	PixelSample mPixelSample;
	bool mUseSample;

public:
	TransparentImageFilter(PixelSample theSample) { SetPixelSample(theSample); }
	TransparentImageFilter(DWORD theTransparentColor) { SetTransparentColor(theTransparentColor); }

	void SetPixelSample(PixelSample theSample) { mUseSample = true; mPixelSample = theSample; }
	void SetTransparentColor(DWORD theColor) { mUseSample = false; mTransparentColor = theColor; }

	virtual RawImagePtr Filter(RawImage *theImage); 
};
typedef SmartPtr<TransparentImageFilter> TransparentImageFilterPtr;




}; // namespace WONAPI

#endif