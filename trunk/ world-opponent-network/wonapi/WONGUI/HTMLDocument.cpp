#include "HTMLDocument.h"
#include "ImageDecoder.h"
#include "ImageFilter.h"
#include "Window.h"


using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTMLDocument::HTMLDocument(const GUIString &theBasePath)
{
	Clear();

	mDecodingImages = false;
	mDelayImages = true;
	mWindow = Window::GetDefaultWindow();

	if(!theBasePath.empty())
		SetBasePath(theBasePath);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTMLDocument::~HTMLDocument()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDocument::Clear()
{
	mBodyStart = mBodyEnd = mTagList.end();
	mImageTagMap.clear();
	mLoadedImageMap.clear();
	mScaledImageMap.clear();
	mImageTagList.clear();
	mTagList.clear();
	mListStack.clear();
	mCurImageMap = NULL;
	mImageMapMap.clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDocument::SetBasePath(const GUIString &theBasePath)
{
	int aPos = theBasePath.rFind('\\');
	if(aPos==-1)
		aPos = theBasePath.rFind('/');

	if(aPos==-1)
		mBasePath.erase();
	else
		mBasePath = theBasePath.substr(0,aPos+1);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLDocument::DoDecodeImage(LoadedImageStruct *theStruct, Window *theWindow, const std::string &theImagePath)
{
	if(theWindow==NULL)
		return false;

	MultiImageDecoder *aDecoder = theWindow->GetWindowManager()->GetImageDecoder();
	RawImagePtr aRawImage = aDecoder->DecodeRaw(theImagePath.c_str());
	if(aRawImage.get()==NULL)
		return false;

	NativeImagePtr anImage = aRawImage->GetNative(theWindow->GetDisplayContext());
	
	AnimationPtr anAnimation = aDecoder->GetAnimation(false);
	if(anAnimation.get()!=NULL)
		anAnimation->RealizeNative(mWindow->GetDisplayContext());

	theStruct->mImage = anImage;
	theStruct->mRawImage = aRawImage;
	theStruct->mAnimation = anAnimation;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTMLDocument::LoadedImageStruct* HTMLDocument::GetImage(const GUIString &thePath, HTMLTag *theTag)
{
	if(thePath.empty())
		return NULL;

	std::pair<LoadedImageMap::iterator,bool> aRet;
	aRet = mLoadedImageMap.insert(LoadedImageMap::value_type(thePath,LoadedImageStruct()));
	LoadedImageStruct &aStruct = aRet.first->second;

	if(aStruct.mImage.get()!=NULL)
		return &aStruct;

	if(mDelayImages && !mDecodingImages)
	{
		mImageTagMap.insert(ImageTagMap::value_type(thePath,theTag));
		return NULL;
	}

	if(!aRet.second) // already failed to decode
			return NULL;

	GUIString aPath = mBasePath;
	aPath.append(thePath);

	if(DoDecodeImage(&aStruct,mWindow,aPath))
		return &aStruct;
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeImage* HTMLDocument::CheckScaleImage(LoadedImageStruct *theStruct, const GUIString &thePath, int theWidth, int theHeight)
{
	NativeImage *anImage = theStruct->mImage;

	if(theWidth==0 && theHeight==0)
	{
		theWidth = anImage->GetWidth();
		theHeight = anImage->GetHeight();
	}
	else if(theWidth==0 && theHeight!=0 && anImage->GetHeight()!=0)
		theWidth = anImage->GetWidth()*theHeight/anImage->GetHeight();
	else if(theHeight==0 && theWidth!=0 && anImage->GetWidth()!=0)
		theHeight = anImage->GetHeight()*theWidth/anImage->GetWidth();

	if((theWidth!=anImage->GetWidth() || theHeight!=anImage->GetHeight()))
	{
		char aBuf[50];
		sprintf(aBuf,"_%d_%d",theWidth,theHeight);
		GUIString aScaleName = thePath;
		aScaleName.append(aBuf);
		
		LoadedImageMap::iterator anImageItr = mScaledImageMap.insert(LoadedImageMap::value_type(aScaleName,LoadedImageStruct())).first;
		LoadedImageStruct &aStruct = anImageItr->second;
		if(aStruct.mImage.get()==NULL)
		{
			if(theStruct->mRawImage.get()==NULL)
				theStruct->mRawImage = anImage->GetRaw();

			ScaleImageFilterPtr aFilter = new ScaleImageFilter(theWidth,theHeight);
			aStruct.mImage = aFilter->Filter(theStruct->mRawImage)->GetNative(mWindow->GetDisplayContext());
		}

		anImage = aStruct.mImage;
	}

	return anImage;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void ImagePaint(Graphics &g, int width, int height)
{
	g.SetColor(0x848484);
	g.DrawRect(0,0,width,height);
	g.SetColor(0xd6d6d6);
	g.DrawLine(width-1,0,width-1,height-1);
	g.DrawLine(0,height-1,width-1,height-1);

	int w = 14, h = 16;
	int ox = 7, oy = 7;

	bool clipped = false;
	if(width<w+ox || height<h+oy)
	{
		if(width<8 || height<8)
			return;

		g.PushClipRect(0,0,width-5,height-5);
		clipped = true;
	}

	g.Translate(ox,oy);
	
	g.SetColor(0xffffff);
	g.FillRect(0,0,w,h);
	g.SetColor(0x848484);
	g.DrawLine(0,0,w-1,0);
	g.DrawLine(0,0,0,h-2);
	g.SetColor(0x000000);
	g.DrawLine(0,h-1,w-1,h-1);
	g.DrawLine(w-1,1,w-1,h-1);
	g.SetColor(0xa0a0a0);
	g.DrawLine(w-2,1,w-2,h-2);
	g.DrawLine(1,h-2,w-2,h-2);

	g.SetColor(0xff0000);


	int offset = 0;
	int hmid = w/2;
	int vmid = h/2;
	int left = hmid-4+offset;
	int right = hmid+3+offset;
	int top = vmid-4+offset;
	int bottom = vmid+3+offset;
	for(int i=0; i<4; i++)
	{
		g.DrawLine(left+i,top+i,left+i+1,top+i);
		g.DrawLine(right-i,top+i,right-1-i,top+i);
		g.DrawLine(left+i,bottom-i,left+i+1,bottom-i);
		g.DrawLine(right-i,bottom-i,right-i-1,bottom-i);
	}

	g.Translate(-ox,-oy);

	if(clipped)
		g.PopClipRect();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDocument::HandleImage(HTMLImageTag *theTag)
{
	LoadedImageStruct *aStruct = GetImage(theTag->mSrc,theTag);
	if(aStruct==NULL)
	{		
		if(mDelayImages && theTag->mWidth>0 && theTag->mHeight>0)
			theTag->mImage = new ProgramImage(theTag->mWidth, theTag->mHeight,ImagePaint);

		return;
	}


	if(aStruct->mAnimation.get()!=NULL)
	{
		if(aStruct->mHaveUsedAnimation)
			theTag->mAnimation = aStruct->mAnimation->Duplicate();
		else
		{
			theTag->mAnimation = aStruct->mAnimation;
			aStruct->mHaveUsedAnimation = true;
		}

		return;
	}
	
	NativeImage *anImage = CheckScaleImage(aStruct,theTag->mSrc,theTag->mWidth,theTag->mHeight);
	theTag->mImage = anImage;	

	if(!theTag->mUseMap.empty())
	{
		ImageMapMap::iterator anItr = mImageMapMap.insert(ImageMapMap::value_type(theTag->mUseMap,NULL)).first;
		if(anItr->second==NULL)
			anItr->second = new ImageMap;

		theTag->mImageMap = anItr->second;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDocument::HandleInput(HTMLInputTag *theTag)
{
	if(theTag->IsEnd())
		return;

	if(theTag->mImageSrc.empty())
		return;

	LoadedImageStruct *aStruct = GetImage(theTag->mImageSrc,theTag);
	if(aStruct==NULL)
		return;

	theTag->mImage = aStruct->mImage;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDocument::HandleImageMap(HTMLImageMapTag* theTag)
{
	if(theTag->IsEnd())
	{
		mCurImageMap = NULL;
		return;
	}

	if(theTag->mName.empty())
		return;

	ImageMapMap::iterator anItr = mImageMapMap.insert(ImageMapMap::value_type(theTag->mName,NULL)).first;
	if(anItr->second.get()==NULL)
		anItr->second = new ImageMap;

	mCurImageMap = anItr->second;				
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDocument::HandleArea(HTMLAreaTag *theTag)
{
	if(theTag->IsEnd())
		return;

	if(mCurImageMap.get()==NULL)
		return;

	if(!theTag->mIsValid)
		return;

	WONRectangle aRect(theTag->mLeft,theTag->mTop,theTag->mRight-theTag->mLeft,theTag->mBottom-theTag->mTop);
	mCurImageMap->AddLink(new TextLinkParam(theTag->mHRef, theTag->mLaunchBrowser), aRect);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDocument::HandleBody(HTMLBodyTag *theTag)
{
	if(!mDecodingImages)
	{
		if(theTag->IsEnd())
		{
			if(mBodyEnd!=mTagList.end())
			{
				mTagList.erase(mBodyEnd);
				mBodyEnd = mTagList.end();
			}

			--mBodyEnd; // point to last added tag

			return;
		}

		if(mBodyStart==mTagList.end())
			--mBodyStart; // point to last added tag
		else // get rid of this body tag because we already have one.
		{
			if(mBodyStart->get()!=theTag)
				mTagList.pop_back();

			return;
		}
	}

	LoadedImageStruct *aStruct = GetImage(theTag->mBGImageSrc,theTag);
	if(aStruct==NULL)
		return;

	NativeImage *anImage = aStruct->mImage;

	// Pre-Tile the image a little if it's too small
	int aWidth = anImage->GetWidth();
	int aHeight = anImage->GetHeight();
	int anExtraWidth = 0;
	int anExtraHeight = 0;
	if(aWidth < 50)
	{
		if(50%aWidth==0)
			anExtraWidth = 50-aWidth;
		else
			anExtraWidth = (50/aWidth)*aWidth;
	}
	if(aHeight < 50)
	{
		if(50%aHeight==0)
			anExtraHeight = 50-aHeight;
		else
			anExtraHeight = (50/aHeight)*aHeight;
	}

	if(anExtraWidth>0 || anExtraHeight>0)
	{
		aWidth+=anExtraWidth;
		aHeight+=anExtraHeight;
		NativeImagePtr aBigImage = mWindow->CreateImage(aWidth,aHeight);
		Graphics &g = aBigImage->GetGraphics();
		for(int x=0; x<aWidth; x+=anImage->GetWidth())
			for(int y=0; y<aHeight; y+=anImage->GetHeight())
				g.DrawImage(anImage,x,y);

		theTag->mBGImage = aBigImage;
	}
	else
		theTag->mBGImage = anImage;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void GetLetterItem(int theCount, char *theBuf, bool upper)
{
	int aPos = 0;
	while(theCount>0)
	{
		theCount--;
		theBuf[aPos++] = (upper?'A':'a') + theCount%26;
		theCount/=26;
	} 


	theBuf[aPos] = '.';
	theBuf[aPos+1] = 0;

	// Reverse String
	int i = 0;
	aPos--;
	while(i<aPos)
	{
		char aTemp = theBuf[aPos]; theBuf[aPos] = theBuf[i]; theBuf[i] = aTemp;
		i++; aPos--;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void GetRomanNumeralHelp(char *theBuf, int &thePos, int &theNumber, int hiVal, char hiChar, char midChar, char lowChar)
{
	int lowVal = hiVal/10;
	int midVal = hiVal>>1;

	while(theNumber>=hiVal)
	{
		theBuf[thePos++] = hiChar;
		theNumber-=hiVal;
	}

	if(theNumber>=hiVal-lowVal)
	{
		theBuf[thePos++] = lowChar;
		theBuf[thePos++] = hiChar;
		theNumber-=hiVal-lowVal;
	}

	if(theNumber>=midVal)
	{
		theBuf[thePos++] = midChar;
		theNumber-=midVal;
	}

	if(theNumber>=midVal-lowVal)
	{
		theBuf[thePos++] = lowChar;
		theBuf[thePos++] = midChar;
		theNumber-=midVal-lowVal;
	}

	while(theNumber>=lowVal)
	{
		theBuf[thePos++] = lowChar;
		theNumber-=lowVal;
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void GetRomanNumeralItem(int theCount, char *theBuf, bool upper)
{
	static char lower_letters[] = { 'm', 'd', 'c', 'l', 'x', 'v', 'i' };
	static char upper_letters[] = { 'M', 'D', 'C', 'L', 'X', 'V', 'I' };
	const char* letters = upper?upper_letters:lower_letters;

	if(theCount>=4000)
	{
		sprintf(theBuf,"%d.",theCount);
		return;
	}

	int aPos = 0;
	int aHiVal = 1000;
	int aLetterPos = 0;
	for(int i=0; i<3; i++)
	{
		GetRomanNumeralHelp(theBuf,aPos,theCount,aHiVal,letters[aLetterPos],letters[aLetterPos+1],letters[aLetterPos+2]);
		aHiVal/=10;
		aLetterPos+=2;
	}
	theBuf[aPos++] = '.';
	theBuf[aPos] = '\0';
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDocument::HandleList(HTMLListTag *theTag)
{	
	if(theTag->IsEnd())
	{
		if(!mListStack.empty())
			mListStack.pop_back();

		return;
	}

	mListStack.push_back(ListInfo());
	ListInfo &anInfo = mListStack.back();
	if(theTag->GetType()==HTMLTagType_UnorderedList)
	{
		anInfo.mOrdered = false;
		if(theTag->mListType==HTMLListType_None)
		{
			anInfo.mType = (HTMLListType)(HTMLListType_Disc + mListStack.size() - 1);
/*			int i = mListStack.size()-2;
			while(i>=0)
			{
				ListInfo &anotherInfo = mListStack[i];
				if(!anotherInfo.mOrdered)
				{
					anInfo.mType = (HTMLListType)(anotherInfo.mType + 1);
					break;
				}
				i--;
			}*/
		}
		else
			anInfo.mType = theTag->mListType;

		if(anInfo.mType > HTMLListType_Square)
			anInfo.mType = HTMLListType_Square;
		else if(anInfo.mType < HTMLListType_Disc)
			anInfo.mType = HTMLListType_Disc;
	}
	else
	{
		anInfo.mOrdered = true;
		anInfo.mType = theTag->mListType;
	}

	anInfo.mCount = theTag->mStart;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDocument::HandleListItem(HTMLListItemTag *theTag)
{
	if(theTag->IsEnd())
		return;

	if(mListStack.empty())
	{
		theTag->mListType = HTMLListType_Disc;
		return;
	}

	ListInfo &anInfo = mListStack.back();
	theTag->mListType = anInfo.mType;

	if(anInfo.mOrdered)
	{
		char aBuf[100];
		aBuf[0] = 0;
		switch(anInfo.mType)
		{
			case HTMLListType_LowerLetter: GetLetterItem(anInfo.mCount,aBuf,false); break;
			case HTMLListType_UpperLetter: GetLetterItem(anInfo.mCount,aBuf,true); break;
			case HTMLListType_LowerRoman: GetRomanNumeralItem(anInfo.mCount,aBuf,false); break;
			case HTMLListType_UpperRoman: GetRomanNumeralItem(anInfo.mCount,aBuf,true); break;

			case HTMLListType_Digit: 
			default:
				sprintf(aBuf,"%d.",anInfo.mCount); break;
		}
		theTag->mBulletText = aBuf;
	}

	anInfo.mCount++;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDocument::Rewind(bool beginAtBodyStart)
{
	if(beginAtBodyStart)
		mTagItr = GetBodyStart();
	else
		mTagItr = mTagList.begin();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTMLTag* HTMLDocument::GetNextTag(bool stopAtBodyEnd)
{
	if(stopAtBodyEnd)
	{
		if(mTagItr==mBodyEnd)
			return NULL;
	}
	else 
	{
		if(mTagItr==mTagList.end())
			return NULL;
	}

	return *mTagItr++;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDocument::AddDocument(HTMLDocument *theDocument)
{
	if(theDocument==NULL)
		return;

	HTMLTagList::iterator start = theDocument->GetBodyStart();
	HTMLTagList::iterator end = theDocument->GetBodyEnd();
	for(HTMLTagList::iterator anItr = start; anItr!=end; ++anItr)
		AddTag(*anItr);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDocument::AddTag(HTMLTag *theTag)
{
	mTagList.push_back(theTag);
	switch(theTag->GetType())
	{
		case HTMLTagType_Body: HandleBody((HTMLBodyTag*)theTag); break;

		case HTMLTagType_Image: HandleImage((HTMLImageTag*)theTag); break;
		case HTMLTagType_Input: HandleInput((HTMLInputTag*)theTag); break;

		case HTMLTagType_ImageMap: HandleImageMap((HTMLImageMapTag*)theTag); break;
		case HTMLTagType_Area: HandleArea((HTMLAreaTag*)theTag); break;

		case HTMLTagType_OrderedList: 
		case HTMLTagType_UnorderedList: HandleList((HTMLListTag*)theTag); break;
		case HTMLTagType_ListItem: HandleListItem((HTMLListItemTag*)theTag); break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDocument::DecodeImages(bool continueDelay)
{
	if(!continueDelay)
		mDelayImages = false;

	mDecodingImages = true;
	ImageTagMap::iterator anItr = mImageTagMap.begin();

	while(anItr!=mImageTagMap.end())
	{
		HTMLTag *aTag = anItr->second;
		switch(aTag->GetType())
		{
			case HTMLTagType_Body: HandleBody((HTMLBodyTag*)aTag); break;
			case HTMLTagType_Image: HandleImage((HTMLImageTag*)aTag); break;
			case HTMLTagType_Input: HandleInput((HTMLInputTag*)aTag); break;
		}

		++anItr;

	}
	mDecodingImages = false;

	if(!continueDelay)
	{
		mImageTagMap.clear();
		mLoadedImageMap.clear();
		mScaledImageMap.clear();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTMLTagList::iterator HTMLDocument::GetBodyStart()
{
	if(mBodyStart!=mTagList.end())
		return mBodyStart;
	else
		return mTagList.begin();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTMLTagList::iterator HTMLDocument::GetBodyEnd()
{
	HTMLTagList::iterator anItr = mBodyEnd;
	if(anItr!=mTagList.end())
		++anItr;

	return anItr;
}

