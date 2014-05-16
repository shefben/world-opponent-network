#include "HTMLDisplayGen.h"
#include "WONGUI/Window.h"
#include "WONGUI/SimpleComponent.h"
#include "WONGUI/MSControls.h"
#include "WONGUI/ImageButton.h"

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTMLDisplayGen::HTMLDisplayGen()
{
	mNoBackground = false;
	mFontSize = HTMLFontSize_Medium;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static std::string gGlobalDefaultFontFace;// = "Times New Roman";
void HTMLDisplayGen::SetGlobalDefaultFontFace(const std::string &theFace)
{
	gGlobalDefaultFontFace = theFace;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTMLTag* HTMLDisplayGen::GetNextTag()
{
	if(mTagListItr==mTagListEnd)
		return NULL;
	else
		return *mTagListItr++;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::CalcLineEmpty(int theFlags, bool addedSegmentHasContent)
{
	if(theFlags&(LineSegmentFlag_PreLineBreak))
	{
		mPrevLineEmpty = mCurLineEmpty;
		mCurLineEmpty = true;
		mLastWasSpace = true;
	}

	if(addedSegmentHasContent)
		mCurLineEmpty = false;

	if(theFlags&LineSegmentFlag_PostLineBreak)
	{
		mPrevLineEmpty = mCurLineEmpty;
		mCurLineEmpty = true;
		mLastWasSpace = true;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::AddDisplaySegment(LineSegment *theSegment, int theFlags)
{
	if(mCanBreak)
	{
		ClearAccumulatedSegment();

		if(theFlags!=0)
			mDisplay->AddSegment(theSegment, theFlags);
		else
			mPrevSegment = theSegment;

//		mDisplay->AddSegment(theSegment, theFlags);
		CalcLineEmpty(theFlags,true);		
	}
	else
	{
		if(!mCurLine.IsStarted())
		{
			mCurLine.StartBuild(new LineElement);
			mCurLine.mLine->mIsGlueLine = true;
		}

		if(mPrevSegment.get()!=NULL)
			mCurLine.AddSegment(mPrevSegment);

		mPrevSegment = theSegment;
		mCurLineEmpty = false;
	}	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::AddNonDisplaySegment(LineSegment *theSegment, int theFlags)
{
	ClearAccumulatedSegment();
	mDisplay->AddSegment(theSegment, theFlags);
	CalcLineEmpty(theFlags,false);		
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::AddEmptySegment(LineSegment *theSegment, int theFlags)
{
	ClearAccumulatedSegment();
	mDisplay->AddSegment(theSegment, theFlags);
	if(!mCurLineEmpty)
		CalcLineEmpty(theFlags,false);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::ClearAccumulatedSegment(int theFlags)
{
	mCanBreak = true;
	if(mPrevSegment.get()!=NULL)
	{
		if(!mCurLine.IsEmpty())
		{
			mCurLine.AddSegment(mPrevSegment);
			mCurLine.FinishBuild();
			mDisplay->AddSegment(mCurLine.mLine,theFlags);
			mCurLine.Clear();
		}
		else
			mDisplay->AddSegment(mPrevSegment,theFlags);

		CalcLineEmpty(theFlags,true);	
		
		mPrevSegment = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::StartFreshLine()
{
	if(!mCurLineEmpty)
		AddNonDisplaySegment(new LineBreakSegment(0),LineSegmentFlag_PostLineBreak);
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::StartFreshParagraph()
{
	if(!mCurLineEmpty)
		AddNonDisplaySegment(new LineBreakSegment(20),LineSegmentFlag_PreLineBreak | LineSegmentFlag_PostLineBreak);
	else if(!mPrevLineEmpty)
		AddNonDisplaySegment(new LineBreakSegment(20),LineSegmentFlag_PostLineBreak);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::AddLineBreak(int extraFlags)
{
	if(!mCurLineEmpty)
		AddNonDisplaySegment(new LineBreakSegment(0),extraFlags | LineSegmentFlag_PostLineBreak);
	else
		AddNonDisplaySegment(new LineBreakSegment(20),extraFlags | LineSegmentFlag_PostLineBreak);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int HTMLDisplayGen::GetFontStyle()
{
	int aStyle = 0;
	if(mBoldCount)
		aStyle |= FontStyle_Bold;
	if(mItalicCount)
		aStyle |= FontStyle_Italic;
	if(mUnderlineCount)
		aStyle |= FontStyle_Underline;

	return aStyle;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLDisplayGen::GetIsFontMonospace()
{
	if(mFontStack.empty())
		return false;
	else
		return mFontStack.back().mIsMonoSpace;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int HTMLDisplayGen::GetFontSize()
{
	if(mFontStack.empty())
		return -1;
	else
		return mFontStack.back().mSize;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::CalcFont()
{
	static int FONT_SIZE_TABLE[HTMLFontSize_Largest+1][8] = 
	{
		{  4,  6,  8,  9, 10, 12, 16, 24 },
		{  5,  7,  9, 10, 12, 16, 20, 30},
		{  6,  8, 10, 12, 14, 18, 24, 36},
		{  8, 10, 12, 14, 16, 20, 28, 42},
		{ 10, 12, 14, 16, 18, 24, 32, 48}
	};
	
	if(mFontStack.empty())
		return;

	FontInfo anInfo = mFontStack.back();
	int aSize = anInfo.mSize-1;
	if(aSize<0)
		aSize = 0;
	if(aSize>6)
		aSize = 6;

	if(!anInfo.mIsMonoSpace)
		aSize++;

	int aPointSize = FONT_SIZE_TABLE[mFontSize][aSize];

	FontDescriptor aDescriptor(anInfo.mFace, GetFontStyle(), aPointSize);
	mCurFont = mWindow->GetFont(aDescriptor);
	if(anInfo.mColor==COLOR_INVALID)
		mCurColor = mDefaultTextColor;
	else
		mCurColor = anInfo.mColor;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::PushFont(const char *theFace, int theSize, int theColor, bool isMonoSpace)
{
	FontInfo anInfo;
	if(!mFontStack.empty())
		anInfo = mFontStack.back();
	else
	{
		anInfo.mColor = COLOR_INVALID;
		anInfo.mSize = 3;
		anInfo.mFace = mDefaultFontFace;
		anInfo.mIsMonoSpace = false;
	}

	if(theFace!=NULL)
		anInfo.mFace = theFace;

	if(theSize>=1 && theSize<=7)
		anInfo.mSize = theSize;

	if(theColor!=COLOR_INVALID)
		anInfo.mColor = theColor;

	anInfo.mIsMonoSpace = isMonoSpace;

	mFontStack.push_back(anInfo);
	CalcFont();
}	

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::PopFont()
{
	if(mFontStack.size()>1)
	{
		mFontStack.pop_back();
		CalcFont();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTMLAlign HTMLDisplayGen::GetCurAlign()
{
	if(mAlignStack.empty())
		return mDefaultAlign;
	else
		return mAlignStack.back();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::PushAlign(HTMLAlign theAlign, bool force)
{
	HTMLAlign anOldAlign = GetCurAlign();
	if(theAlign==HTMLAlign_None)
		theAlign = anOldAlign;

	mAlignStack.push_back(theAlign);

	if(anOldAlign!=theAlign || force)
		AddEmptySegment(new AlignLineSegment((HorzAlign)theAlign),LineSegmentFlag_PreLineBreak);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::PopAlign()
{
	if(mAlignStack.empty())
		return;

	HTMLAlign anOldAlign = GetCurAlign();
	mAlignStack.pop_back();
	HTMLAlign aNewAlign = GetCurAlign();

	if(aNewAlign!=anOldAlign)
		AddEmptySegment(new AlignLineSegment((HorzAlign)aNewAlign),LineSegmentFlag_PreLineBreak);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::HandleBody(HTMLBodyTag *theTag)
{
	if(theTag->IsEnd())
	{
		if(mBottomMargin>0)
			AddNonDisplaySegment(new LineBreakSegment(mBottomMargin),LineSegmentFlag_PreLineBreak | LineSegmentFlag_PostLineBreak);

		mDone = true;

		return;
	}

	if(mDisplayContainer.get()!=NULL) // Set BGColor
	{
		if(mNoBackground)
		{} // do nothing		mDisplayContainer->SetBackground(COLOR_INVALID);
		else if(theTag->mBGImage.get()!=NULL)
		{
			mDisplayContainer->SetBackground(theTag->mBGImage.get());
			if(theTag->mTextColor<0)
				mDefaultTextColor = 0;
			if(theTag->mLinkColor<0)
				mDefaultLinkColor = 0x0000ff;
			if(theTag->mALinkColor<0)
				mDefaultALinkColor = 0xff0000;
		}
		else if(theTag->mBGColor<0)
			mDisplayContainer->SetBackground(Background());
		else		
			mDisplayContainer->SetBackground(theTag->mBGColor);
	}

	if(theTag->mTextColor>=0)
		mDefaultTextColor = theTag->mTextColor;

	if(theTag->mLinkColor>=0)
		mDefaultLinkColor = theTag->mLinkColor;

	if(theTag->mALinkColor>=0)
		mDefaultALinkColor = theTag->mALinkColor;

	mFontStack.clear();
	PushFont(mDefaultFontFace.c_str(),3,COLOR_INVALID);


	mLeftMargin = theTag->mLeftMargin;
	mRightMargin = theTag->mRightMargin;
	mTopMargin = theTag->mTopMargin;
	mBottomMargin = theTag->mBottomMargin;

	if(mLeftMargin>0 || mRightMargin>0)
		AddNonDisplaySegment(new MarginLineSegment(mLeftMargin,mRightMargin));

	if(mTopMargin>0)
		AddNonDisplaySegment(new LineBreakSegment(mTopMargin),LineSegmentFlag_PostLineBreak);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HTMLDisplayGen::FindBody()
{
	HTMLTag *aTag = GetNextTag();
	if(aTag==NULL)
		return false;

	if(aTag->GetType()!=HTMLTagType_Body)
	{
		// auto generate the body tag
		SmartPtr<HTMLBodyTag> aBodyTag = new HTMLBodyTag;
		HandleTag(aBodyTag);
	}

	mTagListItr--;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int HTMLDisplayGen::HandleBeginGlueWord(const GUIString &anOldText)
{
	int i = 0;
	GUIString aNewText;

	if(mLastWasSpace && !mInPre)
	{
		for(; i<anOldText.length(); i++)
		{
			int aChar = anOldText.at(i);
			if(!safe_isspace(aChar))
				break;
		}

		if(i==anOldText.length())
			return i;
	}

	bool lastWasSpace = false;
	for(; i<anOldText.length(); i++)
	{
		int aChar = anOldText.at(i);
		if(!safe_isspace(aChar))
			aNewText.append(aChar);
		else
		{
			if(i==0) // space starts off 
			{
				ClearAccumulatedSegment();
				break;
			}
			else
			{
				if(mInPre)
					HandlePreSpace(aChar,aNewText);
				else
					aNewText.append(' ');

				lastWasSpace = true;
				i++;
			}
			break;
		}
	}

	if(!aNewText.empty())
	{
		AddDisplaySegment(GetTextLineSegment(aNewText,mCurColor,mCurFont));
		if(lastWasSpace)
			ClearAccumulatedSegment();
	}
	mLastWasSpace = lastWasSpace;

	return i;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::HandlePreSpace(int theChar, GUIString &theText)
{
	switch(theChar)
	{
		case '\n':
			AddDisplaySegment(GetTextLineSegment(theText,mCurColor,mCurFont),LineSegmentFlag_PostLineBreak);
			ClearAccumulatedSegment(LineSegmentFlag_PostLineBreak);
			theText.erase();
			break;

		default:
			theText.append(' ');
	}	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TextLineSegment* HTMLDisplayGen::GetTextLineSegment(const GUIString &theStr, int theColor, Font *theFont)
{
	if(mInAnchor)
		return new TextLinkLineSegment(mCurLinkParam,theStr,theColor,mDefaultALinkColor,theFont);
	else
		return new TextLineSegment(theStr,theColor,theFont);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::HandleTextTag(HTMLTextTag *theTag)
{
	const GUIString &anOldText = theTag->GetText();
	if(anOldText.empty())
		return;

	GUIString aNewText(anOldText.length());

	int i=0, aLastWordPos = 0;
	if(!mCanBreak)
		i = HandleBeginGlueWord(anOldText);

	bool lastWasSpace = mLastWasSpace; // mCanBreak

	const int aLength = anOldText.length();
	for(; i<aLength; i++)
	{
		int aChar = anOldText.at(i);
		if(lastWasSpace)
		{
			if(!safe_isspace(aChar))
			{
				aLastWordPos = aNewText.length();
				aNewText.append(aChar);
				lastWasSpace = false;
			}
			else if(mInPre)
				HandlePreSpace(aChar,aNewText);
		}
		else
		{
			if(safe_isspace(aChar))
			{
				if(mInPre)
					HandlePreSpace(aChar,aNewText);
				else
					aNewText.append(' ');
	
				lastWasSpace = true;
			}
			else
				aNewText.append(aChar);
		}
	}

	mLastWasSpace = lastWasSpace;
	if(aNewText.empty())
	{
		mCanBreak = lastWasSpace;	
		return;
	}

	if(!lastWasSpace) // check if there's a space at the beginning of the next tag
	{
		// no space at start of next tag --> must glue last word of this text to first word of next text

		HTMLTextTag *aNextTag = GetNextTextTag();
		if(aNextTag!=NULL && !safe_isspace(aNextTag->GetText().at(0))) 
		{
			GUIString aLastWord = aNewText.substr(aLastWordPos);
			aNewText.resize(aLastWordPos);

			if(!aNewText.empty())
			{
				AddDisplaySegment(GetTextLineSegment(aNewText,mCurColor,mCurFont));
				ClearAccumulatedSegment();
			}

			if(!aLastWord.empty())
				AddDisplaySegment(GetTextLineSegment(aLastWord,mCurColor,mCurFont));

			mCanBreak = false;
			return;
		}
	}

	if(!aNewText.empty())
	{
		ClearAccumulatedSegment();
		AddDisplaySegment(GetTextLineSegment(aNewText,mCurColor,mCurFont));
	}
	
	mCanBreak = lastWasSpace;	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HTMLTextTag* HTMLDisplayGen::GetNextTextTag()
{
	HTMLTagList::const_iterator anItr = mTagListItr;
	while(anItr!=mTagList->end())
	{
		HTMLTag *aTag = *anItr;
		if(aTag->GetType()==HTMLTagType_Text)
			return (HTMLTextTag*)aTag;
		else if(aTag->InducesLineBreak())
			return NULL;

		++anItr;
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::HandleLineBreak(HTMLLineBreakTag *theTag)
{
	int anExtraFlags = 0;
	if(theTag->mClearLeft)
		anExtraFlags |= LineSegmentFlag_BreakClearLeft;
	if(theTag->mClearRight)
		anExtraFlags |= LineSegmentFlag_BreakClearRight;

	AddLineBreak(anExtraFlags);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::HandleHorizontalRule(HTMLHorizontalRuleTag *theTag)
{
	HorizontalRuleSegmentPtr aSeg = new HorizontalRuleSegment;
	aSeg->mHeight = theTag->mSize;
	aSeg->mWidthSpec = theTag->mWidthSpec;
	aSeg->mNoShade = theTag->mNoShade;

	PushAlign(theTag->mAlign);
	StartFreshLine();
	AddDisplaySegment(aSeg,LineSegmentFlag_PostLineBreak);
	PopAlign();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::HandleHeading(HTMLHeadingTag *theTag)
{
	if(theTag->IsStart())
	{
		StartFreshParagraph();
		PushAlign(theTag->mAlign);
		mBoldCount++;
		PushFont(mDefaultFontFace.c_str(),HTMLTagType_H6 - theTag->GetType() + 1, COLOR_INVALID);
	}
	else
	{
		mBoldCount--;
		PopFont(); PopAlign();
		StartFreshParagraph();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::HandleAnchor(HTMLAnchorTag *theTag)
{
	if(theTag->IsStart())
	{
		if(!theTag->mName.empty())
			mDisplay->AddSegment(new AnchorLineSegment(theTag->mName));
		if(!theTag->mHRef.empty())
		{
			mInAnchor = true;
			mUnderlineCount++;
			PushFont(NULL,-1,mDefaultLinkColor, GetIsFontMonospace());
			mCurLinkParam = new TextLinkParam(theTag->mHRef, theTag->mLaunchBrowser);
		}
	}
	else
	{
		if(mInAnchor)
		{
			mInAnchor = false;
			mUnderlineCount--;
			PopFont();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::HandlePre(HTMLTag *theTag)
{
	if(theTag->IsStart())
	{
		StartFreshParagraph();
		mInPre = true;
		mNoBreak++;
		PushFont("Courier New",-1,COLOR_INVALID,true);
	}
	else
	{
		PopFont();
		mInPre = false;
		mNoBreak--;
		StartFreshParagraph();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::HandleCenter(HTMLTag *theTag)
{
	StartFreshLine();
	if(theTag->IsStart())
		PushAlign(HTMLAlign_Center);
	else
		PopAlign();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::HandleModifyFontStyle(HTMLTag *theTag, int theStyleMod)
{
	int anInc = theTag->IsStart()?1:-1;
	if(theStyleMod & FontStyle_Bold)
		mBoldCount += anInc;

	if(theStyleMod & FontStyle_Italic)
		mItalicCount += anInc;

	if(theStyleMod & FontStyle_Underline)
		mUnderlineCount += anInc;

	CalcFont();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::HandleModifyFont(HTMLTag *theTag, const char *theFace, int theSize, int theColor, bool isMonoSpace)
{
	if(theTag->IsStart())
		PushFont(theFace,theSize,theColor, isMonoSpace);
	else 
		PopFont();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::HandleFont(HTMLFontTag *theTag)
{
	if(theTag->IsStart())
	{
		if(theTag->mFace.empty())
			PushFont(NULL,theTag->mSize,theTag->mColor);
		else
		{
			std::string aStr = theTag->mFace;
			PushFont(aStr.c_str(),theTag->mSize,theTag->mColor);
		}
	}
	else
		PopFont();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::EndParagraph()
{
	PopAlign();
	StartFreshParagraph();
	mInParagraph = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::HandleBlock(HTMLBlockTag *theTag)
{
	if(theTag->IsStart())
	{
		if(theTag->GetType()==HTMLTagType_Paragraph) 
		{
			StartFreshParagraph();
			mInParagraph = true;
		}
		else
			StartFreshLine();

		PushAlign(theTag->mAlign);
	}
	else
	{
		if(theTag->GetType()!=HTMLTagType_Paragraph) // paragrpaphs end automatically when another block level element is encountered
		{
			StartFreshLine();
			PopAlign();
		}
		else if(mInParagraph)
			EndParagraph();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::HandleImage(HTMLImageTag* theTag)
{
	if(theTag->IsEnd())
		return;

	if(theTag->mAnimation.get()!=NULL)
	{
		AnimationComponentPtr anAnimComp = new AnimationComponent(theTag->mAnimation);
		anAnimComp->Start();
		AddDisplaySegment(new ComponentLineSegment(anAnimComp));
		mCanBreak = false;
		return;
	}
	else if(theTag->mImage.get()==NULL)
		return;

	Image *anImage = theTag->mImage;

	ImageLineSegmentPtr anImageSegment;

	ImageMap *anImageMap = theTag->mImageMap;

	int aBorder = mInAnchor?2:0;
	int aBorderColor = mCurColor;
	if(mInAnchor)
		anImageSegment = new ImageLinkLineSegment(mCurLinkParam,mCurColor,mDefaultALinkColor,anImage);
	else if(anImageMap!=NULL)
	{
		aBorder = 2;
		aBorderColor = mDefaultLinkColor;
		anImageSegment = new ImageLinkLineSegment(NULL,mCurColor,mDefaultALinkColor,anImage);
		((ImageLinkLineSegment*)anImageSegment.get())->mImageMap = anImageMap;
	}
	else
		anImageSegment = new ImageLineSegment(anImage);

	if(theTag->mBorder>=0)
		aBorder = theTag->mBorder;

	int aFlags = 0;
	int anAlign = 0;

	switch(theTag->mAlign)
	{
		case HTMLAlign_Left: aFlags |= LineSegmentFlag_FloatLeft; break;
		case HTMLAlign_Right: aFlags |= LineSegmentFlag_FloatRight; break;
		case HTMLAlign_Middle: anAlign = 1; break;
		case HTMLAlign_Top: anAlign = 2; break;
	}

	anImageSegment->mHSpace = theTag->mHSpace;
	anImageSegment->mVSpace = theTag->mVSpace;
	anImageSegment->mAlign = anAlign;
	anImageSegment->mBorder = aBorder;
	anImageSegment->mBorderColor = aBorderColor;

	ClearAccumulatedSegment();
	AddDisplaySegment(anImageSegment,aFlags);

	if(aFlags==0)
	{
		mCanBreak = false;
		mLastWasSpace = false;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::HandleList(HTMLListTag* theTag)
{
	if(theTag->IsEnd())
	{	
		if(mListDepth<=0)
			return;

		mListDepth--;
		AddEmptySegment(new MarginLineSegment(mListDepth*40+mLeftMargin,mRightMargin),LineSegmentFlag_PreLineBreak);
		if(mListDepth==0)
			StartFreshParagraph();
	}
	else
	{
		mListDepth++;
		if(mListDepth==1)
			StartFreshParagraph();

		AddEmptySegment(new MarginLineSegment(mListDepth*40+mLeftMargin,mRightMargin),LineSegmentFlag_PreLineBreak);

	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::HandleListItem(HTMLListItemTag* theTag)
{
	if(theTag->IsEnd())
		return; 

	StartFreshLine();

	if(mListDepth<1)
		AddDisplaySegment(new BulletElement(mCurColor));
	else if(!theTag->mBulletText.empty())
		AddDisplaySegment(new OrderedListElement(mCurColor,mCurFont,theTag->mBulletText));
	else
		AddDisplaySegment(new BulletElement(mCurColor,theTag->mListType - HTMLListType_Disc + 1));

	mCanBreak = false;
	mLastWasSpace = true;
	mCurLineEmpty = mPrevLineEmpty = true; // prevent new paragraph from asking for a carriage return
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::HandleTable(HTMLTableTag *theTag)
{
	if(theTag->IsEnd())
	{
		if(mInTable)
		{
			mInTable = false;
			int aFlags = LineSegmentFlag_PreLineBreak;
			if(mCurTableAlign==HTMLAlign_Left)
				aFlags |= LineSegmentFlag_FloatLeft;
			else if(mCurTableAlign==HTMLAlign_Right)
				aFlags |= LineSegmentFlag_FloatRight;
			else 
			{
				if(mCurTableAlign==HTMLAlign_Center)
					PushAlign(HTMLAlign_Center);
			}
			AddDisplaySegment(mCurTable,aFlags);
			if(mCurTableAlign==HTMLAlign_Center)
				PopAlign();
			else if(mCurTableAlign==HTMLAlign_None)
				StartFreshLine();
		}
		else if(mParsingTableData)
			HandleEndTableData();
	}
	else
	{
		ClearAccumulatedSegment();
		mInTable = true;
		mCurTable = new TableElement;
		mCurTable->mCellSpacing = theTag->mCellSpacing;
		mCurTable->mCellPadding = theTag->mCellPadding;
		mCurTable->mBorder = theTag->mBorder;
		mCurTable->mBGColor = theTag->mBGColor;
		mCurTable->mWidthSpec = theTag->mWidthSpec;
		mCurTableAlign = theTag->mAlign;

		mCurTable->StartTraversal();
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::HandleTableRow(HTMLTableRowTag *theTag)
{
	if(theTag->IsEnd())
	{
		if(mParsingTableData && !mInTable)
			HandleEndTableData();

		return;
	}

	if(mInTable)
	{
		mCurTable->NextRow();
		mCurTable->SetRowBGColor(theTag->mBGColor);

		int anHAlign = theTag->mHAlign - HTMLAlign_Left;
		int aVAlign = theTag->mVAlign - HTMLAlign_Top;
		mCurTable->SetRowAlign(anHAlign, aVAlign);
	}
	else if(mParsingTableData)
		HandleEndTableData();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::HandleTableData(HTMLTableDataTag *theTag)
{
	if(theTag->IsEnd())
	{
		if(mParsingTableData && !mInTable)
			HandleEndTableData();

		return;
	}

	if(mInTable)
	{
		TableElement::CellInfo *aCellInfo = mCurTable->NextColumn();
		MultiLineElement *anElement = aCellInfo->mContents;

		aCellInfo->mBGColor = theTag->mBGColor;
		aCellInfo->mHAlign = theTag->mHAlign - HTMLAlign_Left;
		aCellInfo->mVAlign = theTag->mVAlign - HTMLAlign_Top;
		if(aCellInfo->mVAlign<0 || aCellInfo->mVAlign>2)
			aCellInfo->mVAlign = -1;
		if(aCellInfo->mHAlign<0 || aCellInfo->mHAlign>2)
			aCellInfo->mHAlign = -1;

		aCellInfo->mNoWrap = theTag->mNoWrap;
		if(theTag->mWidthSpec>=0)
			aCellInfo->mWidth = theTag->mWidthSpec;		
		else if(theTag->mColSpan<=1)
			mCurTable->SetColumnWidth(theTag->mWidthSpec);

		if(theTag->mHeight>0)
			aCellInfo->mHeight = theTag->mHeight;

		HTMLAlign aDefaultAlign = (HTMLAlign)((mCurTable->GetCurRowAlign() - HorzAlign_Left) + HTMLAlign_Left);
		if(aCellInfo->mHAlign==1)
			aDefaultAlign = HTMLAlign_Center;
		else if(aCellInfo->mHAlign==2)
			aDefaultAlign = HTMLAlign_Right;

		// Important!  aCellInfo can be invalidated with SetCellSpan since this can grow the vector so call SetCellSpan after done using aCellInfo
		if(theTag->mRowSpan>1 || theTag->mColSpan>1)
			mCurTable->SetCellSpan(theTag->mRowSpan, theTag->mColSpan);

		HTMLDisplayGen aGen;

	    aGen.GenerateTableData(*this,anElement,theTag->GetType()==HTMLTagType_TableHeader,aDefaultAlign);
	}
	else if(mParsingTableData)
		HandleEndTableData();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::HandleEndTableData()
{
	mTagListItr--;
	mDone = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::HandleGUIComponent(HTMLGUIComponentTag *theTag)
{
	AddDisplaySegment(new ComponentLineSegment(theTag->mComponent));
	mCanBreak = false;
	mLastWasSpace = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::HandleOption(HTMLTag *theTag)
{
	if(mCurCombo.get()==NULL)
		return;

	if(theTag->IsEnd())
		return;

	GUIString anOption;
	bool lastWasSpace = true;
	while(true)
	{
		HTMLTag *aTag = GetNextTag();
		if(aTag==NULL)
			break;

		if(aTag->GetType()==HTMLTagType_Option)
		{
			if(aTag->IsStart()) // get the next option on the next pass
				mTagListItr--; 

			break;
		}
		else if(aTag->GetType()==HTMLTagType_Text)
		{
			const GUIString &anOldText = ((HTMLTextTag*)aTag)->GetText();
			for(int i=0; i<anOldText.length(); i++)
			{
				int aChar = anOldText.at(i);
				if(safe_isspace(aChar))
				{
					if(!lastWasSpace)
					{
						anOption.append(' ');
						lastWasSpace = true;
					}
				}
				else
				{
					anOption.append(aChar);
					lastWasSpace = false;
				}
			}	
		}
		else
		{
			mTagListItr--; // get the next option on the next pass
			break;
		}
	}

	mCurCombo->InsertString(anOption);
	int aWidth = mCurCombo->GetListArea()->GetTotalWidth();
	mCurCombo->SetSize(aWidth+20,mCurCombo->Height());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::HandleInput(HTMLInputTag *theTag)
{
	if(theTag->IsEnd())
	{
		if(mCurCombo.get()!=NULL)
		{
			AddDisplaySegment(new ComponentLineSegment(mCurCombo));
			mCanBreak = false;
			mLastWasSpace = false;
			mCurCombo = NULL;
		}
		return;
	}

	LineSegmentPtr aSegment;
	FontPtr aFont = mWindow->GetFont(FontDescriptor("Tahoma",FontStyle_Plain,10));

	switch(theTag->mInputType)
	{
		case HTMLInputType_Button:
		case HTMLInputType_Submit:
		case HTMLInputType_Reset:
		{
			MSButtonPtr aButton = new MSButton(theTag->mValue);
			aButton->SetFont(aFont);
			aButton->SetDesiredSize();
			aSegment = new ComponentLineSegment(aButton);
			break;
		}

		
		case HTMLInputType_Radio:
		{
			MSRadioButtonPtr aRadio = new MSRadioButton;
			aRadio->SetTransparent(true);
			aRadio->SetSize(15,12);
			aRadio->SetButtonFlags(ButtonFlag_Radio,false);
			aSegment = new ComponentLineSegment(aRadio);
			break;
		}

		case HTMLInputType_Image:
		{
			Image *anImage = theTag->mImage;
			if(anImage==NULL)
				break;

			ImageButtonPtr anImageButton = new ImageButton;
			anImageButton->SetSize(anImage->GetWidth(),anImage->GetHeight());
			anImageButton->SetImage(ImageButtonType_Normal,anImage);
			anImageButton->SetDefaultCursor(Cursor::GetStandardCursor(StandardCursor_Hand));
			aSegment = new ComponentLineSegment(anImageButton);
			break;
		}

		case HTMLInputType_Checkbox: break;
		case HTMLInputType_File: break;
		case HTMLInputType_Hidden: break;

		case HTMLInputType_Select:
			mCurCombo = new MSComboBox;
			mCurCombo->SetSize(50,22);
			break;

		case HTMLInputType_Text:
		case HTMLInputType_Password:
		{
			MSInputBoxPtr anInputBox = new MSInputBox;
			anInputBox->SetFont(aFont);
			if(theTag->mInputType==HTMLInputType_Password)
				anInputBox->SetPasswordChar('*');
			else if(theTag->mReadOnly)
				anInputBox->SetComponentFlags(ComponentFlag_WantFocus,false);

			anInputBox->SetSize(theTag->mSize*6+24,22);
			anInputBox->SetMaxChars(theTag->mMaxLength);
			anInputBox->SetText(theTag->mValue);
			aSegment = new ComponentLineSegment(anInputBox);
			break;
		}
	}

	if(aSegment.get()!=NULL)
	{
		AddDisplaySegment(aSegment);
		mCanBreak = false;
		mLastWasSpace = false;
	}	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::HandleTag(HTMLTag *theTag)
{
	if(mInParagraph && theTag->IsBlockElement() && theTag->IsStart())
	{
//		if(theTag->GetType()==HTMLTagType_ListItem || theTag->GetType()==HTMLTagType_OrderedList || theTag->GetType()==HTMLTagType_UnorderedList)
//		{
//			mCurLineEmpty = true;	
//		}
//		EndParagraph();
	}

	switch(theTag->GetType())
	{
		case HTMLTagType_Body:				HandleBody((HTMLBodyTag*)theTag); break;
		case HTMLTagType_Anchor:			HandleAnchor((HTMLAnchorTag*)theTag); break;
		case HTMLTagType_LineBreak:			HandleLineBreak((HTMLLineBreakTag*)theTag); break;
		case HTMLTagType_HorizontalRule:	HandleHorizontalRule((HTMLHorizontalRuleTag*)theTag); break;
		case HTMLTagType_Text:				HandleTextTag((HTMLTextTag*)theTag); break;
		case HTMLTagType_Bold:				HandleModifyFontStyle(theTag,FontStyle_Bold); break;
		case HTMLTagType_Italic:			HandleModifyFontStyle(theTag,FontStyle_Italic); break;
		case HTMLTagType_Underline:			HandleModifyFontStyle(theTag,FontStyle_Underline); break;
		case HTMLTagType_Center:			HandleCenter(theTag); break;
		case HTMLTagType_Small:				HandleModifyFont(theTag,NULL,2,COLOR_INVALID,GetIsFontMonospace()); break;
		case HTMLTagType_Big:				HandleModifyFont(theTag,NULL,4,COLOR_INVALID,GetIsFontMonospace()); break;
		case HTMLTagType_Code:				HandleModifyFont(theTag,"Courier New",-1,COLOR_INVALID,true); break;
		case HTMLTagType_Pre:				HandlePre(theTag); break;
		case HTMLTagType_Font:				HandleFont((HTMLFontTag*)theTag); break;
		case HTMLTagType_BaseFont:			HandleFont((HTMLFontTag*)theTag); break;
		
		case HTMLTagType_Div:				
		case HTMLTagType_Paragraph:			HandleBlock((HTMLBlockTag*)theTag); break;

		case HTMLTagType_Table:				HandleTable((HTMLTableTag*)theTag); break;
		case HTMLTagType_TableRow:			HandleTableRow((HTMLTableRowTag*)theTag); break;
		case HTMLTagType_TableData:			
		case HTMLTagType_TableHeader:		HandleTableData((HTMLTableDataTag*)theTag); break;

		case HTMLTagType_UnorderedList:
		case HTMLTagType_OrderedList:		HandleList((HTMLListTag*)theTag); break;
		case HTMLTagType_ListItem:			HandleListItem((HTMLListItemTag*)theTag); break;

		case HTMLTagType_Image:				HandleImage((HTMLImageTag*)theTag); break;

		case HTMLTagType_Input:
		case HTMLTagType_Select:			HandleInput((HTMLInputTag*)theTag); break;
		case HTMLTagType_Option:			HandleOption(theTag); break;

		case HTMLTagType_H1:
		case HTMLTagType_H2:
		case HTMLTagType_H3:
		case HTMLTagType_H4:
		case HTMLTagType_H5:
		case HTMLTagType_H6:				HandleHeading((HTMLHeadingTag*)theTag); break;

		case HTMLTagType_GUIComponent:		HandleGUIComponent((HTMLGUIComponentTag*)theTag); break;
	}
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::GenerateTableData(HTMLDisplayGen &theGen, MultiLineElement *theDisplay, bool isHeader,  HTMLAlign theStartAlign)
{
	mDisplay = theDisplay;
	mWindow = theGen.mWindow;
	mDefaultLinkColor = theGen.mDefaultLinkColor;
	mDefaultALinkColor = theGen.mDefaultALinkColor;
	mDefaultTextColor = theGen.mDefaultTextColor;
	mDefaultFontFace = theGen.mDefaultFontFace;
	mDefaultAlign = theStartAlign;
	mBoldCount = mItalicCount = mUnderlineCount = 0;
	mTagList = theGen.mTagList;
	mTagListItr = theGen.mTagListItr;
	mTagListEnd = theGen.mTagListEnd;
	mFontSize = theGen.mFontSize;

	mLeftMargin = mRightMargin = mTopMargin = mBottomMargin = 0;

	if(isHeader)
		mBoldCount++;

	mParsingTableData = true;

	PushFont(mDefaultFontFace.c_str(),3,COLOR_INVALID);
	if(mDefaultAlign!=HorzAlign_Left)
		PushAlign(mDefaultAlign,true);

	GenerateBase();

	theGen.mTagListItr = mTagListItr;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::Generate(HTMLDocument *theDocument, MultiLineElement *theDisplay, Table *theDisplayContainer)
{
	mTagList = &theDocument->GetTagList();
	mTagListItr = theDocument->GetBodyStart();
	mTagListEnd = theDocument->GetBodyEnd();

	mWindow = WindowManager::GetDefaultWindowManager()->GetDefaultWindow();
	mDisplay = theDisplay;
	mDisplayContainer = theDisplayContainer;

	mAlignStack.clear();
	mFontStack.clear();

	mDefaultTextColor = ColorScheme::GetColorRef(StandardColor_Text);

	if(gGlobalDefaultFontFace.empty())
		mDefaultFontFace = WindowManager::GetDefaultWindowManager()->GetDefaultFont()->GetFace();
	else
		mDefaultFontFace = gGlobalDefaultFontFace;

	mBoldCount = mItalicCount = mUnderlineCount = 0;
	mDefaultLinkColor = ColorScheme::GetColorRef(StandardColor_Link);
	mDefaultALinkColor = ColorScheme::GetColorRef(StandardColor_LinkDown);

	mParsingTableData = false;

	mDefaultAlign = HTMLAlign_Left;

	if(!FindBody())
		return;

	GenerateBase();

	LineSegment::mCounter = 0;
	mDisplay->SetSelIndex();
} 

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HTMLDisplayGen::GenerateBase()
{
	mListDepth = 0;
	mCanBreak = true;
	mLastWasSpace = true;
	mCurLineEmpty = true;
	mPrevLineEmpty = true;
	mInPre = false;
	mNoBreak = 0;
	mInAnchor = false;
	mInParagraph = false;
	mDone = false;
	mInTable = false;

	mNextTextTag = NULL;


	HTMLTag *aTag;
	while(!mDone)
	{
		aTag = GetNextTag();
		if(aTag==NULL)
			break;

		HandleTag(aTag);
	}
	ClearAccumulatedSegment();
}
