#ifndef __WON_STRETCHIMAGE_H__
#define __WON_STRETCHIMAGE_H__

#include "NativeImage.h"
#include "RawImage.h"
#include "ImageFilter.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class StretchImageFilter : public ImageFilter
{
public: 
	int mWidth, mHeight;
	int mLeftWidth, mRightWidth, mTopHeight, mBottomHeight;
	bool mTileHorz, mTileVert; 
	bool mSkipCenter;

public:
	StretchImageFilter();
	void SetSize(int theWidth, int theHeight) { mWidth = theWidth; mHeight = theHeight; }
	void SetBorders(int left, int top, int right, int bottom) { mLeftWidth = left; mRightWidth = right; mTopHeight = top; mBottomHeight = bottom; }
	void SetTile(bool tileHorz, bool tileVert) { mTileHorz = tileHorz; mTileVert = tileVert; }
	void SetSkipCenter(bool skip) { mSkipCenter = skip; }

	virtual RawImagePtr Filter(RawImage *theImage); 
};
typedef SmartPtr<StretchImageFilter> StretchImageFilterPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
typedef SmartPtr<ScaleImageFilter> ScaleImageFilterPtr;
class StretchImage : public ProgramImage
{
private:
	RawImagePtr mImage;
	NativeImagePtr mCachedImage;
	bool mNeedRecalc;
	int mLeftWidth, mRightWidth, mTopHeight, mBottomHeight;
	bool mTileHorz, mTileVert;
	bool mSkipCenter;

	void TileHorz(DisplayContext *theContext, int x1, int x2, int y, int imageLeft, int imageTop, int imageWidth, int imageHeight);
	void TileVert(DisplayContext *theContext, int y1, int y2, int x, int imageLeft, int imageTop, int imageWidth, int imageHeight);
	void TileHorzVert(DisplayContext *theContext, int x1, int x2, int y1, int y2, int imageLeft, int imageTop, int imageWidth, int imageHeight);

	void CalcImage(DisplayContext *theContext);

protected:
	virtual ~StretchImage();

public:
	StretchImage(RawImage *theImage, int leftWidth, int topHeight, int rightWidth, int bottomHeight); 
	virtual void Paint(Graphics &g);
	virtual ImagePtr Duplicate();
	virtual NativeImagePtr GetNative(DisplayContext *theContext);

	void CopyAttributes(StretchImage *from);
	void SetImage(RawImage *theImage);
	void SetBorders(int left, int top, int right, int bottom);
	void SetTile(bool tileHorz, bool tileVert) { mTileHorz = tileHorz; mTileVert = tileVert; }
	void SetSkipCenter(bool skip) { mSkipCenter = skip; }
	virtual void SetSize(int theWidth, int theHeight);

	int GetLeft() { return mLeftWidth; }
	int GetRight() { return mRightWidth; }
	int GetTop() { return mTopHeight; }
	int GetBottom() { return mBottomHeight; }
};
typedef SmartPtr<StretchImage> StretchImagePtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class StretchCenterImage : public ProgramImage
{
protected:
	ImagePtr mStretchImage;
	ImagePtr mCenterImage;
	
public:
	StretchCenterImage(Image *theStretchImage = NULL, Image *theCenterImage = NULL) : mStretchImage(theStretchImage), mCenterImage(theCenterImage) { }
	void SetStretchImage(Image *theStretchImage) { mStretchImage = theStretchImage; }
	void SetCenterImage(Image *theCenterImage) { mCenterImage = theCenterImage; }

	virtual void Paint(Graphics &g);

	virtual ImagePtr Duplicate();
	virtual void SetSize(int theWidth, int theHeight);
};
typedef SmartPtr<StretchImage> StretchImagePtr;

}; // namespace WONAPI

#endif
