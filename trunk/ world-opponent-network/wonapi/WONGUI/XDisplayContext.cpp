#include "DisplayContext.h"


typedef std::map<DWORD,DWORD> ColorTable;
typedef std::list<std::pair<DWORD,DWORD> > ColorList;

static ColorTable mColorTable;
static ColorList mColorList;
static bool mStaticInit = false;
static bool mIsTrueColor = false;
static bool mIsGoodTrueColor = false;
static int mRedMask, mGreenMask, mBlueMask;
static int mRedShift, mGreenShift, mBlueShift;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONAPI::GCHolder::~GCHolder()
{
	XFreeGC(mDisplay, mGC);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::DisplayContext::CreateNewGC()
{
	mGC  = XCreateGC(mDisplay, mDrawable, 0, NULL);
	mGCHolder = new GCHolder(mDisplay,mGC);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static int GetHighestBit(int theVal)
{
	int aCount = 0;
	while(theVal > 0)
	{
		theVal>>=1;
		aCount++;
	}

	return aCount;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::DisplayContext::StaticInit(DisplayContext *theContext)
{
	if(mStaticInit)
		return;

	mStaticInit = true;
	Visual *v = theContext->mVisual;
	if(v->c_class==TrueColor)
	{
		mIsTrueColor = true;
		mRedMask = v->red_mask;
		mGreenMask = v->green_mask;
		mBlueMask = v->blue_mask;

		if(mRedMask==0xff0000 && mGreenMask==0xff00 && mBlueMask==0xff)
			mIsGoodTrueColor = true;
		else
		{
			mRedShift = GetHighestBit(mRedMask)-24;
			mGreenShift = GetHighestBit(mGreenMask)-16;
			mBlueShift = GetHighestBit(mBlueMask)-8;
		}

	}


	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static inline int GetColorVal(DWORD theVal, int theShift, int theMask)
{
	if(theShift<0)
		return (theVal>>(-theShift))&theMask;
	else
		return (theVal<<theShift)&theMask;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONAPI::DisplayContext::~DisplayContext()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
DWORD WONAPI::DisplayContext::AllocColor(DWORD theColor)
{
	if(mIsTrueColor)
	{
		if(mIsGoodTrueColor)
			return theColor;

		return GetColorVal(theColor,mRedShift,mRedMask) | 
				GetColorVal(theColor,mGreenShift,mGreenMask) | 
					GetColorVal(theColor,mBlueShift,mBlueMask);
	}

	std::pair<ColorTable::iterator,bool> aRet = mColorTable.insert(ColorTable::value_type(theColor,0));
	if(!aRet.second) // already in map
		return aRet.first->second;

//	printf("%d %d\n",mColorTable.size(),mColorList.size());


	int r1 = (theColor&0xff0000)>>16;
	int g1 = (theColor&0x00ff00)>>8;
	int b1 = (theColor&0x0000ff);

	ColorList::iterator anItr = mColorList.begin();
	int minDist = 0x7ffffff;
	int aPixel = mWhitePixel;
	while(anItr!=mColorList.end())
	{
		DWORD aColor = anItr->first;
		int r2 = (aColor&0xff0000)>>16;
		int g2 = (aColor&0x00ff00)>>8;
		int b2 = (aColor&0x0000ff);
		
		int aDist = (r2-r1)*(r2-r1) + (g2-g1)*(g2-g1) + (b2-b1)*(b2-b1);
		if(aDist<minDist)
		{
			minDist = aDist;
			aPixel = anItr->second;
		}

		++anItr;
	}

	if(minDist > 400 && mColorList.size()<200)
	{
		XColor aColor; 
		aColor.red = r1<<8;
		aColor.green = g1<<8;
		aColor.blue = b1<<8;
		if(XAllocColor(mDisplay, mColormap, &aColor)!=BadColor)
		{
			aPixel = aColor.pixel;
			mColorList.push_back(std::pair<DWORD,DWORD>(theColor,aPixel));
		}
	}

	aRet.first->second = aPixel;
	return aPixel;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONAPI::NativeImagePtr WONAPI::DisplayContext::CreateImage(int theWidth, int theHeight)
{
	return new WONAPI::XNativeImage(this,theWidth,theHeight);
}
