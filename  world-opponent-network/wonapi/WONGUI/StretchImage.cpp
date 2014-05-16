#include "StretchImage.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
StretchImageFilter::StretchImageFilter()
{
	mWidth = mHeight = 0;
	mLeftWidth =  mRightWidth = mTopHeight = mBottomHeight = 0;
	mTileHorz = mTileVert = true; 
	mSkipCenter = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <class ImageType, class PixelType> 
static void CopyRawImage(ImageType *src, ImageType *dst, int destx, int desty, int x, int y, int width, int height)
{
	for(int i = 0; i<height; i++)
	{
		PixelType *aFromPixel = src->StartRowTraversal(y+i) + x;
		PixelType *aToPixel = dst->StartRowTraversal(desty+i) + destx;
		memcpy(aToPixel,aFromPixel,sizeof(PixelType)*width);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class ImageType, class PixelType> 
static void RawTileHorz(ImageType *src, ImageType *dst, int x1, int x2, int y, int tileLeft, int tileTop, int tileWidth, int tileHeight)
{
	int endX = x2 - tileWidth;
	int x;
	for(x = x1; x<endX; x+=tileWidth)
		CopyRawImage<ImageType,PixelType>(src,dst,x,y,tileLeft,tileTop,tileWidth,tileHeight);

	int finalWidth = x2 - x;
	if(finalWidth>0)
		CopyRawImage<ImageType,PixelType>(src,dst,x,y,tileLeft,tileTop,finalWidth,tileHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class ImageType, class PixelType> 
static void RawTileVert(ImageType *src, ImageType *dst, int y1, int y2, int x, int tileLeft, int tileTop, int tileWidth, int tileHeight)
{
	int endY = y2 - tileHeight;
	int y;
	for(y = y1; y<endY; y+=tileHeight)
		CopyRawImage<ImageType,PixelType>(src,dst,x,y,tileLeft,tileTop,tileWidth,tileHeight);

	int finalHeight = y2 - y;
	if(finalHeight>0)
		CopyRawImage<ImageType,PixelType>(src,dst,x,y,tileLeft,tileTop,tileWidth,finalHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class ImageType, class PixelType> 
static void RawTileHorzVert(ImageType *src, ImageType *dst, int x1, int x2, int y1, int y2, int tileLeft, int tileTop, int tileWidth, int tileHeight)
{
	int endY = y2 - tileHeight;
	int y;
	for(y = y1; y<endY; y+=tileHeight)
		RawTileHorz<ImageType,PixelType>(src,dst,x1,x2,y,tileLeft,tileTop,tileWidth,tileHeight);

	int finalHeight = y2 - y;
	if(finalHeight>0)
		RawTileHorz<ImageType,PixelType>(src,dst,x1,x2,y,tileLeft,tileTop,tileWidth,finalHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class ImageType, class PixelType> 
static void RawStretch(ImageType *src, ImageType *dst, int x1, int x2, int y1, int y2, int tileLeft, int tileTop, int tileWidth, int tileHeight)
{
	int theNewWidth = x2 - x1;
	int theNewHeight = y2 - y1;
	if(theNewWidth<=0 || theNewHeight<=0)
		return;


	int cx = 0, cy = 0, oy = tileTop;
	for(int y=0; y<theNewHeight; y++)
	{
		src->StartRowTraversal(oy);
		src->NextRowPixel(tileLeft);
		dst->StartRowTraversal(y+y1);
		dst->NextRowPixel(x1);

		PixelType curVal = src->GetRowPixel();

		int cx = 0;
		for(int x=0; x<theNewWidth; x++)
		{
			dst->SetRowPixel(curVal);
			dst->NextRowPixel();
			cx += tileWidth;
			if(cx >= theNewWidth)
			{
				src->NextRowPixel(cx/theNewWidth);
				cx%=theNewWidth;
				curVal = src->GetRowPixel();
			}
		}

		cy += tileHeight;
		if(cy >= theNewHeight)
		{
			oy += cy/theNewHeight;
			cy%=theNewHeight;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template <class ImageType, class PixelType> 
static SmartPtr<ImageType> DoFilter(ImageType *theImage, StretchImageFilter *theFilter)
{
	int mWidth = theFilter->mWidth, mHeight = theFilter->mHeight;
	int mLeftWidth = theFilter->mLeftWidth, mRightWidth = theFilter->mRightWidth, mTopHeight = theFilter->mTopHeight, mBottomHeight = theFilter->mBottomHeight;
	bool mTileHorz = theFilter->mTileHorz, mTileVert = theFilter->mTileVert; 

	if(mTopHeight + mBottomHeight > mHeight)
		mHeight = mTopHeight + mBottomHeight;

	if(mLeftWidth + mRightWidth > mWidth)
		mWidth = mLeftWidth + mRightWidth;

	ImageType *src = theImage;
	SmartPtr<ImageType> newImage;// = new ImageType(mWidth,mHeight);
	ImageFilter_NewRawImage(newImage,mWidth,mHeight);
	ImageType *dst = newImage;
	dst->CopyInfo(src);


	int x = 0, y = 0, left = 0, top = 0, width = mWidth, height = mHeight;

	// Simplification: Assume left and top are 0, and width and height are mWidth and mHeight
	int imageLeft = 0;
	int imageTop = 0;
	int imageRight = src->GetWidth()-mRightWidth;
	int imageBottom = src->GetHeight()-mBottomHeight;
	
	left = x;
	top = y;
	int right = x + mWidth - mRightWidth;
	int bottom = y + mHeight - mBottomHeight;

	int centerWidth = mWidth - mLeftWidth - mRightWidth;
	int centerHeight = mHeight - mTopHeight - mBottomHeight;

	int imageCenterWidth = src->GetWidth() - mLeftWidth - mRightWidth;
	int imageCenterHeight = src->GetHeight() - mTopHeight - mBottomHeight;

	int minCenterWidth = centerWidth<imageCenterWidth?centerWidth:imageCenterWidth; 
	int minCenterHeight = centerHeight<imageCenterHeight?centerHeight:imageCenterHeight; 

	// 9 pieces

	// Center
	if(!theFilter->mSkipCenter && imageCenterHeight>0 && imageCenterWidth>0)
	{
		if(!mTileHorz && !mTileVert) // just stretch
			RawStretch<ImageType,PixelType>(src,dst,left+mLeftWidth,right,top+mTopHeight,bottom,imageLeft+mLeftWidth,imageTop+mTopHeight,imageCenterWidth,imageCenterHeight);
		else if(mTileHorz && mTileVert) // just tile
			RawTileHorzVert<ImageType,PixelType>(src,dst,left+mLeftWidth,right,top+mTopHeight,bottom,imageLeft+mLeftWidth,imageTop+mTopHeight,imageCenterWidth,imageCenterHeight);
		else if(mTileVert) // stretch horz, tile vert 
		{
			RawStretch<ImageType,PixelType>(src,dst,left+mLeftWidth,right,top+mTopHeight,top+mTopHeight+minCenterHeight,imageLeft+mLeftWidth,imageTop+mTopHeight,imageCenterWidth,imageCenterHeight);
			RawTileVert<ImageType,PixelType>(dst,dst,top+mTopHeight+imageCenterHeight,bottom,left+mLeftWidth,left+mLeftWidth,top+mTopHeight,centerWidth,imageCenterHeight);
		}
		else if(mTileHorz) // stretch vert, tile horz
		{
			RawStretch<ImageType,PixelType>(src,dst,left+mLeftWidth,left+mLeftWidth+minCenterWidth,top+mTopHeight,bottom,imageLeft+mLeftWidth,imageTop+mTopHeight,imageCenterWidth,imageCenterHeight);
			RawTileHorz<ImageType,PixelType>(dst,dst,left+mLeftWidth+imageCenterWidth,right,top+mTopHeight,left+mLeftWidth,top+mTopHeight,imageCenterWidth,centerHeight);
		}
	}


	// Sides
	if(mLeftWidth>0 && imageCenterHeight>0) // left side
	{
		if(mTileVert)
			RawTileVert<ImageType,PixelType>(src,dst,top+mTopHeight,bottom,left,imageLeft,imageTop+mTopHeight,mLeftWidth,imageCenterHeight);
		else
			RawStretch<ImageType,PixelType>(src,dst,left,left+mLeftWidth,top+mTopHeight,bottom,imageLeft,imageTop+mTopHeight,mLeftWidth,imageCenterHeight);
	}

	if(mRightWidth>0 && imageCenterHeight>0) // right side
	{
		if(mTileVert)
			RawTileVert<ImageType,PixelType>(src,dst,top+mTopHeight,bottom,right,imageRight,imageTop+mTopHeight,mRightWidth,imageCenterHeight);
		else
			RawStretch<ImageType,PixelType>(src,dst,right,right+mRightWidth,top+mTopHeight,bottom,imageRight,imageTop+mTopHeight,mRightWidth,imageCenterHeight);
	}


	if(mTopHeight>0 && imageCenterWidth>0) // top side
	{
		if(mTileHorz)
			RawTileHorz<ImageType,PixelType>(src,dst,left+mLeftWidth,right,top,imageLeft+mLeftWidth,imageTop,imageCenterWidth,mTopHeight);
		else
			RawStretch<ImageType,PixelType>(src,dst,left+mLeftWidth,right,top,top+mTopHeight,imageLeft+mLeftWidth,imageTop,imageCenterWidth,mTopHeight);
	}

	if(mBottomHeight>0 && imageCenterWidth>0) // bottom side
	{
		if(mTileHorz)
			RawTileHorz<ImageType,PixelType>(src,dst,left+mLeftWidth,right,bottom,imageLeft+mLeftWidth,imageBottom,imageCenterWidth,mBottomHeight);
		else
			RawStretch<ImageType,PixelType>(src,dst,left+mLeftWidth,right,bottom,bottom+mBottomHeight,imageLeft+mLeftWidth,imageBottom,imageCenterWidth,mBottomHeight);
	}

	// Corners
	if(mLeftWidth>0 && mTopHeight>0)
		CopyRawImage<ImageType,PixelType>(src,dst,left,top,imageLeft,imageTop,mLeftWidth,mTopHeight); // top-left

	if(mRightWidth>0 && mTopHeight>0)
		CopyRawImage<ImageType,PixelType>(src,dst,right,top,imageRight,imageTop,mRightWidth,mTopHeight); // top-right

	if(mLeftWidth>0 && mBottomHeight>0)
		CopyRawImage<ImageType,PixelType>(src,dst,left,bottom,imageLeft,imageBottom,mLeftWidth,mBottomHeight); // bottom-left

	if(mRightWidth>0 && mBottomHeight>0)
		CopyRawImage<ImageType,PixelType>(src,dst,right,bottom,imageRight,imageBottom,mRightWidth,mBottomHeight); // bottom-right


	return newImage;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RawImagePtr StretchImageFilter::Filter(RawImage *theImage)
{

	switch(theImage->GetType())
	{
		case RawImageType_8: return DoFilter<RawImage8,RawImage8::PixelType>((RawImage8*)theImage,this).get(); 
		case RawImageType_32: return DoFilter<RawImage32,RawImage32::PixelType>((RawImage32*)theImage,this).get(); 
		default: return NULL;
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
StretchImage::StretchImage(RawImage *theImage, int leftWidth, int topHeight, int rightWidth, int bottomHeight)
{
	mImage = theImage;
	mWidth = mHeight = 0;
	mLeftWidth = leftWidth;
	mRightWidth = rightWidth;
	mTopHeight = topHeight;
	mBottomHeight = bottomHeight;
	mNeedRecalc = true;
	mTileHorz = mTileVert = true;
	mSkipCenter = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
StretchImage::~StretchImage()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StretchImage::SetSize(int theWidth, int theHeight)
{
	if(theWidth==mWidth && theHeight==mHeight)
		return;

	mWidth = theWidth;
	mHeight = theHeight;
	mNeedRecalc = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/*
void StretchImage::TileHorz(DisplayContext *theContext, int x1, int x2, int y, int imageLeft, int imageTop, int imageWidth, int imageHeight)
{
	int endX = x2 - imageWidth;
	for(int x = x1; x<endX; x+=imageWidth)
		mImage->Draw(theContext,x,y,imageLeft,imageTop,imageWidth,imageHeight);

	int finalWidth = x2 - x;
	if(finalWidth>0)
		mImage->Draw(theContext,x,y,imageLeft,imageTop,finalWidth,imageHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StretchImage::TileVert(DisplayContext *theContext, int y1, int y2, int x, int imageLeft, int imageTop, int imageWidth, int imageHeight)
{
	int endY = y2 - imageHeight;
	for(int y = y1; y<endY; y+=imageHeight)
		mImage->Draw(theContext,x,y,imageLeft,imageTop,imageWidth,imageHeight);

	int finalHeight = y2 - y;
	if(finalHeight>0)
		mImage->Draw(theContext,x,y,imageLeft,imageTop,imageWidth,finalHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StretchImage::TileHorzVert(DisplayContext *theContext, int x1, int x2, int y1, int y2, int imageLeft, int imageTop, int imageWidth, int imageHeight)
{
	int endY = y2 - imageHeight;
	for(int y = y1; y<endY; y+=imageHeight)
		TileHorz(theContext,x1,x2,y,imageLeft,imageTop,imageWidth,imageHeight);

	int finalHeight = y2 - y;
	if(finalHeight>0)
		TileHorz(theContext,x1,x2,y,imageLeft,imageTop,imageWidth,finalHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StretchImage::CalcImage(DisplayContext *context)
{
	mCachedImage = context->CreateImage(mWidth,mHeight);
	DisplayContext *theContext = mCachedImage->GetDisplayContext();

	int x = 0, y = 0, left = 0, top = 0, width = mWidth, height = mHeight;


	// Simplification: Assume left and top are 0, and width and height are mWidth and mHeight
	int imageLeft = 0;
	int imageTop = 0;
	int imageRight = mImage->GetWidth()-mRightWidth;
	int imageBottom = mImage->GetHeight()-mBottomHeight;
	
	left = x;
	top = y;
	int right = x + mWidth - mRightWidth;
	int bottom = y + mHeight - mBottomHeight;

	int centerWidth = mWidth - mLeftWidth - mRightWidth;
	int centerHeight = mHeight - mTopHeight - mBottomHeight;

	int imageCenterWidth = mImage->GetWidth() - mLeftWidth - mRightWidth;
	int imageCenterHeight = mImage->GetHeight() - mTopHeight - mBottomHeight;

	// 9 pieces

	// Center
	if(imageCenterHeight>0 && imageCenterWidth>0)
		TileHorzVert(theContext,left+mLeftWidth,right,top+mTopHeight,bottom,imageLeft+mLeftWidth,imageTop+mTopHeight,imageCenterWidth,imageCenterHeight);

	// Sides
	if(mLeftWidth>0 && imageCenterHeight>0) // left side
		TileVert(theContext,top+mTopHeight,bottom,left,imageLeft,imageTop+mTopHeight,mLeftWidth,imageCenterHeight);

	if(mRightWidth>0 && imageCenterHeight>0) // right side
		TileVert(theContext,top+mTopHeight,bottom,right,imageRight,imageTop+mTopHeight,mRightWidth,imageCenterHeight);

	if(mTopHeight>0 && imageCenterWidth>0) // top side
		TileHorz(theContext,left+mLeftWidth,right,top,imageLeft+mLeftWidth,imageTop,imageCenterWidth,mTopHeight);

	if(mBottomHeight>0 && imageCenterWidth>0) // bottom side
		TileHorz(theContext,left+mLeftWidth,right,bottom,imageLeft+mLeftWidth,imageBottom,imageCenterWidth,mBottomHeight);

	// Corners
	if(mLeftWidth>0 && mTopHeight>0)
		mImage->Draw(theContext,left,top,imageLeft,imageTop,mLeftWidth,mTopHeight); // top-left

	if(mRightWidth>0 && mTopHeight>0)
		mImage->Draw(theContext,right,top,imageRight,imageTop,mRightWidth,mTopHeight); // top-right

	if(mLeftWidth>0 && mBottomHeight>0)
		mImage->Draw(theContext,left,bottom,imageLeft,imageBottom,mLeftWidth,mBottomHeight); // bottom-left

	if(mRightWidth>0 && mBottomHeight>0)
		mImage->Draw(theContext,right,bottom,imageRight,imageBottom,mRightWidth,mBottomHeight); // bottom-right
}
*/

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StretchImage::CalcImage(DisplayContext *context)
{
	StretchImageFilterPtr aFilter = new StretchImageFilter;
	aFilter->SetBorders(mLeftWidth,mTopHeight,mRightWidth,mBottomHeight);
	aFilter->SetSize(GetWidth(),GetHeight());
	aFilter->SetTile(mTileHorz,mTileVert);
	aFilter->SetSkipCenter(mSkipCenter);
	mCachedImage = aFilter->Filter(mImage)->GetNative(context);
	mNeedRecalc = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StretchImage::Paint(Graphics &g)
{
	if(mNeedRecalc)
		CalcImage(g.GetDisplayContext());

	if(mSkipCenter)
	{
		if(mLeftWidth>0)
		{
			g.PushClipRect(0,0,mLeftWidth,mHeight); // Left part
			g.DrawImage(mCachedImage,0,0);
			g.PopClipRect();
		}

		if(mRightWidth>0)
		{
			g.PushClipRect(mWidth-mRightWidth,0,mRightWidth,mHeight); // right part
			g.DrawImage(mCachedImage,0,0);
			g.PopClipRect();
		}

		if(mTopHeight>0)
		{	
			g.PushClipRect(0,0,mWidth,mTopHeight); // top part
			g.DrawImage(mCachedImage,0,0);
			g.PopClipRect();
		}

		if(mBottomHeight>0)
		{
			g.PushClipRect(0,mHeight-mBottomHeight,mWidth,mBottomHeight);
			g.DrawImage(mCachedImage,0,0);
			g.PopClipRect();
		}
	}
	else
		g.DrawImage(mCachedImage,0,0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StretchImage::SetImage(RawImage *theImage)
{
	mImage = theImage;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StretchImage::SetBorders(int left, int top, int right, int bottom)
{
	mLeftWidth = left;
	mTopHeight = top;
	mRightWidth = right;
	mBottomHeight = bottom;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StretchImage::CopyAttributes(StretchImage *from)
{
	mLeftWidth = from->mLeftWidth;
	mTopHeight = from->mTopHeight; 
	mRightWidth = from->mRightWidth;
	mBottomHeight = from->mBottomHeight;
	mTileHorz = from->mTileHorz;
	mTileVert = from->mTileVert;
	mSkipCenter = from->mSkipCenter;
	mWidth = from->GetWidth();
	mHeight = from->GetHeight();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ImagePtr StretchImage::Duplicate()
{
	StretchImagePtr aCopy = new StretchImage(mImage,mLeftWidth,mTopHeight,mRightWidth,mBottomHeight);
	aCopy->SetTile(mTileHorz,mTileVert);
	aCopy->SetSkipCenter(mSkipCenter);
	return aCopy.get();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeImagePtr StretchImage::GetNative(DisplayContext *theContext)
{
	if(mNeedRecalc)
		CalcImage(theContext);

	return mCachedImage;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StretchCenterImage::Paint(Graphics &g)
{
	g.DrawImage(mStretchImage,0,0);
	
	if(mCenterImage->GetWidth()<=mWidth && mCenterImage->GetHeight()<=mHeight)
		g.DrawImage(mCenterImage,(mWidth-mCenterImage->GetWidth())/2,(mHeight-mCenterImage->GetHeight())/2);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void StretchCenterImage::SetSize(int theWidth, int theHeight)
{
	mWidth = theWidth;
	mHeight = theHeight;
	mStretchImage->SetSize(theWidth,theHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ImagePtr StretchCenterImage::Duplicate()
{
	return new StretchCenterImage(mStretchImage->Duplicate(),mCenterImage);
}
