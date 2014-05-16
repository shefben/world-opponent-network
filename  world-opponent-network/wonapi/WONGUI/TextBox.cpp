#include "TextBox.h"
#include "Clipboard.h"
#include "ColorScheme.h"
#include <algorithm>

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TextSegmentBase::TextSegmentBase()
{
	mLine = NULL;
	mAbsStartChar = mAbsEndChar = 0;
	mWidth = mHeight = mAscent = 0;
	mx = my = 0;
	mLineBreak = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextSegmentBase::Wrap(TextArea *theTextArea, TextLine *theLine)
{
	if(!theLine->mSegmentList.empty())
		theLine = theTextArea->AddLine();

	theLine->mWidth = theTextArea->mWrapIndentSize;
	theTextArea->AddSeg(this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextSegmentBase::GetSelText(TextArea *theTextArea, GUIString &theStr)
{
	if(mLineBreak && theTextArea->mSelStartChar<=mAbsStartChar && theTextArea->mSelEndChar>mAbsEndChar)
		theStr.append("\r\n");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TextSegment::TextSegment()
{
	mColor = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextSegment::CalcDimensions()
{
	mWidth = mFont->GetStringWidth(mText);
	mHeight = mFont->GetHeight();
	mAscent = mFont->GetAscent();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextSegment::GetSelText(TextArea *theTextArea, GUIString &theStr)
{
	int aSelStartChar = theTextArea->mSelStartChar;
	int aSelEndChar = theTextArea->mSelEndChar;

	if(mAbsStartChar > aSelStartChar)
		aSelStartChar = mAbsStartChar;

	if(mAbsEndChar < aSelEndChar)
		aSelEndChar = mAbsEndChar;

	if(aSelStartChar >= aSelEndChar)
		return;

	aSelStartChar -= mAbsStartChar;
	aSelEndChar -= mAbsStartChar;

	theStr.append(mText, aSelStartChar, aSelEndChar-aSelStartChar);
	if(mLineBreak && theTextArea->mSelEndChar>mAbsEndChar)
		theStr.append("\r\n");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextSegment::Wrap(TextArea *theTextArea, TextLine *theLine)
{
	mLine = NULL; // set mLine to NULL then reset it to aLine on the first line to which we add a segment piece

	int aCurPos = 0;
	GUIString &aStr = mText;
	Font *aFM = mFont;
	int aTextBoxWidth = theTextArea->GetScrollArea().Width(); 
	TextLine *aLine = theLine;

	while(aCurPos < aStr.length())
	{
		int aWidth = aLine->mWidth;
		int aBreakWidth = aLine->mWidth;
		int aStartPos = aCurPos;
		int aBreakPos = aStartPos-1;
		while(aCurPos < aStr.length())
		{
			int aChar = aStr.at(aCurPos);
			aWidth += aFM->GetCharWidth(aChar);
			if(aChar==' ') 
			{
				aBreakPos = aCurPos;
				aBreakWidth = aWidth;
			}

			if(aWidth > aTextBoxWidth)
				break;

			aCurPos++;
		}

		bool forceBreak = false;
		if(aBreakPos < aStartPos)
		{
			if(aWidth < aTextBoxWidth)
				aBreakPos = aStr.length(); 
			else if(aLine->mSegmentList.empty()) // force the break in the middle of the word if we're at the beginning of the line
			{
				aBreakPos = aCurPos;
				aBreakWidth = aTextBoxWidth;
				if(aBreakPos==aStartPos)	// need at least one character per line
					aBreakPos++;

				forceBreak = true;
			}
			else	// otherwise try again on new line
				aCurPos = 0;
		}

		if(aBreakPos >= aStartPos)
		{
			if(aBreakPos!=aStartPos)
			{
				theTextArea->AddSeg(new TextSegmentPiece(this, aStartPos, aBreakPos, aBreakWidth/*aTextBoxWidth-aLine->mWidth*/));
				if(mLine==NULL)
					mLine = aLine;
			}

			aCurPos = aBreakPos;
			if(!forceBreak)	// Skip the space where we wrapped
				aCurPos++;
		}

		if(aCurPos>=aStr.length())
			break;

		aLine = theTextArea->AddLine(mHeight, mAscent);
		if(theTextArea->mLineAlign==HorzAlign_Left)
			aLine->mWidth = theTextArea->mWrapIndentSize;

		aWidth = aFM->GetStringWidth(aStr, aCurPos, aStr.length()-aCurPos);
		if(aWidth <= aTextBoxWidth - aLine->mWidth)
		{
			theTextArea->AddSeg(new TextSegmentPiece(this, aCurPos, aStr.length(), aWidth));
			if(mLine==NULL)
				mLine = aLine;

			break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextSegment::Paint(Graphics &g, TextArea *theTextArea, int theStartChar, int theEndChar, int xpos, int ypos, int lineHeight)
{
	int aSelStartChar = theTextArea->mSelStartChar - mAbsStartChar;
	int aSelEndChar = theTextArea->mSelEndChar - mAbsStartChar;

	if(theStartChar > aSelStartChar)
		aSelStartChar = theStartChar;

	if(theEndChar < aSelEndChar)
		aSelEndChar = theEndChar;

	g.SetColor(mColor);	
	g.SetFont(mFont);
	
	bool fullSelection = aSelStartChar==theStartChar && aSelEndChar==theEndChar;
	if(!fullSelection)
		g.DrawString(mText, theStartChar, theEndChar - theStartChar, xpos, ypos);

	if(aSelEndChar > aSelStartChar)
	{
		int x = xpos + mFont->GetStringWidth(mText,theStartChar,aSelStartChar-theStartChar);
		int aWidth = mFont->GetStringWidth(mText,aSelStartChar,aSelEndChar-aSelStartChar);

		g.SetColor(theTextArea->mSelColor.GetBackgroundColor(g));
		g.FillRect(x,0,aWidth,lineHeight);
		
		g.SetColor(theTextArea->mSelColor.GetForegroundColor(g,mColor));
		g.DrawString(mText,aSelStartChar,aSelEndChar-aSelStartChar,x,ypos);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextSegment::Paint(Graphics &g, TextArea *theTextArea)
{
	Paint(g,theTextArea,0,mText.length(),mx,my,mLine->mHeight);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int TextSegment::CharAt(int theWidth, int theStartChar, int theEndChar)
{
	int aPos = theStartChar;

	int aWidth = 0;
	int aCharWidth = 0;
	while(aPos<theEndChar && aWidth<theWidth)
	{
		aCharWidth = mFont->GetCharWidth(mText.at(aPos));
		aWidth+=aCharWidth;
		aPos++;
	}

	if(theWidth>=0 && theWidth<aWidth-aCharWidth/2)
		aPos--;

	return aPos + mAbsStartChar;	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int TextSegment::CharAt(int x)
{
	return CharAt(x-mx, 0, mText.length());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TextSegmentPiece::TextSegmentPiece(TextSegment *theSeg, int theStartChar, int theEndChar, int theWidth)
{
	mSeg = theSeg;
	mWidth =  theWidth;
	mHeight = theSeg->mHeight;
	mAscent = theSeg->mAscent;

	mStartChar = theStartChar;
	mEndChar = theEndChar;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextSegmentPiece::Paint(Graphics &g, TextArea *theTextArea)
{
	mSeg->Paint(g,theTextArea,GetStartChar(),GetEndChar(),mx,my,mLine->mHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int TextSegmentPiece::CharAt(int x)
{
	return mSeg->CharAt(x-mx,GetStartChar(),GetEndChar());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextImage::Paint(Graphics &g, TextArea *theTextArea)
{
	if(theTextArea->mSelStartChar<=mAbsStartChar && theTextArea->mSelEndChar>=mAbsEndChar)
	{
		g.SetColor(theTextArea->mSelColor.GetBackgroundColor(g));
		g.FillRect(mx,0,mWidth,mLine->mHeight);
	}

	g.DrawImage(mImage,mx,my);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextImage::CalcDimensions()
{
	mWidth = mImage->GetWidth();
	mHeight = mImage->GetHeight();
	mAscent = mHeight;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextImage::GetSelText(TextArea *theTextArea, GUIString &theStr)
{
	if(theTextArea->mSelStartChar<=mAbsStartChar && theTextArea->mSelEndChar>=mAbsEndChar)
	{
		theStr.append(mText);
		if(mLineBreak && theTextArea->mSelEndChar>mAbsEndChar)
			theStr.append("\r\n");
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TextLink::TextLink()
{
	mDownColor = 0xFF0000;
	mPressed = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextLink::Pressed() 
{ 
	mPressed = true; 
	mNormalColor = mColor; 
	mColor = mDownColor; 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TextLink::Released() 
{ 
	if(mPressed) 
	{ 
		mColor = mNormalColor; 
		mPressed = false; 
		return true;
	}	

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextAlignInstruction::PerformInstructions(TextArea *theTextArea)
{
	theTextArea->mLineAlign = mAlign;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextWidthInstruction::PerformInstructions(TextArea *theTextArea)
{
	if(mLine->mWidth < mLineShouldBeAtLeastThisWidth)
		mWidth = mLineShouldBeAtLeastThisWidth - mLine->mWidth;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextAnchorInstruction::PerformInstructions(TextArea *theTextArea)
{
	theTextArea->AddAnchor(mAnchorName, theTextArea->GetNumLines()-1);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TextLine::TextLine()
{
	mx = my = 0;
	mHeight = 0;
	mWidth = 0;
	mBaseline = 0;
	mDescent = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TextLine::~TextLine()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TextArea::TextArea()
{
	mSearchLine = new TextLine;

	mTextFlags = TextFlag_WordWrap | TextFlag_AllowSelection | TextFlag_ShowPartialLines | TextFlag_ScrollOnBottom;
	mHasBeenVisible = false;

	mTextColor = ColorScheme::GetColorRef(StandardColor_Text);
	mFont = GetNamedFont("TextArea");

	mLinkColor = 0x0000FF;
	mLinkFont = GetNamedFont("TextAreaLink");
	if(mLinkFont.get()==GetWindow()->GetWindowManager()->GetDefaultFont())
	{
		FontDescriptor aDescriptor = mFont->GetDescriptor();
		aDescriptor.mStyle |= FontStyle_Underline;
		mLinkFont = GetFont(aDescriptor);
	}
	SetComponentFlags(ComponentFlag_WantFocus, true);

	mMaxChars = 80*2000; mMaxLines = 0; 
	mWrapIndentSize = 0;
	mLineAlign = HorzAlign_Left;

	mTopPad = mLeftPad = mRightPad = mBottomPad = 5;

	Reset();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TextArea::~TextArea()
{
	//Reset(false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::Reset(bool justDisplay)
{
	mLineVector.clear();
	mAnchorMap.clear();

	if(!justDisplay)
	{
		mSegmentList.clear();

		mSelecting = false;
		mSelStartChar = mSelEndChar = 0;
		mBackwardsSel = false;	
		mVertOffset = mHorzOffset = 0;
		mTotalWidth = mTotalHeight = 0;

		AdjustPosition();
	}

	mNumLines = 0;
	mTopLineVectorPos = 0;
	mTotalHeight = mTotalWidth = 0;
	mCurLink = NULL;
	mLastFindSeg = NULL;
	mNeedRecalcWordWrap = false;
	mDelayWordWrapTimeAccum = 0;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::RecalcWordWrap(bool wasDelayed)
{
	Reset(true);
	SegmentList::iterator anItr = mSegmentList.begin();
	while(anItr!=mSegmentList.end())
	{
		TextSegmentBase *aSeg = *anItr;
		AddSegmentPrv(aSeg);
		++anItr;
	}

	if(ShouldScrollOnBottom())
		mVertOffset = mTotalHeight;

	if(wasDelayed)
		AdjustPosition();
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::SizeChanged()
{
	int oldWidth = mScrollArea.Width();
	bool scrollOnBottom = ShouldScrollOnBottom();
	ScrollArea::SizeChanged();

	// Re-calc wordwrap if necessary by reinserting every segment
	if(TextFlagSet(TextFlag_WordWrap) && !mSegmentList.empty() && mScrollArea.Width()!=oldWidth && mHasBeenVisible)
	{
		if(TextFlagSet(TextFlag_DelayWordWrap) && !mSegmentList.empty() && mSegmentList.back()->mAbsEndChar > 5000)
		{
			mDelayWordWrapTimeAccum = 0;
			mNeedRecalcWordWrap = true;
			RequestTimer(true);
		}
		else
			RecalcWordWrap(false);
	}

	if(scrollOnBottom)
		mVertOffset = mTotalHeight + mExtraHeight - mScrollArea.height;

	AdjustPosition();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::AdjustPosition(bool becauseOfScrollbar)
{
	mExtraHeight = 0;
	if(TextFlagSet(TextFlag_ScrollFullLine) && mTotalHeight>mScrollArea.height) 
	{
		// Need to calculate extra height in textbox in order to be able to see the last line 
		// while still seeing the entire top line
		LineVector::iterator anItr = FindLineAt(mTotalHeight-mScrollArea.height);
		if(anItr!=mLineVector.end())
		{
			TextLine *aLine = *anItr;
			mExtraHeight = aLine->my + aLine->mHeight + mScrollArea.height - mTotalHeight;
			if(mExtraHeight < 0)
				mExtraHeight = 0;
		}
	}

	ScrollArea::AdjustPosition(becauseOfScrollbar);
	FindTopLine();

	if(TextFlagSet(TextFlag_ScrollFullLine))
	{
		if(mTopLineVectorPos<mLineVector.size())
		{
			TextLine *aLine = mLineVector[mTopLineVectorPos]; 
			mVertOffset = aLine->my;
			mDownScrollAmount = aLine->mHeight;
			if(mTopLineVectorPos>0)
			{
				aLine = mLineVector[mTopLineVectorPos-1];
				mUpScrollAmount = aLine->mHeight;
			}
			ScrollArea::AdjustPosition(true);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::AddSeg(TextSegmentBase *theSegment)
{
	TextLine *aLine = mLineVector.back();

	bool needAdjust = false;
	int aBaseline = theSegment->mAscent;
	if(aBaseline > aLine->mBaseline)
	{
		aLine->mBaseline = aBaseline;
		needAdjust = true;
	}

	int aDescent = theSegment->mHeight - theSegment->mAscent;
	if(aDescent > aLine->mDescent)
		aLine->mDescent = aDescent;

	if(theSegment->mHeight > aLine->mHeight)
	{
		mTotalHeight -= aLine->mHeight;
		aLine->mHeight = theSegment->mHeight;
		mTotalHeight += aLine->mHeight;
		needAdjust = true;
	}	

	if(aLine->mDescent + aLine->mBaseline > aLine->mHeight)
	{
		mTotalHeight -= aLine->mHeight;
		aLine->mHeight = aLine->mBaseline + aLine->mDescent;
		mTotalHeight += aLine->mHeight;
		needAdjust = true;
	}

	if(needAdjust)
	{
		TextLine::SegmentList::iterator anItr = aLine->mSegmentList.begin();
		while(anItr!=aLine->mSegmentList.end())
		{
			TextSegmentBase *aSeg = *anItr;
			aSeg->my = aLine->mBaseline - aSeg->mAscent;
			++anItr;
		}
	}

	theSegment->mx = aLine->mWidth;
	theSegment->my = aLine->mBaseline - theSegment->mAscent;
	theSegment->mLine = aLine;
	aLine->mSegmentList.push_back(theSegment);

	aLine->mWidth += theSegment->mWidth;
	AlignLine(aLine, mLineAlign);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::AlignLine(TextLine *theLine, HorzAlign theAlign)
{
	int x = 0;
	switch(theAlign)
	{
		case HorzAlign_Center: x = (mScrollArea.width - theLine->mWidth)/2; break;
		case HorzAlign_Right: x = mScrollArea.width - theLine->mWidth; break;
	}

	if(x<=0)
		x = 0;

	theLine->mx = x;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TextLine* TextArea::AddLine(int theHeight, int theBaseline)
{
	TextLinePtr aNewLine = new TextLine;
	aNewLine->mHeight = theHeight;
	aNewLine->mBaseline = theBaseline;
	if(!mLineVector.empty())
	{
		TextLine *lastLine = mLineVector.back();
		aNewLine->my = lastLine->my + lastLine->mHeight;
	}
	
	mLineVector.push_back(aNewLine);
	mTotalHeight+=theHeight;

	return aNewLine;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::LineBreak(int theHeight)
{
	TextSegmentBasePtr aSeg = new TextSegmentBase;
	aSeg->mHeight = aSeg->mAscent = theHeight;
	aSeg->mLineBreak = true;
	AddSegment(aSeg);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::SetLineAlignment(HorzAlign theAlignment)
{
	if(theAlignment!=mLineAlign)
	{
		TextAlignInstructionPtr anInstruction = new TextAlignInstruction;
		anInstruction->mAlign = theAlignment;
		AddSegment(anInstruction);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::AddSegmentPrv(TextSegmentBase *theSegment)
{
	if(mLineVector.empty())
		AddLine();

	TextLine *aLine = mLineVector.back();
	theSegment->mLine = aLine;

	theSegment->PerformInstructions(this);
	if(Width()<=0 || aLine->mWidth + theSegment->mWidth <= mScrollArea.width || !TextFlagSet(TextFlag_WordWrap) || !mHasBeenVisible)
	{
		AddSeg(theSegment);
		if(aLine->mWidth > mTotalWidth && !TextFlagSet(TextFlag_WordWrap))
			mTotalWidth = aLine->mWidth;
	}
	else
		theSegment->Wrap(this, aLine);
	
	if(theSegment->mLineBreak)
	{
		mNumLines++;
		AddLine();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TextArea::ShouldScrollOnBottom()
{
	if(TextFlagSet(TextFlag_ScrollOnBottom))
	{
		if(!mUserHasScrolled)
			return true;

		if(!mLineVector.empty())
		{
			TextLine *aLine = mLineVector.back();
			if(aLine->mHeight==0 && mLineVector.size()>1)
				aLine = mLineVector[mLineVector.size()-2];

			if(IsLineVisible(aLine))
				return true;
		}
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::AddSegment(TextSegmentBase *theSegment)
{
	int anOldHeight = mTotalHeight;
	if(!mSegmentList.empty())
	{
		TextSegmentBase *aSeg = mSegmentList.back();
		theSegment->mAbsStartChar = aSeg->mAbsEndChar;
	}
		
	theSegment->mAbsEndChar = theSegment->mAbsStartChar + theSegment->GetLength();


	// Handle ScrollOnBottom
	bool scrollOnBottom = ShouldScrollOnBottom();

	mSegmentList.push_back(theSegment);
	theSegment->CalcDimensions();
	AddSegmentPrv(theSegment);
	EnforceMaxChars();	
	EnforceMaxLines();
 
	if(scrollOnBottom)
		mVertOffset = mTotalHeight + mExtraHeight - mScrollArea.height;

	AdjustPosition();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::AddAnchor(const GUIString &theName, int theLineNum)
{
	mAnchorMap[theName] = theLineNum;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TextArea::GotoAnchor(const GUIString &theName)
{
	if(GetNumLines()<=0)
		return false;

	AnchorMap::iterator anItr = mAnchorMap.find(theName);
	if(anItr==mAnchorMap.end())
		return false;

	int aLineNum = anItr->second;
	if(aLineNum>=mLineVector.size())
		aLineNum = mLineVector.size()-1;

	if(aLineNum<0)
		aLineNum = 0;

	TextLine *aLine = mLineVector[aLineNum];
	mVertOffset = aLine->my;
	AdjustPosition();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::EnforceInvariants()
{
	mTotalWidth = 0;
	mTotalHeight = 0;
	mNumLines = 0;

	if(mLineVector.empty() || mSegmentList.empty())
		return;
	
	TextLine *aLine = mLineVector.front();
	int aDeltaPos = aLine->my;
	for(LineVector::iterator aLineItr = mLineVector.begin(); aLineItr!=mLineVector.end(); ++aLineItr)
	{
		aLine = *aLineItr;
		if(aLine->mWidth > mTotalWidth)
			mTotalWidth = aLine->mWidth;

		aLine->my-=aDeltaPos;
		mTotalHeight+=aLine->mHeight;
	}

	TextSegmentBase *aSeg = mSegmentList.front();
	int aDeltaChar = aSeg->mAbsStartChar;
	for(SegmentList::iterator aSegmentItr = mSegmentList.begin(); aSegmentItr!=mSegmentList.end(); ++aSegmentItr)
	{
		aSeg = *aSegmentItr;
		aSeg->mAbsStartChar-=aDeltaChar;
		aSeg->mAbsEndChar-=aDeltaChar;
		if(aSeg->mLineBreak)
			mNumLines++;
	}
	mSelStartChar -= aDeltaChar;
	mSelEndChar -= aDeltaChar;
	if(mSelStartChar < 0)
		mSelStartChar = 0;
	if(mSelEndChar<0)
		mSelEndChar = 0;

	mVertOffset-=aDeltaPos;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::RemoveSegmentsUpTo(TextSegmentBase *theSegment)
{
	TextLine *aFindLine = theSegment->mLine;

	mLastFindSeg = NULL;

	bool foundSeg = false;
	LineVector::iterator aLineItr = mLineVector.begin();
	while(aLineItr!=mLineVector.end())
	{
		TextLine *aLine = *aLineItr;
		if(aLine==aFindLine)
			foundSeg = true;

		if(foundSeg && !aLine->mSegmentList.empty())
		{
			TextSegmentBase *aSeg = aLine->mSegmentList.back();
			if(aSeg->GetActual()!=theSegment) // can't delete line if it contains other segments
			{
				theSegment->mLine = aLine;
				break;
			}					
		}

		++aLineItr;
	}
	mLineVector.erase(mLineVector.begin(), aLineItr);

	TextSegmentBase *aStopSeg = NULL;
	if(!mLineVector.empty())
	{
		TextLine *aLine = mLineVector.front();
		if(!aLine->mSegmentList.empty())
			aStopSeg = aLine->mSegmentList.front()->GetActual();
	}

	SegmentList::iterator aSegmentItr = mSegmentList.begin();
	TextSegmentBase *aSeg = NULL;
	while(aSegmentItr!=mSegmentList.end())
	{	
		aSeg = *aSegmentItr;
		if(aSeg==aStopSeg)
			break;

		if(aSeg==mCurLink)
			mCurLink = NULL;
		
		++aSegmentItr;
	}
	mSegmentList.erase(mSegmentList.begin(), aSegmentItr);

	EnforceInvariants();
	AdjustPosition();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::EnforceMaxChars()
{
	if(mMaxChars<=0 || mSegmentList.empty())
		return;

	TextSegmentBase *aSeg = mSegmentList.back();
	int aNumCharsToRemove =  aSeg->mAbsEndChar - mMaxChars;
	if(aNumCharsToRemove<=0)
		return;

	int aCharCount = 0;
	SegmentList::iterator anItr = mSegmentList.begin();
	while(anItr!=mSegmentList.end())
	{
		aSeg = *anItr;
		aCharCount = aSeg->mAbsEndChar+1;
		if(aCharCount >= aNumCharsToRemove)
			break;

		++anItr;
	}

	RemoveSegmentsUpTo(aSeg);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::EnforceMaxLines()
{
	if(mMaxLines<=0 || mSegmentList.empty())
		return;

	TextSegmentBase *aSeg = mSegmentList.back();
	int aNumLinesToRemove =  mNumLines - mMaxLines;
	if(aNumLinesToRemove<=0)
		return;

	int aLineCount = 0;
	SegmentList::iterator anItr = mSegmentList.begin();
	while(anItr!=mSegmentList.end())
	{
		aSeg = *anItr;
		if(aSeg->mLineBreak)
			aLineCount++;

		if(aLineCount >= aNumLinesToRemove)
			break;

		++anItr;
	}

	RemoveSegmentsUpTo(aSeg);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TextArea::IsLineVisible(TextLine *theLine)
{
	if(theLine->my + theLine->mHeight < mVertOffset)
		return false;

	int aYPos = theLine->my;
	if(!TextFlagSet(TextFlag_ShowPartialLines))
		aYPos += theLine->mHeight;

	return aYPos <= mVertOffset+mScrollArea.height;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::NotifyRootVisibilityChange(bool isVisible)
{
	ScrollArea::NotifyRootVisibilityChange(isVisible);
	if(isVisible && !mHasBeenVisible)
	{
		mHasBeenVisible = true;
		if(TextFlagSet(TextFlag_WordWrap) && !mSegmentList.empty())
			RecalcWordWrap(true);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::ScrollPaint(Graphics &g)
{
	LineVector::iterator aLineItr = mLineVector.begin()+mTopLineVectorPos; //mTopLineItr;
	TextLine::SegmentList::iterator aSegItr;
	while(aLineItr!=mLineVector.end())
	{
		TextLine *aLine = *aLineItr;
		if(!IsLineVisible(aLine))
			break;

		aSegItr = aLine->mSegmentList.begin();
		g.Translate(aLine->mx,aLine->my);
		while(aSegItr!=aLine->mSegmentList.end())
		{
			TextSegmentBase *aSeg = *aSegItr;
			aSeg->Paint(g,this);
			++aSegItr;
		}
		g.Translate(-aLine->mx,-aLine->my);
		++aLineItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TextArea::LineVector::iterator TextArea::FindLineAt(int theYPos)
{
	mSearchLine->my = theYPos;
	LineVector::iterator anItr = std::lower_bound(mLineVector.begin(), mLineVector.end(), mSearchLine, LineSearchPred());
	if(anItr==mLineVector.end() || (*anItr)->my>theYPos)
	{
		if(anItr!=mLineVector.begin())
			--anItr;
	}

	return anItr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::FindTopLine()
{
	LineVector::iterator mTopLineItr = FindLineAt(mVertOffset);
	mTopLineVectorPos = mTopLineItr - mLineVector.begin();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TextSegmentBase* TextArea::FindSegment(int x, int y, bool lenient)
{
	if(mLineVector.empty())
		return NULL;

	if(lenient)
	{
		if(x>=mTotalWidth && mTotalWidth!=0) x = mTotalWidth-1;
		if(x<0) x = 0;
		if(y>=mTotalHeight) y = mTotalHeight-1;
		if(y<0) y = 0;

	}
	if(mLastFindSeg!=NULL)
	{
		TextLine *aLine = mLastFindSeg->mLine;
		int xpos = x-aLine->mx;
		if(aLine->my<=y && aLine->my+aLine->mHeight>=y)
		{
			if(xpos>=mLastFindSeg->mx && xpos<=mLastFindSeg->mx + mLastFindSeg->mWidth)
				return mLastFindSeg;
		}
	}

	LineVector::iterator aLineItr = FindLineAt(y);
	TextLine *aLine = *aLineItr;
	if(!lenient && aLine->my + aLine->mHeight < y)
		return NULL;

	x-=aLine->mx;
	TextLine::SegmentList::iterator aSegItr = aLine->mSegmentList.begin();
	
	TextSegmentBase *aSeg = NULL;
	while(aSegItr!=aLine->mSegmentList.end())
	{
		aSeg = *aSegItr;
		if(x<=aSeg->mx+aSeg->mWidth)
			break;

		++aSegItr;
	}

	if(aSeg!=NULL && !lenient)
		if(x<aSeg->mx)
			aSeg = NULL;

	mLastFindSeg = aSeg;
	return aSeg;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::CheckSel(int x, int y, bool newStart)
{
	TextSegmentBase *aSeg = FindSegment(x,y,true);
	if(aSeg==NULL)
		return;

	int aSelChar = aSeg->CharAt(x-aSeg->mLine->mx);
	if(newStart)
	{
		mSelStartChar = mSelEndChar = aSelChar;
		mBackwardsSel = false;
	}
	else
	{
		if(mBackwardsSel)
			mSelStartChar = aSelChar;
		else
			mSelEndChar = aSelChar;

		if(mSelStartChar > mSelEndChar)
		{
			mBackwardsSel = !mBackwardsSel;
			std::swap(mSelStartChar,mSelEndChar);
		}
	}
	
	Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TextArea::TimerEvent(int theDelta)
{
	if(mNeedRecalcWordWrap)
	{
		mDelayWordWrapTimeAccum++;
		if(mDelayWordWrapTimeAccum > 1) // don't accumulate time since resizing is probably causing delays
		{
			int aNumChars = 0;
			if(!mSegmentList.empty())
				aNumChars = mSegmentList.back()->mAbsEndChar;

			if(mDelayWordWrapTimeAccum>=10 || mDelayWordWrapTimeAccum>=aNumChars/16000)
				RecalcWordWrap(true);
		}
	}

	if(!mSelecting)
		return mNeedRecalcWordWrap;

	int x,y;
	GetMousePos(x,y);
	if(x>=0 && x<=Width() && y>=0 && y<=Height())
		return true;

	if(y>Height())
	{
		int aDelta = y-Height();
		if(TextFlagSet(TextFlag_ScrollFullLine) && aDelta<mDownScrollAmount) 
			aDelta = mDownScrollAmount;
		
		mVertOffset+=aDelta;
	}
	else if(y<0)
	{
		int aDelta = -y;
		if(TextFlagSet(TextFlag_ScrollFullLine) && aDelta<mUpScrollAmount)
			aDelta = mUpScrollAmount;

		mVertOffset-=aDelta;
	}

	if(x>Width())
		mHorzOffset+=(x-Width());
	else if(x<0)
		mHorzOffset+=x;
		
	AdjustPosition();
	ScrollTranslate(x,y);
	CheckSel(x,y,false);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::MouseDown(int x, int y, MouseButton theButton)
{
	ScrollArea::MouseDown(x,y,theButton);
	if(theButton!=MouseButton_Left)
		return;

	ScrollTranslate(x,y);

	RequestFocus();

	if(mCurLink!=NULL)
	{
		mCurLink->Pressed();
		Invalidate();
		mSelecting = false;
		return;
	}
	
	if(TextFlagSet(TextFlag_AllowSelection))
	{
		mSelecting = true;
		RequestTimer(true);
		CheckSel(x,y,(GetKeyMod() & KEYMOD_SHIFT)?false:true);
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::MouseUp(int x, int y, MouseButton theButton)
{
	ScrollArea::MouseUp(x,y,theButton);
	if(theButton!=MouseButton_Left)
		return;

	ScrollTranslate(x,y);

	TextSegmentBase *aSeg = FindSegment(x,y,false);
	bool linkPressed = false;
	if(mCurLink!=NULL)
		linkPressed = mCurLink->Released();

	if(aSeg==mCurLink && linkPressed)
	{
		// link activated
		mLinkParam = mCurLink->GetLinkParam();
		FireEvent(new TextLinkActivatedEvent(this,mCurLink->GetActual()));
	}

	if(mCurLink!=NULL)
	{
		if(aSeg==NULL || !aSeg->IsLink())
		{
			mCurLink = NULL;
			mParent->SetCursor(TextFlagSet(TextFlag_AllowSelection)?Cursor::GetStandardCursor(StandardCursor_IBeam):(Cursor*)NULL);
		}
		else
			mCurLink = aSeg;

		Invalidate();
	}

	mSelecting = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::MouseExit()
{
	ScrollArea::MouseExit();
	if(mCurLink!=NULL)
	{
		mCurLink->Released();
		mCurLink = NULL;
		Invalidate();
	}

	mSelecting = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::MouseMove(int x, int y)
{
	ScrollArea::MouseMove(x,y);
	ScrollTranslate(x,y);

	TextSegmentBase *aSeg = FindSegment(x,y,false);
	if(aSeg==mCurLink)
		return;
	
	if(aSeg!=NULL && aSeg->IsLink())
	{
		mCurLink = aSeg;
		mParent->SetCursor(Cursor::GetStandardCursor(StandardCursor_Hand));
	}
	else 
	{
		mCurLink = NULL;
		mParent->SetCursor(TextFlagSet(TextFlag_AllowSelection)?Cursor::GetStandardCursor(StandardCursor_IBeam):(Cursor*)NULL);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::MouseEnter(int x, int y)
{
	ScrollArea::MouseEnter(x,y);

	mParent->SetCursor(TextFlagSet(TextFlag_AllowSelection)?Cursor::GetStandardCursor(StandardCursor_IBeam):(Cursor*)NULL);
	MouseMove(x,y);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::MouseDrag(int x, int y)
{
	ScrollArea::MouseDrag(x,y);
	ScrollTranslate(x,y);
	if(mSelecting)
		CheckSel(x,y,false);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TextArea::KeyDown(int theKey)
{
	if(ScrollArea::KeyDown(theKey))
		return true;

	switch(theKey)
	{
		case KEYCODE_RIGHT: mHorzOffset += 10; break;
		case KEYCODE_LEFT: mHorzOffset -= 10; break;
		case KEYCODE_DOWN: mVertOffset += mDownScrollAmount; break;
		case KEYCODE_UP: mVertOffset -= mUpScrollAmount; break;
		case KEYCODE_PGDN: mVertOffset += mScrollArea.height; break;
		case KEYCODE_PGUP: mVertOffset -= mScrollArea.height; break;
		default: return false;
	}

	mUserHasScrolled = true;
	AdjustPosition();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::CopySelection()
{
	GUIString aStr;
	SegmentList::iterator anItr = mSegmentList.begin();

	TextSegmentBase *aSeg;
	while(anItr!=mSegmentList.end())
	{
		aSeg = *anItr;
		aSeg->GetSelText(this, aStr);
		++anItr;
	}

	Clipboard::Set(aStr);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TextArea::KeyChar(int theKey)
{
	if(ScrollArea::KeyChar(theKey))
		return true;

	if(theKey==3) // CTRL-C
	{
		CopySelection();
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::AddLink(const GUIString &theText, const GUIString &theLinkParam, bool lineBreak)
{
	TextLinkPtr aSeg = new TextLink;
	aSeg->mText = theText;
	aSeg->mLinkParam = theLinkParam;
	aSeg->mLineBreak = lineBreak;
	aSeg->mColor = mLinkColor;
	aSeg->mFont = mLinkFont;
	AddSegment(aSeg);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::AddImage(Image *theImage, const GUIString &theCopyText, bool lineBreak)
{
	TextImagePtr aSeg = new TextImage;
	aSeg->mImage = theImage;
	aSeg->mText = theCopyText;
	aSeg->mLineBreak = lineBreak;
	AddSegment(aSeg);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::AddSegment(const GUIString &theText, bool lineBreak)
{
	AddSegment(theText, mTextColor, lineBreak); 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::AddSegment(const GUIString &theText, int theColor, bool lineBreak)
{
	TextSegmentPtr aSeg = new TextSegment;
	aSeg->mText = theText;
	aSeg->mColor = theColor;
	aSeg->mFont = mFont;
	aSeg->mLineBreak = lineBreak;
	AddSegment(aSeg);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::AddSegment(const GUIString &theText, int theColor, Font *theFont, bool lineBreak)
{
	TextSegmentPtr aSeg = new TextSegment;
	aSeg->mText = theText;
	aSeg->mColor = theColor;
	aSeg->mFont = theFont;
	aSeg->mLineBreak = lineBreak;
	AddSegment(aSeg);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::AddFormatedText(const GUIString &theText, bool lineBreak)
{
	int aStartPos = 0;
	int anEndPos = 0;
	int aNextStartPos = 0;
	while(aStartPos < theText.length())
	{
		int anEndPos = aStartPos;
		while(anEndPos < theText.length()) // find "\r", "\r\n", or "\n" 
		{
			int aChar = theText.at(anEndPos);
			if(aChar=='\r')
			{
				if(anEndPos+1 < theText.length() && theText.at(anEndPos+1)=='\n')
					aNextStartPos = anEndPos + 2;
				else
					aNextStartPos = anEndPos + 1;
				
				break;
			}
			else if(aChar=='\n')
			{
				aNextStartPos = anEndPos + 1;
				break;
			}

			anEndPos++;
		}

		bool foundReturn = true;
		if(anEndPos>=theText.length())
		{
			aNextStartPos = theText.length(); // Force it to exit next pass.
			foundReturn = lineBreak;
		}

		AddSegment(theText.substr(aStartPos,anEndPos-aStartPos),foundReturn);
		aStartPos = aNextStartPos;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::SetTextFlags(int theFlags, bool on)
{
	bool wordWrap = TextFlagSet(TextFlag_WordWrap);
	bool allowSelection = TextFlagSet(TextFlag_AllowSelection);
	if(on)
		mTextFlags |= theFlags;
	else
		mTextFlags &= ~theFlags;

	if(!wordWrap && TextFlagSet(TextFlag_WordWrap))
		SizeChanged(); // re-layout the text

	if(!TextFlagSet(TextFlag_AllowSelection) && mSelStartChar!=mSelEndChar) // cancel selection
	{
		mSelecting = false;
		mSelStartChar = mSelEndChar = 0;
		Invalidate();
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::SetMaxChars(int theMaxChars)
{
	mMaxChars = theMaxChars;
	EnforceMaxChars();
	AdjustPosition();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextArea::SetMaxLines(int theMaxLines)
{
	mMaxLines = theMaxLines;
	EnforceMaxLines();
	AdjustPosition();
}