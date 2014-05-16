#include "Table.h"
#include "ColorScheme.h"
#include "NativeImage.h"
#include "RawImage.h"
#include "Clipboard.h"
#include <algorithm>

using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Table* LineSegment::mTableContext = NULL;
int LineSegment::mCounter = 0;
ImagePtr LineSegment::mSelectImageMask;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LineSegment::Wrap(MultiLineBuilder *theContainer, bool needSegmentOnFirstLine)
{
	if(!needSegmentOnFirstLine)
		theContainer->AddLine();

	theContainer->AddSegmentToCurLine(this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LineSegment::SetViewport(ScrollAreaViewport *theViewport, int x, int y)
{
	mAbsX = x;
	mAbsY = y;
//	mStartSelIndex = mCounter++;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LineSegment::GetSelText(GUIString &theText, ElementSelectionInfo &theInfo)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LineSegment::DrawSelection1(Graphics &g, int x, int y, ElementPaintInfo &theInfo)
{
	if(mStartSelIndex<theInfo.mStartSelSegment || mStartSelIndex>theInfo.mEndSelSegment)
		return false;

	if(mStartSelIndex==theInfo.mStartSelSegment && theInfo.mStartSelSegmentPos>0)
		return false;

	if(mStartSelIndex==theInfo.mEndSelSegment && theInfo.mEndSelSegmentPos<1)
		return false;

	g.ApplyColorSchemeColor(StandardColor_Hilight);
	g.FillRect(x,theInfo.mLineY,GetWidth(),theInfo.mLineHeight);
//	g.FillRect(x,y,GetWidth(),GetHeight());
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LineSegment::DrawSelection2(Graphics &g, int x, int y)
{
	const int MASK_WIDTH = 100;
	const int MASK_HEIGHT = 100;
	if(mSelectImageMask.get()==NULL)
	{
		PalettePtr aPalette = new Palette(2);
		aPalette->SetColor(0,0x000000);
		aPalette->SetColor(1,0x000080);
		RawImage8Ptr anImage = Window::CreateRawImage8(MASK_WIDTH,MASK_HEIGHT,aPalette);
		anImage->SetTransparentColor(0);

		for(int i=0; i<MASK_WIDTH; i++)
		{
			int pixel = i%2;
			anImage->StartRowTraversal(i);
			for(int j=0; j<MASK_HEIGHT; j++)
			{
				anImage->SetRowPixel(pixel);
				anImage->NextRowPixel();
				pixel = 1-pixel;
			}
		}
		mSelectImageMask = anImage->GetNative(g.GetDisplayContext());
	}

	int aWidth = GetWidth();
	int aHeight = GetHeight();
	g.PushClipRect(x,y,aWidth,aHeight);
	for(int i=0; i<aWidth; i+=MASK_WIDTH)
		for(int j=0; j<aHeight; j+=MASK_HEIGHT)
			g.DrawImage(mSelectImageMask,i+x,j+y);

	g.PopClipRect();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int LineSegment::GetSelChar(int x, int y)
{
	if(x >= GetWidth()/2)
		return 1;
	else
		return 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComponentLineSegment::Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo)
{
	bool selected = DrawSelection1(g,x,y,theInfo);

	mComponent->InvalidateDown();
	g.Translate(x,y);
	mComponent->Paint(g);
	g.Translate(-x,-y);
	if(selected)
		DrawSelection2(g,x,y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ComponentLineSegment::SetViewport(ScrollAreaViewport *theViewport, int x, int y)
{
	LineSegment::SetViewport(theViewport,x,y);
	if(mComponent->ComponentFlagSet(ComponentFlag_GrabBG))
		mComponent->SetComponentFlags(ComponentFlag_GrabBG,false);

	mComponent->SetParent(theViewport);	
	if(theViewport!=NULL)
	{
		mComponent->SetPos(x,y);
		mComponent->AddedToParent();	
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool AlignLineSegment::PerformInstructions(MultiLineBuilder *theContainer)
{
	theContainer->SetLineAlignment(mAlign);
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MarginLineSegment::PerformInstructions(MultiLineBuilder *theContainer)
{
	theContainer->SetIndentSize(mLeftMargin, mNextLeftMargin);
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool AnchorLineSegment::PerformInstructions(MultiLineBuilder *theContainer)
{
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AnchorLineSegment::SetViewport(ScrollAreaViewport *theViewport, int x, int y)
{
	LineSegment::SetViewport(theViewport,x,y);
	if(theViewport!=NULL)
	{
		Table *aTable = (Table*)theViewport->GetParent();
		aTable->AddAnchor(mName,x,y);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LineBreakSegment::GetSelText(GUIString &theText, ElementSelectionInfo &theInfo)
{
	if(mStartSelIndex<theInfo.mStartSelSegment || mStartSelIndex>theInfo.mEndSelSegment)
		return;

	if(mHeight>0)
		theText.append("\r\n\r\n");
	else
		theText.append("\r\n");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HorizontalRuleSegment::SetMaxWidth(int theWidth)
{
	if(mWidthSpec<0)
		mWidth = (theWidth * -mWidthSpec)/100;
	else if(mWidthSpec>0)
		mWidth = mWidthSpec;
	else
		mWidth = theWidth;

	if(mWidth>theWidth)
		mWidth = theWidth;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HorizontalRuleSegment::Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo)
{
	int pad = 7;
	DrawSelection1(g,x,y,theInfo);
/*	if(mStartSelIndex>=theInfo.mStartSelSegment && mStartSelIndex<=theInfo.mEndSelSegment)
	{
		g.SetColor(0x000080);
		g.FillRect(x,theInfo.mLineY,mWidth,theInfo.mLineHeight);
	}*/

	g.SetColor(0x808080);
	if(mNoShade)
		g.FillRect(x,y+pad,mWidth,mHeight);
	else
	{
		int light = 0xd6d6d6;
		int dark = 0x848484;
		bool up = false;

		g.SetColor(up?light:dark);
		g.DrawLine(x,y+pad,x+mWidth-1,y+pad);
		g.DrawLine(x,y+pad,x,y+pad+mHeight-1);
		g.SetColor(up?dark:light);
		g.DrawLine(x+mWidth-1,y+pad,x+mWidth-1,y+pad+mHeight-1);
		g.DrawLine(x,y+pad+mHeight-1,x+mWidth-1,y+pad+mHeight-1);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HorizontalRuleSegment::GetSelText(GUIString &theText, ElementSelectionInfo &theInfo)
{
	if(mStartSelIndex<theInfo.mStartSelSegment || mStartSelIndex>theInfo.mEndSelSegment)
		return;

	theText.append("-------------------------------------------------------------------------------\r\n");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int TextLineSegment::GetMinWidth()
{
	if(mMinWidth==0)
	{
		int aWidth = 0;
		int aPos = 0;
		while(aPos<mText.length())
		{
			int aChar = mText.at(aPos);
			if(aChar==' ')
			{
				if(aWidth>mMinWidth)
					mMinWidth = aWidth;

				aWidth = 0;
			}
			else
				aWidth += mFont->GetCharWidth(aChar);

			aPos++;
		}

		if(mMinWidth==0)  // no space found
			mMinWidth = aWidth;
	}

	return mMinWidth;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int TextLineSegment::GetWidth()
{
	if(mWidth==0)
		mWidth = mFont->GetStringWidth(mText);

	return mWidth;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int TextLineSegment::GetFirstWrapWidth()
{
	int aWidth = 0;
	int aPos = 0;
	while(aPos<mText.length())
	{
		int aChar = mText.at(aPos);
		if(aChar==' ')
			return aWidth;
		else
			aWidth += mFont->GetCharWidth(aChar);

		aPos++;
	}

	return aWidth;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int TextLineSegment::GetSelChar(int x, int y, int theStartChar, int theEndChar)
{
	int aWidth = 0;
	int aPos = theStartChar;
	while(aPos<theEndChar)
	{
		int aCharWidth = mFont->GetCharWidth(mText.at(aPos));
		if(aWidth + (aCharWidth>>1) >= x)
			break;

		aWidth += aCharWidth;
		aPos++;
	}

	return aPos;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int TextLineSegment::GetSelChar(int x, int y)
{
	return GetSelChar(x,y,0,mText.length());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextLineSegment::GetSelText(GUIString &theText, ElementSelectionInfo &theInfo)
{
	if(mStartSelIndex<theInfo.mStartSelSegment || mStartSelIndex>theInfo.mEndSelSegment)
		return;

	int aStartPos = 0;
	int anEndPos = mText.length();
	bool partial = false;
	if(theInfo.mStartSelSegment==mStartSelIndex)
	{
		aStartPos = theInfo.mStartSelSegmentPos;
		partial = true;
	}
	if(theInfo.mEndSelSegment==mStartSelIndex)
	{
		anEndPos = theInfo.mEndSelSegmentPos;
		partial = true;
	}

	if(partial)
		theText.append(mText.substr(aStartPos,anEndPos-aStartPos));
	else
		theText.append(mText);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TextLineSegment::GetSelCharPos(int &x, int &y, int theSelIndex, int theSelChar)
{
	if(theSelIndex!=mStartSelIndex)
		return false;

	if(theSelChar<0 || theSelChar>mText.length())
		return false;

	x = mAbsX + mFont->GetStringWidth(mText,0,theSelChar);
	y = mAbsY;
	return true;
}
		
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TextPieceLineSegment::GetSelCharPos(int &x, int &y, int theSelIndex, int theSelChar)
{
	if(theSelIndex!=mStartSelIndex)
		return false;

	if(theSelChar<mStartChar || theSelChar>mEndChar)
		return false;

	x = mAbsX + mSegment->mFont->GetStringWidth(mSegment->mText,mStartChar,theSelChar-mStartChar);
	y = mAbsY;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TextLineSegment::FindPartial(ElementFindInfo &theInfo, int startChar, int endChar)
{
	int p1 = startChar, p2 = theInfo.mCurFoundLength;
	const GUIString &aText = theInfo.mFindText;
	int len2 = aText.length();
	const bool caseSensitive = theInfo.mCaseSensitive;

	while(p1<endChar && p2<len2)
	{
		if(caseSensitive)
		{
			if(mText.at(p1)!=aText.at(p2))
				break;
		}
		else if(towupper(mText.at(p1))!=towupper(aText.at(p2)))
			break;

		p1++; p2++;
	}

	if(p1<endChar && p2<len2)
		return false;
	else
	{
		theInfo.mCurFoundLength = p2;
		theInfo.mFoundEndSegment = mStartSelIndex;
		theInfo.mFoundEndChar = p1;
		return true;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TextLineSegment::FindStr(ElementFindInfo &theInfo)
{ 
	if(mStartSelIndex<theInfo.mStartSelSegment || mStartSelIndex>theInfo.mEndSelSegment)
		return false;

	int aStartPos = 0;
	int anEndPos = mText.length();
	if(theInfo.mStartSelSegment==mStartSelIndex)
		aStartPos = theInfo.mStartSelSegmentPos;
	if(theInfo.mEndSelSegment==mStartSelIndex)
		anEndPos = theInfo.mEndSelSegmentPos;

	if(theInfo.mFoundStartSegment>=0) // 
	{
		if(FindPartial(theInfo,aStartPos,anEndPos))
			return theInfo.mCurFoundLength == theInfo.mFindText.length();

		theInfo.mFoundStartSegment = -1;
	}

	const GUIString &aText = theInfo.mFindText;
	int p1,p2;
	int cp = aStartPos;
	int len2 = aText.length();
	const bool caseSensitive = theInfo.mCaseSensitive;
	while(cp < anEndPos)
	{
		p1 = cp;
		p2 = 0;
		while(p1<anEndPos && p2<len2)
		{
			if(caseSensitive)
			{
				if(mText.at(p1)!=aText.at(p2))
					break;
			}
			else if(towupper(mText.at(p1))!=towupper(aText.at(p2)))
				break;

			p1++; p2++;
		}
		if(p1==anEndPos || p2==len2)
			break;

		cp++;
	}

	if(p2>0)
	{
		theInfo.mFoundX = mAbsX;
		theInfo.mFoundY = mAbsY;
		theInfo.mCurFoundLength = p2;
		theInfo.mFoundStartSegment = theInfo.mFoundEndSegment = mStartSelIndex;
		theInfo.mFoundStartChar = cp;
		theInfo.mFoundEndChar = p1;
 
		return theInfo.mCurFoundLength==theInfo.mFindText.length();
	}
	else
		return false; 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextLineSegment::Wrap(MultiLineBuilder *theContainer, bool needSegmentOnFirstLine)
{
	int aCurPos = 0;
	const GUIString &aStr = mText;
	Font *aFM = mFont;
	int aTextBoxWidth = theContainer->GetCurMaxLineWidth() - theContainer->CurLine()->GetWidth();
	
	int aWidth = 0;
	int aBreakWidth = 0;

	while(aCurPos < aStr.length())
	{
		int aStartPos = aCurPos;
		int aBreakPos = aStartPos-1;
		while(aCurPos < aStr.length())
		{
			int aChar = aStr.at(aCurPos);
			if(aChar==' ') 
			{
				aBreakPos = aCurPos;
				aBreakWidth = aWidth;
			}

			aWidth += aFM->GetCharWidth(aChar);

			if(aWidth>aTextBoxWidth && (aBreakPos>=aStartPos || !needSegmentOnFirstLine))
				break;

			aCurPos++;
		}

		if(aCurPos==aStr.length())
		{
			aBreakPos = aStr.length();
			aBreakWidth = aWidth;
		}
	
		needSegmentOnFirstLine = true;
		if(aBreakPos<aStartPos) // didn't find break
		{
			theContainer->AddLine();
			aCurPos = aStartPos;
			aTextBoxWidth = theContainer->GetCurMaxLineWidth();
			aWidth = aBreakWidth = 0;
			continue;
		}

		theContainer->AddSegmentToCurLine(new TextPieceLineSegment(this, aStartPos, aBreakPos, aBreakWidth));
		aCurPos = aBreakPos + 1; // Skip the space where we wrapped
		if(aCurPos>=aStr.length())
			break;

		theContainer->AddLine();
		aTextBoxWidth = theContainer->GetCurMaxLineWidth();
		aWidth = aFM->GetStringWidth(aStr,aCurPos, aStr.length()-aCurPos);
		if(aWidth <= aTextBoxWidth)
		{
			theContainer->AddSegmentToCurLine(new TextPieceLineSegment(this, aCurPos, aStr.length(), aWidth));
			break;
		}
		
		aWidth = aBreakWidth = 0;		
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextLineSegment::Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo)
{
	Paint(g,x,y,mStartSelIndex,0,mText.length(),GetWidth(),GetHeight(),theInfo);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextLineSegment::PaintSelection(Graphics &g, int x, int y, ElementPaintInfo &theInfo)
{
	PaintSelection(g,x,y,mStartSelIndex,0,mText.length(),GetWidth(),GetHeight(),theInfo);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextLineSegment::Paint(Graphics &g, int x, int y, int theSelIndex, int startChar, int endChar, int theWidth, int theHeight, ElementPaintInfo &theInfo)
{
	g.SetFont(mFont);
	if(theSelIndex>=theInfo.mStartSelSegment && theSelIndex<=theInfo.mEndSelSegment)
	{
		int aStartSelPos = startChar;
		int anEndSelPos = endChar;
		bool fullSelect = true;
		if(theSelIndex==theInfo.mStartSelSegment)
		{
			fullSelect = false;
			aStartSelPos = theInfo.mStartSelSegmentPos;
			if(aStartSelPos<startChar)
				aStartSelPos = startChar;
		}
		if(theSelIndex==theInfo.mEndSelSegment)
		{
			fullSelect = false;
			anEndSelPos = theInfo.mEndSelSegmentPos;
			if(anEndSelPos>endChar)
				anEndSelPos = endChar;
		}

		if(fullSelect)
		{
			g.ApplyColorSchemeColor(StandardColor_Hilight);
//			g.FillRect(x,theInfo.mLineY,theWidth,theInfo.mLineHeight);
			g.ApplyColorSchemeColor(StandardColor_HilightText);
			g.DrawString(mText,startChar,endChar-startChar,x,y);
			return;
		}
		else if(aStartSelPos<anEndSelPos)
		{
			int aStartWidth = mFont->GetStringWidth(mText,startChar,aStartSelPos-startChar);
			int aSelWidth = mFont->GetStringWidth(mText,aStartSelPos,anEndSelPos-aStartSelPos);
			g.SetColor(mColor);
			g.DrawString(mText,startChar,aStartSelPos-startChar,x,y);
			g.DrawString(mText,anEndSelPos,endChar-anEndSelPos,x+aStartWidth+aSelWidth,y);
			g.ApplyColorSchemeColor(StandardColor_Hilight);
//			g.FillRect(x+aStartWidth,theInfo.mLineY,aSelWidth,theInfo.mLineHeight);
			g.ApplyColorSchemeColor(StandardColor_HilightText);
			g.DrawString(mText,aStartSelPos,anEndSelPos-aStartSelPos,x+aStartWidth,y);
			return;
		}
	}

	g.SetColor(mColor);
	g.DrawString(mText,startChar,endChar-startChar,x,y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextLineSegment::PaintSelection(Graphics &g, int x, int y, int theSelIndex, int startChar, int endChar, int theWidth, int theHeight, ElementPaintInfo &theInfo)
{
	g.SetFont(mFont);
	if(theSelIndex>=theInfo.mStartSelSegment && theSelIndex<=theInfo.mEndSelSegment)
	{
		int aStartSelPos = startChar;
		int anEndSelPos = endChar;
		bool fullSelect = true;
		if(theSelIndex==theInfo.mStartSelSegment)
		{
			fullSelect = false;
			aStartSelPos = theInfo.mStartSelSegmentPos;
			if(aStartSelPos<startChar)
				aStartSelPos = startChar;
		}
		if(theSelIndex==theInfo.mEndSelSegment)
		{
			fullSelect = false;
			anEndSelPos = theInfo.mEndSelSegmentPos;
			if(anEndSelPos>endChar)
				anEndSelPos = endChar;
		}

		if(fullSelect)
		{
			g.ApplyColorSchemeColor(StandardColor_Hilight);
			g.FillRect(x,theInfo.mLineY,theWidth,theInfo.mLineHeight);
//			g.ApplyColorSchemeColor(StandardColor_HilightText);
//			g.DrawString(mText,startChar,endChar-startChar,x,y);
			return;
		}
		else if(aStartSelPos<anEndSelPos)
		{
			int aStartWidth = mFont->GetStringWidth(mText,startChar,aStartSelPos-startChar);
			int aSelWidth = mFont->GetStringWidth(mText,aStartSelPos,anEndSelPos-aStartSelPos);
//			g.SetColor(mColor);
//			g.DrawString(mText,startChar,aStartSelPos-startChar,x,y);
//			g.DrawString(mText,anEndSelPos,endChar-anEndSelPos,x+aStartWidth+aSelWidth,y);
			g.ApplyColorSchemeColor(StandardColor_Hilight);
			g.FillRect(x+aStartWidth,theInfo.mLineY,aSelWidth,theInfo.mLineHeight);
//			g.ApplyColorSchemeColor(StandardColor_HilightText);
//			g.DrawString(mText,aStartSelPos,anEndSelPos-aStartSelPos,x+aStartWidth,y);
			return;
		}
	}

//	g.SetColor(mColor);
//	g.DrawString(mText,startChar,endChar-startChar,x,y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextLinkParam::LinkDown()
{
	mLinkIsDown = true;
	for(LinkList::iterator anItr = mLinkList.begin(); anItr!=mLinkList.end(); ++anItr)
		(*anItr)->LinkDown();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TextLinkParam::LinkUp()
{
	if(mLinkIsDown)
	{
		mLinkIsDown = false;
		for(LinkList::iterator anItr = mLinkList.begin(); anItr!=mLinkList.end(); ++anItr)
			(*anItr)->LinkUp();

		return true;
	}
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextLinkLineSegment::LinkDown()
{
	mColor = mDownColor;
	mTableContext->Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextLinkLineSegment::LinkUp()
{
	if(mColor!=mNormalColor)
	{
		mColor = mNormalColor;
		mTableContext->Invalidate();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextLinkLineSegment::MouseEnter(int x, int y)
{
	mTableContext->SetCursor(Cursor::GetStandardCursor(StandardCursor_Hand));
	mTableContext->FireEvent(new HyperLinkEvent(mTableContext,ComponentEvent_HyperLinkOver,mLinkParam));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextLinkLineSegment::MouseDown(int x, int y, MouseButton theButton)
{
	if(theButton==MouseButton_Left)
		mLinkParam->LinkDown();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextLinkLineSegment::MouseUp(int x, int y, MouseButton theButton)
{
	if(theButton==MouseButton_Left)
	{
		if(mLinkParam->LinkUp())
			mTableContext->FireEvent(new HyperLinkEvent(mTableContext,ComponentEvent_HyperLinkActivated,mLinkParam));
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextLinkLineSegment::MouseExit()
{
	mLinkParam->LinkUp();
	mTableContext->FireEvent(new HyperLinkEvent(mTableContext,ComponentEvent_HyperLinkOver,NULL));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TextLinkParam* ImageMap::GetLink(int x, int y)
{
	for(ShapeList::iterator anItr = mShapeList.begin(); anItr!=mShapeList.end(); ++anItr)
	{
		ShapeLink &aLink = *anItr;
		if(aLink.mShape.Contains(x,y))
			return aLink.mLinkParam;
	}
	
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ImageLinkLineSegment::LinkDown()
{
	mBorderColor = mDownColor;
	mTableContext->Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ImageLinkLineSegment::LinkUp()
{
	if(mBorderColor!=mNormalColor)
	{
		mBorderColor = mNormalColor;
		mTableContext->Invalidate();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ImageLinkLineSegment::MouseEnter(int x, int y)
{
	if(mImageMap.get()!=NULL)
	{
		mCurLinkParam = NULL;
		MouseMove(x,y);
		return;
	}

	mTableContext->SetCursor(Cursor::GetStandardCursor(StandardCursor_Hand));
	mTableContext->FireEvent(new HyperLinkEvent(mTableContext,ComponentEvent_HyperLinkOver,mCurLinkParam));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ImageLinkLineSegment::MouseMove(int x, int y)
{
	if(mImageMap.get()==NULL)
		return;

	TextLinkParam *aParam = mImageMap->GetLink(x,y);
	if(aParam==NULL)
		aParam = mAnchorLinkParam;

	if(aParam!=mCurLinkParam)
	{
		mCurLinkParam = aParam;
		mTableContext->SetCursor(aParam!=NULL ? Cursor::GetStandardCursor(StandardCursor_Hand) : NULL);
		mTableContext->FireEvent(new HyperLinkEvent(mTableContext,ComponentEvent_HyperLinkOver,mCurLinkParam));
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ImageLinkLineSegment::MouseDown(int x, int y, MouseButton theButton)
{
	if(mCurLinkParam.get()==NULL)
		return;

	if(theButton==MouseButton_Left)
		mCurLinkParam->LinkDown();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ImageLinkLineSegment::MouseUp(int x, int y, MouseButton theButton)
{
	if(mCurLinkParam.get()==NULL)
		return;

	if(theButton==MouseButton_Left)
	{
		if(mCurLinkParam->LinkUp())
			mTableContext->FireEvent(new HyperLinkEvent(mTableContext,ComponentEvent_HyperLinkActivated,mCurLinkParam));
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ImageLinkLineSegment::MouseExit()
{
	if(mCurLinkParam.get()==NULL)
		return;

	mCurLinkParam->LinkUp();
	mTableContext->FireEvent(new HyperLinkEvent(mTableContext,ComponentEvent_HyperLinkOver,NULL));
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextLineSegment::MouseEnter(int x, int y)
{
	mTableContext->SetCursor(Cursor::GetStandardCursor(StandardCursor_IBeam));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int TextPieceLineSegment::GetSelChar(int x, int y)
{
	return mSegment->GetSelChar(x,y,mStartChar,mEndChar);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextPieceLineSegment::Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo)
{
	mSegment->Paint(g,x,y,mStartSelIndex,mStartChar,mEndChar,GetWidth(),GetHeight(),theInfo);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TextPieceLineSegment::PaintSelection(Graphics &g, int x, int y, ElementPaintInfo &theInfo)
{
	mSegment->PaintSelection(g,x,y,mStartSelIndex,mStartChar,mEndChar,GetWidth(),GetHeight(),theInfo);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ImageLineSegment::Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo)
{
	bool selected = DrawSelection1(g,x,y,theInfo);
	if(mBorder)
	{
		int aWidth = mImage->GetWidth()+2*mBorder;
		int aHeight = mImage->GetHeight()+2*mBorder;
		g.SetColor(mBorderColor);
		for(int i=0; i<mBorder; i++)
			g.DrawRect(x+mHSpace+i,y+mVSpace+i,aWidth-2*i,aHeight-2*i);
	}

	g.DrawImage(mImage,x+mHSpace+mBorder,y+mVSpace+mBorder);
	if(selected)
		DrawSelection2(g,x,y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ScaledImageLineSegment::Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo)
{
	g.PushClipRect(x,y,mWidth,mHeight);
	x+=mHSpace; y+=mVSpace;
	for(int w=0; w<mWidth; w+=mImage->GetWidth())
		for(int h=0; h<mHeight; h+=mImage->GetHeight())
			g.DrawImage(mImage,x+w,y+h);

	g.PopClipRect();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BulletElement::Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo)
{
	int aYPos = y+1;
	int anXPos = x;
	int aType;
	if(mType>=0)
	{
		anXPos -= mWidth+10;	
		aType = mType;
	}
	else
		aType = -mType;

	g.SetColor(mColor);
	switch(aType)
	{
		case 1: g.FillEllipse(anXPos,aYPos,mWidth,mHeight); break;
		case 2: g.DrawEllipse(anXPos,aYPos,mWidth+2,mHeight+2); break;
		case 3: g.FillRect(anXPos,aYPos,mWidth,mHeight); break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void OrderedListElement::Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo)
{
	g.SetColor(mColor);
	g.SetFont(mFont);
	int aBackWidth = mWidth + 10;
	if(aBackWidth>40)
		aBackWidth = 40;

	g.DrawString(mText,x-aBackWidth,y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LineBuilder::StartBuild(LineElement *theElement)
{
	mLine = theElement;
	mMaxAscent = mMaxDescent = mTopAlignHeight = mWidth = 0;
}	


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LineBuilder::AddSegment(LineSegment *theSegment)
{
	mLine->mSegmentList.push_back(theSegment);
	mWidth += theSegment->GetWidth();

	int anAscent;
	int aDescent;
	int aTopHeight;
	switch(theSegment->GetVertAlign())
	{
		case 0: // align with baseline
			anAscent = theSegment->GetAscent();
			aDescent = theSegment->GetHeight() - anAscent;
			break;

		case 1: // align middle
			anAscent = aDescent = theSegment->GetHeight()/2;
			break;

		case 2: // align top
			aTopHeight = theSegment->GetHeight();
			if(aTopHeight>mTopAlignHeight)
				mTopAlignHeight = aTopHeight;
			return;
	}

	if(anAscent > mMaxAscent)
		mMaxAscent = anAscent;

	if(aDescent > mMaxDescent)
		mMaxDescent = aDescent;

}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LineBuilder::FinishBuild()
{
	mLine->mWidth = mWidth;
	mLine->mHeight = GetHeight();
	mLine->mBaseline = mMaxAscent;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LineElement::LineElement()
{
	mBaseline = mWidth = mHeight = mLeftMargin = 0;
	mIsGlueLine = false;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LineSegment* LineElement::GetSegmentAt(int &x, int &y, bool lenient)
{
	if(x<mLeftMargin && !lenient)
		return this;

	int anXPos = mLeftMargin;
	SegmentList::iterator anItr = mSegmentList.begin();
	while(anItr!=mSegmentList.end())
	{
		LineSegment *aSeg = *anItr++;
		int aWidth = aSeg->GetWidth();
		if(anXPos + aWidth >= x || (lenient && anItr==mSegmentList.end()))
		{
			int aYPos = 0;
			int aHeight = aSeg->GetHeight();
			switch(aSeg->GetVertAlign())
			{
				case 0: aYPos = mBaseline-aSeg->GetAscent(); break;
				case 1: aYPos = mBaseline-aSeg->GetAscent()/2; break;
			}

			if((y>=aYPos && y<=aYPos+aHeight) || lenient)
			{
				x-=anXPos;
				y-=aYPos;
				return aSeg->GetSegmentAt(x, y, lenient);
			}
			else
				return this;
		}
		anXPos += aWidth;
	}

	return this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LineElement::SetViewport(ScrollAreaViewport *theViewport, int x, int y)
{
	LineSegment::SetViewport(theViewport,x,y);
	x+=mLeftMargin;
	SegmentList::iterator anItr = mSegmentList.begin();
	while(anItr!=mSegmentList.end())
	{
		LineSegment *aSeg = *anItr;
		int aYPos = y;
		switch(aSeg->GetVertAlign())
		{
			case 0: aYPos = y+mBaseline-aSeg->GetAscent(); break;
			case 1: aYPos = y+mBaseline-aSeg->GetAscent()/2; break;
			case 2: aYPos = y; break;
		}

		aSeg->SetViewport(theViewport,x,aYPos);
		x += aSeg->GetWidth();
		++anItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LineElement::SetSelIndex()
{
	LineSegment::SetSelIndex();
	SegmentList::iterator anItr = mSegmentList.begin();
	while(anItr!=mSegmentList.end())
	{
		(*anItr)->SetSelIndex();
		++anItr;
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int LineElement::GetEndSelIndex()
{
	if(!mSegmentList.empty())
		return mSegmentList.back()->GetEndSelIndex();
	else
		return mStartSelIndex;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LineElement::GetSelText(GUIString &theText, ElementSelectionInfo &theInfo)
{
	SegmentList::iterator anItr = mSegmentList.begin();
	while(anItr!=mSegmentList.end())
	{
		LineSegment *aSeg = *anItr;
		if(aSeg->mStartSelIndex>=theInfo.mStartSelSegment)
			break;

		++anItr;
	}

	while(anItr!=mSegmentList.end())
	{
		LineSegment *aSeg = *anItr;
		if(aSeg->mStartSelIndex<=theInfo.mEndSelSegment)
			aSeg->GetSelText(theText,theInfo);
		else
			break;			

		++anItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LineElement::GetSelCharPos(int &x, int &y, int theSelIndex, int theSelChar)
{
	SegmentList::iterator anItr = mSegmentList.begin();
	while(anItr!=mSegmentList.end())
	{
		LineSegment *aSeg = *anItr;
		if(aSeg->GetSelCharPos(x,y,theSelIndex,theSelChar))
			return true;

		++anItr;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool LineElement::FindStr(ElementFindInfo &theInfo)
{
	SegmentList::iterator anItr = mSegmentList.begin();
	while(anItr!=mSegmentList.end())
	{
		LineSegment *aSeg = *anItr;
		if(aSeg->mStartSelIndex>=theInfo.mStartSelSegment)
			break;

		++anItr;
	}

	while(anItr!=mSegmentList.end())
	{
		LineSegment *aSeg = *anItr;
		if(aSeg->mStartSelIndex<=theInfo.mEndSelSegment)
		{
			if(aSeg->FindStr(theInfo))
				return true;
		}
		else
			break;			

		++anItr;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LineElement::AddSegment(LineSegment *theSegment)
{
	mSegmentList.push_back(theSegment);
/*	mWidth += theSegment->GetWidth();
	int anAscent;
	int aDescent;
	switch(theSegment->GetVertAlign())
	{
		case 0: // align with baseline
			anAscent = theSegment->GetAscent();
			aDescent = theSegment->GetHeight() - anAscent;
			break;

		case 1: // align middle
			anAscent = aDescent = theSegment->GetHeight()/2;
			break;

		case 2: // align top
			anAscent = theSegment->GetHeight();
			if(anAscent>mMaxHeight)
				mMaxHeight = anAscent;
			return;
	}

	if(anAscent > mMaxAscent)
		mMaxAscent = anAscent;

	if(aDescent > mMaxDescent)
		mMaxDescent = aDescent;*/
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LineElement::Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo)
{
	SegmentList::iterator anItr = mSegmentList.begin();
	int oldY = theInfo.mLineY, oldHeight = theInfo.mLineHeight;

	if(!mIsGlueLine)
	{
		theInfo.mLineY = y; 
		theInfo.mLineHeight = mHeight;

		// Paint the whole line's selection before painiting
		if(!mSegmentList.empty())
		{
			int aStartSelIndex = mSegmentList.front()->mStartSelIndex;
//			int aStartSelIndex = mStartSelIndex; 
			if(theInfo.mEndSelSegment>=aStartSelIndex && theInfo.mStartSelSegment<=GetEndSelIndex())
				PaintSelection(g,x,y,theInfo);
		}
	}


	while(anItr!=mSegmentList.end())
	{
		LineSegment *aSeg = *anItr;
		switch(aSeg->GetVertAlign())
		{
			case 0: aSeg->Paint(g,x+mLeftMargin,y+mBaseline-aSeg->GetAscent(),theInfo); break;
			case 1: aSeg->Paint(g,x+mLeftMargin,y+mBaseline-aSeg->GetAscent()/2,theInfo); break;
			case 2: aSeg->Paint(g,x+mLeftMargin,y,theInfo); break;
		}
		x+=aSeg->GetWidth();
		++anItr;
	}

	theInfo.mLineY = oldY; theInfo.mLineHeight = oldHeight;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void LineElement::PaintSelection(Graphics &g, int x, int y, ElementPaintInfo &theInfo)
{
	SegmentList::iterator anItr = mSegmentList.begin();
	while(anItr!=mSegmentList.end())
	{
		LineSegment *aSeg = *anItr;
		switch(aSeg->GetVertAlign())
		{
			case 0: aSeg->PaintSelection(g,x+mLeftMargin,y+mBaseline-aSeg->GetAscent(),theInfo); break;
			case 1: aSeg->PaintSelection(g,x+mLeftMargin,y+mBaseline-aSeg->GetAscent()/2,theInfo); break;
			case 2: aSeg->PaintSelection(g,x+mLeftMargin,y,theInfo); break;
		}
		x+=aSeg->GetWidth();
		++anItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MultiLineBuilder::MultiLineBuilder()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiLineBuilder::SetIndentSize(int theLeftIndent, int theRightIndent)
{
	mLeftIndent = theLeftIndent;
	mRightIndent = theRightIndent;
	CalcMargins();
/*	if(mLeftMargin<mLeftIndent)
		mLeftMargin = mLeftIndent;
	if(mSpecifiedWidth-mRightMargin<mRightIndent)
		mRightMargin = mSpecifiedWidth-mRightIndent;*/
}
 
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiLineBuilder::AlignCurLine()
{
	if(!mLineVector->empty()) // Calc alignment
	{
		CurLine()->FinishBuild();
		LineElement *anElement = CurLine()->mLine;
		int aDelta = 0;
		switch(mAlignment)
		{
			case HorzAlign_Center: 
			{
				int anExtraWidth = GetCurMaxLineWidth() - anElement->GetWidth();
				aDelta = anExtraWidth>>1; 
				if((anExtraWidth&1) && (anElement->GetWidth()&1) && aDelta<anExtraWidth)
					aDelta++;
				
				break;
			}
			case HorzAlign_Right: aDelta = GetCurMaxLineWidth() - anElement->GetWidth(); break;//(mSpecifiedWidth - mRightMargin - mRightIndent - anElement->GetWidth()); break;
		}
		anElement->mLeftMargin = aDelta + mLeftMargin;//+ mLeftIndent;
		if(anElement->mLeftMargin<0)
			anElement->mLeftMargin = 0;

		if(anElement->GetWidth() + anElement->mLeftMargin > mWidth)
			mWidth = anElement->GetWidth() + anElement->mLeftMargin;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MultiLineBuilder::FlushDeferredItems()
{
	if(mHaveDeferredItems)
	{
		if(!mDeferredLeftItems.empty())
		{
			for(LineSegmentList::iterator anItr = mDeferredLeftItems.begin(); anItr!=mDeferredLeftItems.end(); ++anItr)
				AddFloatingSegment(*anItr,true);

			mDeferredLeftItems.clear();
		}
		if(!mDeferredRightItems.empty())
		{
			for(LineSegmentList::iterator anItr = mDeferredRightItems.begin(); anItr!=mDeferredRightItems.end(); ++anItr)
				AddFloatingSegment(*anItr,false);
			
			mDeferredRightItems.clear();
		}
		mHaveDeferredItems = false;
		return true;
	}
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LineElement* MultiLineBuilder::AddLine(bool clearLeft, bool clearRight)
{
	AlignCurLine();
	if(mHaveDeferredItems)
	{
		CurLine()->Clear();
		FlushDeferredItems();
	}

	if(clearLeft && mHeight<mMaxLeftHeight)
		mHeight = mMaxLeftHeight;

	if(clearRight && mHeight<mMaxRightHeight)
		mHeight = mMaxRightHeight;

	CalcMargins();
	LineElement *anElement = new LineElement;
	anElement->my = mHeight;
	anElement->mStartSelIndex = mCurSelIndex;

	mLineVector->push_back(anElement);
	mCurLine.StartBuild(anElement);

	return anElement;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiLineBuilder::AddSegmentToCurLine(LineSegment *theSegment)
{
	mCurSelIndex = theSegment->mStartSelIndex;

	int anOldHeight = CurLine()->GetHeight();
	CurLine()->AddSegment(theSegment);
//	int aWidth = CurLine()->GetWidth()+mLeftMargin+mRightMargin;
	int aWidth = CurLine()->GetWidth()+mLeftMargin;//+mLeftIndent;
	int aNewHeight = CurLine()->GetHeight();
	
	if(aWidth>mWidth)
	{
/*		int aLineWidth = CurLine()->GetWidth();
		if(mLeftMargin + aLineWidth > mSpecifiedWidth - mRightMargin) // eliminate overlap
			aWidth -= mLeftMargin+aLineWidth - (mSpecifiedWidth-mRightMargin);*/

		if(aWidth>mWidth)
			mWidth = aWidth;
	}

	if(aNewHeight!=anOldHeight)
		mHeight += aNewHeight-anOldHeight;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiLineBuilder::CalcMargins()
{
	FloatingList::iterator anItr;

	int aBottom;
	int aLeftMargin = 0;
	int aRightMargin = mSpecifiedWidth;
	for(anItr=mCurLeftItems.begin(); anItr!=mCurLeftItems.end(); )
	{
		FloatingSegment *aSegment = &*anItr;
		aBottom = aSegment->my + aSegment->GetHeight();
		if(aBottom > mHeight)
		{
			if(aSegment->mx + aSegment->GetWidth() > aLeftMargin)
				aLeftMargin = aSegment->mx + aSegment->GetWidth();

			if(aBottom>mMaxLeftHeight)
				mMaxLeftHeight = aBottom;

			++anItr;
		}
		else
			mCurLeftItems.erase(anItr++);
	}

	for(anItr=mCurRightItems.begin(); anItr!=mCurRightItems.end(); )
	{
		FloatingSegment *aSegment = &*anItr;
		aBottom = aSegment->my + aSegment->GetHeight();
		if(aBottom > mHeight)
		{
			if(aSegment->mx < aRightMargin)
				aRightMargin = aSegment->mx;

			if(aBottom>mMaxRightHeight)
				mMaxRightHeight = aBottom;

			++anItr;
		}
		else
			mCurRightItems.erase(anItr++);
	}

	if(aLeftMargin<mLeftIndent)
		aLeftMargin = mLeftIndent;
	if(mSpecifiedWidth-aRightMargin<mRightIndent)
		aRightMargin = mSpecifiedWidth-mRightIndent;

	mLeftMargin = aLeftMargin;
	mRightMargin = /*mSpecifiedWidth - */aRightMargin;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiLineBuilder::AddFloatingSegment(LineSegment *theSegment, bool left)
{
//	if(!CurLine()->IsEmpty())
//		AddLine(left,!left);

	if(!CurLine()->IsEmpty())
	{
		mHaveDeferredItems = true;
		if(left)
			mDeferredLeftItems.push_back(theSegment);
		else
			mDeferredRightItems.push_back(theSegment);

		return;
	}

	int aWidth = theSegment->GetWidth();
	if(left)
	{

		if(mLeftMargin+aWidth>mRightMargin)
			mLeftMargin = mRightMargin-aWidth;
		if(mLeftMargin<0)
			mLeftMargin = 0;

		if(mLeftMargin+aWidth>mWidth)
			mWidth = mLeftMargin+aWidth;

		mLeftItems->push_back(FloatingSegment(theSegment,mLeftMargin,mHeight));
		mCurLeftItems.push_back(mLeftItems->back());
	}
	else 
	{
		if(mWidth<mSpecifiedWidth)
			mWidth = mSpecifiedWidth;

		if(mRightMargin-aWidth<mLeftMargin)
		{
			mRightMargin = mLeftMargin+aWidth;
			if(mRightMargin>mWidth)
				mWidth = mRightMargin;
		}

		mRightItems->push_back(FloatingSegment(theSegment,mRightMargin - aWidth,mHeight));
		mCurRightItems.push_back(mRightItems->back());
	}

	CalcMargins();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiLineBuilder::AddSegments(SegmentList *theSegmentList, LineVector *theLineVector,
								   FloatingVector *theLeftItems, FloatingVector *theRightItems,
								   int theMaxWidth, int theStartSelIndex)
{
	mSegmentList = theSegmentList;
	mLineVector = theLineVector;
	mLeftItems = theLeftItems;
	mRightItems = theRightItems;
	mSpecifiedWidth = theMaxWidth;
	mWidth = mHeight = 0;
	mLeftMargin = mRightMargin = 0;
	mLeftIndent = mRightIndent = 0;
	mMaxLeftHeight =  mMaxRightHeight = 0;
	mAlignment = HorzAlign_Left;
	mHaveDeferredItems = false;

	mLeftItems->clear();
	mRightItems->clear();
	mLineVector->clear();

	mCurSelIndex = theStartSelIndex;
	AddLine();

	SegmentList::iterator anItr = mSegmentList->begin();
	while(anItr!=mSegmentList->end())
	{
		SegmentInfo &anInfo = *anItr;
		LineSegment *aSegment = anInfo.mSegment;
		unsigned short aFlags = anInfo.mFlags;

		aSegment->SetMaxWidth(theMaxWidth-mLeftIndent-mRightIndent);
		if(aFlags&LineSegmentFlag_PreLineBreak && (!CurLine()->IsEmpty() || 
			(aFlags&LineSegmentFlag_BreakClearLeft) || (aFlags&LineSegmentFlag_BreakClearRight)))
				AddLine(aFlags&LineSegmentFlag_BreakClearLeft?true:false,aFlags&LineSegmentFlag_BreakClearRight?true:false);

		if(aSegment->PerformInstructions(this))
		{
			if(aFlags&(LineSegmentFlag_FloatLeft | LineSegmentFlag_FloatRight))
				AddFloatingSegment(aSegment,aFlags&LineSegmentFlag_FloatLeft?true:false);
			else
			{
				if(CurLine()->GetWidth()+aSegment->GetWidth() > GetCurMaxLineWidth())
					aSegment->Wrap(this, CurLine()->IsEmpty());
				else
					AddSegmentToCurLine(aSegment);
			}
		}

		if(aFlags&LineSegmentFlag_PostLineBreak && (!CurLine()->IsEmpty() || 
			(aFlags&LineSegmentFlag_BreakClearLeft) || (aFlags&LineSegmentFlag_BreakClearRight)))
				AddLine(aFlags&LineSegmentFlag_BreakClearLeft?true:false,aFlags&LineSegmentFlag_BreakClearRight?true:false);
	
		++anItr;
	}

	AlignCurLine();
	if(mHaveDeferredItems)
	{
		CurLine()->Clear();
		FlushDeferredItems();
	}

	if(mHeight<mMaxLeftHeight)
		mHeight = mMaxLeftHeight;

	if(mHeight<mMaxRightHeight)
		mHeight = mMaxRightHeight;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MultiLineElement::MultiLineElement()
{
	mMinWidth = mMaxWidth = 0;
	mCurLineWidth = 0;
	mSearchLine = new LineElement;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LineSegment* MultiLineElement::GetSegmentAt(int &x, int &y, bool lenient) 
{ 
	LineVector::iterator anItr = FindLineAt(y);
	FloatingSegment *aFloat = NULL; 
	if(anItr!=mLineVector.end())
	{
		LineElement *aLine = *anItr;
		if(y>=aLine->my && y<aLine->my+aLine->GetHeight() && aLine->mLeftMargin<=x && aLine->mLeftMargin+aLine->mWidth>=x)
		{
			y-=aLine->my;
			return aLine->GetSegmentAt(x,y,lenient);
		}
		else if(aLine->mLeftMargin>x) // find left floating element
			aFloat = FindFloatingSegmentAt(x,y,true);
		else // find right floating element
			aFloat = FindFloatingSegmentAt(x,y,false);

		if(lenient && aFloat==NULL)
		{
			y-=aLine->my;
			return aLine->GetSegmentAt(x,y,lenient);
		}
	}
	else
	{
		aFloat = FindFloatingSegmentAt(x,y,true);
		if(aFloat==NULL)
			aFloat = FindFloatingSegmentAt(x,y,false);
	}

	if(aFloat==NULL)
		return this;

	x-=aFloat->mx;
	y-=aFloat->my;
	return aFloat->mSegment->GetSegmentAt(x,y,lenient);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiLineElement::SetViewport(ScrollAreaViewport *theViewport, int x, int y)
{
	LineSegment::SetViewport(theViewport,x,y);
	LineVector::iterator anItr = mLineVector.begin();
	while(anItr!=mLineVector.end())
	{
		LineElement *aLine = *anItr;
		aLine->SetViewport(theViewport,x,y+aLine->my);
		++anItr;
	}

	for(int i=0; i<2; i++)
	{
		for(FloatingVector::iterator anItr =  mFloatingItems[i].begin(); anItr!=mFloatingItems[i].end(); ++anItr)
		{
			FloatingSegment &aSeg = *anItr;
			aSeg.mSegment->SetViewport(theViewport,x + aSeg.mx, y + aSeg.my);
		}
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiLineElement::SetSelIndex()
{
	LineSegment::SetSelIndex();
	SegmentList::iterator anItr = mSegmentList.begin();
	while(anItr!=mSegmentList.end())
	{
		anItr->mSegment->SetSelIndex();
		++anItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int MultiLineElement::GetEndSelIndex()
{
	if(!mSegmentList.empty())
		return mSegmentList.back().mSegment->GetEndSelIndex();
	else
		return mStartSelIndex;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiLineElement::GetSelText(GUIString &theText, ElementSelectionInfo &theInfo)
{
	SegmentList::iterator anItr = mSegmentList.begin();
	while(anItr!=mSegmentList.end())
	{
		LineSegment *aSeg = anItr->mSegment;
		if(aSeg->GetEndSelIndex()>=theInfo.mStartSelSegment)
			break;

		++anItr;
	}

	bool hadText = false;
	while(anItr!=mSegmentList.end())
	{
		SegmentInfo &aSegInfo = *anItr;
		LineSegment *aSeg = aSegInfo.mSegment;
		if(aSeg->mStartSelIndex<=theInfo.mEndSelSegment)
		{
//			hadText = true;
			if((aSegInfo.mFlags&LineSegmentFlag_PreLineBreak) && hadText && aSeg->mStartSelIndex!=theInfo.mStartSelSegment)
			{
//				theText += "\r\n";
				hadText = false;
			}

			int anOldLen = theText.length();
			aSeg->GetSelText(theText,theInfo);
			hadText = theText.length()!=anOldLen;
//			hadText = true;
		
			if((aSegInfo.mFlags&LineSegmentFlag_PostLineBreak) && hadText && aSeg->mStartSelIndex!=theInfo.mEndSelSegment)
			{
//				theText += "\r\n";
				hadText = false;
			}
		}
		else
			break;			

		++anItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MultiLineElement::GetSelCharPos(int &x, int &y, int theSelIndex, int theSelChar)
{
	LineVector::iterator aLineItr = mLineVector.begin();
	while(aLineItr!=mLineVector.end())
	{
		LineElement *anElement = *aLineItr;
		if(anElement->GetSelCharPos(x,y,theSelIndex,theSelChar))
			return true;

		++aLineItr;
	}

	for(int i=0; i<2; i++)
	{
		FloatingVector::iterator anItr = mFloatingItems[i].begin();
		while(anItr!=mFloatingItems[i].end())
		{
			FloatingSegment &aSegment = *anItr;
			if(aSegment.mSegment->GetSelCharPos(x,y,theSelIndex,theSelChar))
				return true;

			++anItr;
		}
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MultiLineElement::FindStr(ElementFindInfo &theInfo)
{
	SegmentList::iterator anItr = mSegmentList.begin();
	while(anItr!=mSegmentList.end())
	{
		LineSegment *aSeg = anItr->mSegment;
		if(aSeg->GetEndSelIndex()>=theInfo.mStartSelSegment)
			break;

		++anItr;
	}

	bool found = false;
	SegmentList::iterator aStartFindItr = mSegmentList.end();
	ElementFindInfo aFoundInfo;

	while(anItr!=mSegmentList.end())
	{
		SegmentInfo &aSegInfo = *anItr;
		LineSegment *aSeg = aSegInfo.mSegment;
		if(aSeg->mStartSelIndex<=theInfo.mEndSelSegment)
		{
			if((aSegInfo.mFlags&LineSegmentFlag_PreLineBreak))
				theInfo.mFoundStartSegment = -1;

			if(aSeg->FindStr(theInfo))
			{
				found = true;
				aFoundInfo = theInfo;
				if(theInfo.mFindPrev)
				{
					if(aStartFindItr!=mSegmentList.end())
						anItr = aStartFindItr;

					theInfo.mStartSelSegment = theInfo.mFoundStartSegment;
					theInfo.mStartSelSegmentPos = theInfo.mFoundStartChar+1;
					theInfo.mCurFoundLength = 0;
					theInfo.mFoundStartSegment = -1;
					continue;
				}
				else
					break;
			}
			else if(theInfo.mCurFoundLength>0 && aStartFindItr==mSegmentList.end())
				aStartFindItr = anItr;
			else
				aStartFindItr = mSegmentList.end();


			if((aSegInfo.mFlags&LineSegmentFlag_PostLineBreak))
				theInfo.mFoundStartSegment = -1;
		}
		else
			break;			

		++anItr;
	}

	if(!found)
	{
		theInfo.mStartSelSegment = -1;
		return false;
	}
	else
	{
		theInfo = aFoundInfo;
		GetSelCharPos(theInfo.mFoundX,theInfo.mFoundY,theInfo.mFoundStartSegment,theInfo.mFoundStartChar);
		return true;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiLineElement::CalcMinMaxWidths()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int MultiLineElement::GetMinWidth()
{
	if(mMinWidth==0)
		CalcMinMaxWidths();

	return mMinWidth;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int MultiLineElement::GetMaxWidth()
{
	if(mMaxWidth==0)
		CalcMinMaxWidths();

	return mMaxWidth;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LineVector::iterator MultiLineElement::FindLineAt(int theYPos)
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
FloatingSegment* MultiLineElement::FindFloatingSegmentAt(int x, int y, bool left)
{
	FloatingVector *aVector = left?&mFloatingItems[0]:&mFloatingItems[1];
	FloatingVector::iterator anItr = std::lower_bound(aVector->begin(), aVector->end(), FloatingSegment(NULL,x,y), FloatSearchPred());
	if(anItr==aVector->end() || anItr->my>y)
	{
		if(anItr!=aVector->begin())
			--anItr;
	}

	if(anItr==aVector->end())
		return NULL;

	FloatingSegment &aSeg = *anItr;
	if(aSeg.mx<=x && aSeg.mx+aSeg.GetWidth()>=x && aSeg.my+aSeg.GetHeight()>=y)
		return &aSeg;
	else
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiLineElement::SetMaxWidth(int theMaxWidth)
{
	int aMinWidth = GetMinWidth();
//	if(theMaxWidth < aMinWidth)
//		theMaxWidth = aMinWidth;


	MultiLineBuilder aBuilder;
	aBuilder.AddSegments(&mSegmentList, &mLineVector, &mFloatingItems[0], &mFloatingItems[1], theMaxWidth, mStartSelIndex);

	if(aBuilder.mWidth > theMaxWidth)
		mWidth = aBuilder.mWidth;
	else
		mWidth = theMaxWidth;

	mHeight = aBuilder.mHeight;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiLineElement::AddSegment(LineSegment *theSegment, unsigned short theFlags)
{
	int aMinWidth = theSegment->GetMinWidth();
	if(aMinWidth>mMinWidth)
		mMinWidth = aMinWidth;

	if(theFlags & LineSegmentFlag_PreLineBreak)
		mCurLineWidth = 0;

	mCurLineWidth += theSegment->GetMaxWidth();
	if(mCurLineWidth>mMaxWidth)
		mMaxWidth = mCurLineWidth;

	if(theFlags & LineSegmentFlag_PostLineBreak)
		mCurLineWidth = 0;

//	mMaxWidth += aMaxWidth;

	mSegmentList.push_back(SegmentInfo(theSegment,theFlags));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MultiLineElement::IsLastLineEmpty()
{
	if(mSegmentList.empty())
		return true;
	else
		return (mSegmentList.back().mFlags & LineSegmentFlag_PostLineBreak)?true:false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiLineElement::Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo)
{
	int aTop = theInfo.ClipTop()-y;
	int aBottom = theInfo.ClipBottom()-y;
	if(aTop>mHeight || theInfo.ClipLeft()-x>mWidth || aBottom<0 || theInfo.ClipRight()-x<0)
		return;

	LineVector::iterator anItr;
	if(aTop>0)
		anItr = FindLineAt(aTop);
	else
		anItr = mLineVector.begin();

	while(anItr!=mLineVector.end())
	{
		LineElement *anElement = *anItr;
		if(anElement->my > aBottom)
			break;

		anElement->Paint(g,x,y+anElement->my,theInfo);
		++anItr;
	}

	FloatingVector::iterator aFloatItr;
	for(int i=0; i<2; i++)
	{
		for(aFloatItr=mFloatingItems[i].begin(); aFloatItr!=mFloatingItems[i].end(); ++aFloatItr)
		{
			FloatingSegment *aSegment = &*aFloatItr;
			theInfo.mLineY = y+aSegment->my;
			theInfo.mLineHeight = aSegment->GetHeight();
			aSegment->mSegment->Paint(g,x+aSegment->mx,y+aSegment->my,theInfo);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TableElement::TableElement()
{
	mCurCell = NULL;
	mCellSpacing = 2;
	mCellPadding = 1;
	mBorder = 0;
	mBGColor = -1;
	mMinWidth = mMaxWidth = 0;
	mWidthSpec = 0; 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TableElement::SetSize(int numRows, int numCols)
{
	mRowVector.clear();
	mColumnVector.clear();
	EnsureNumCols(numCols);
	EnsureNumRows(numRows);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TableElement::EnsureNumCols(int numCols)
{
	int aNumCols = mColumnVector.size();
	if(numCols<aNumCols)
		return;

	mColumnVector.resize(numCols);
	int i;
	for(i=aNumCols; i<numCols; i++)
		mColumnVector[i].mColumnNum = i;

	for(i=0; i<mRowVector.size(); i++)
		EnsureNumCols(mRowVector[i], numCols);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TableElement::EnsureNumCols(RowInfo &theRow, int numCols)
{
	int aNumCols = theRow.mRow.size();
	if(numCols < aNumCols)
		return;
	
	theRow.mRow.reserve(numCols);
	for(int i=aNumCols; i<numCols; i++)
		theRow.mRow.push_back(CellInfo());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TableElement::EnsureNumRows(int numRows)
{
	int aNumCols = mColumnVector.size();
	int aNumRows = mRowVector.size();
	int i;
	for(i=aNumRows; i<numRows; i++)
	{
		mRowVector.push_back(RowInfo(i));
		RowInfo &aRowInfo = mRowVector.back();
		EnsureNumCols(aRowInfo,aNumCols);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TableElement::CellInfo* TableElement::GetCellInfoAt(RowInfo &theInfo, int theCol)
{
	if(theCol<0 || theCol>=theInfo.mRow.size())
		return NULL;

	return &(theInfo.mRow[theCol]);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TableElement::CellInfo* TableElement::GetCellInfoAt(int theRow, int theCol)
{
	if(theRow<0 || theRow>=mRowVector.size() || theCol<0 || theCol>=mColumnVector.size())
		return NULL;
	
	RowInfo &anInfo = mRowVector[theRow];
	if(theCol>=anInfo.mRow.size())
		return NULL;

	return &(anInfo.mRow[theCol]);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MultiLineElement* TableElement::GetCellAt(int theRow, int theCol)
{
	CellInfo *anInfo = GetCellInfoAt(theRow, theCol);
	if(anInfo==NULL)
		return NULL;

	return anInfo->mContents;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MultiLineElement* TableElement::GetCellAt(RowInfo &theInfo, int theCol)
{
	CellInfo *anInfo = GetCellInfoAt(theInfo,theCol);
	if(anInfo==NULL)
		return NULL;
	else
		return anInfo->mContents;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TableElement::Rewind()
{
	mCurRow = mCurCol = 0;
	mCurCell = GetCellAt(mCurRow, mCurCol);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TableElement::StartTraversal()
{
	mCurRow = mCurCol = -1;
	mCurCell = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MultiLineElement* TableElement::NextCell()
{
	if(mCurRow >= mRowVector.size())
		return NULL;

	while(true)
	{
		if(mCurCol < mColumnVector.size()-1)
			mCurCol++;
		else
		{
			mCurRow++;
			mCurCol = 0;
		}
		if(mCurRow >= mRowVector.size())
			return NULL;

		mCurCell = GetCellAt(mCurRow, mCurCol);
		if(mCurCell!=NULL)
			return mCurCell;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TableElement::NextRow()
{
	mCurRow++;
	mCurCol = -1;
	EnsureNumRows(mCurRow+1);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
TableElement::CellInfo* TableElement::NextColumn()
{
	if(mCurRow<0)
		NextRow();

	RowInfo &aRowInfo = mRowVector[mCurRow];
	CellInfo *aCellInfo;
	do
	{
		mCurCol++;
		EnsureNumCols(mCurCol+1);
		aCellInfo = GetCellInfoAt(aRowInfo,mCurCol);
		mCurCell = aCellInfo->mContents;
	} while(mCurCell==NULL);

	return aCellInfo;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HorzAlign TableElement::GetCurRowAlign()
{
	RowInfo &aRowInfo = mRowVector[mCurRow];
	return (HorzAlign)aRowInfo.mHAlign;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TableElement::AddSegment(LineSegment *theSegment, unsigned short theFlags)
{
	if(mCurCell!=NULL)
	{
		mCurCell->AddSegment(theSegment, theFlags);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TableElement::SetCellSpan(int theRowSpan, int theColumnSpan, bool truncate)
{
	if(mCurCell!=NULL)
	{
		if(truncate)
		{
			if(mCurRow + theRowSpan > mRowVector.size())
				theRowSpan = mRowVector.size()-mCurRow;

			if(mCurCol + theColumnSpan > mColumnVector.size())
				theColumnSpan = mColumnVector.size()-mCurCol;
		}
		else
		{
			EnsureNumCols(mCurCol + theColumnSpan);
			EnsureNumRows(mCurRow + theRowSpan);
		}

		for(int i=0; i<theRowSpan; i++)
		{
			for(int j=0; j<theColumnSpan; j++)
			{
				CellInfo* anInfo = GetCellInfoAt(mCurRow+i,mCurCol+j);
				if(anInfo==NULL)
				{
					int x = 0; 
					x++;
				}
				if(i!=0 || j!=0)
				{
					anInfo->mContents = NULL;
					anInfo->mColumnSpan = -j;
					anInfo->mRowSpan = -i;
				}
			}
		}

		CellInfo *anInfo = GetCellInfoAt(mCurRow,mCurCol);
		anInfo->mColumnSpan = theColumnSpan;
		anInfo->mRowSpan = theRowSpan;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TableElement::SetCellBGColor(int theColor)
{
	if(mCurCell!=NULL)
	{
		CellInfo *anInfo = GetCellInfoAt(mCurRow,mCurCol);
		anInfo->mBGColor = theColor;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TableElement::SetRowBGColor(int theColor)
{
	if(mCurRow<0 || mCurRow>=mRowVector.size())
		return;

	mRowVector[mCurRow].mBGColor = theColor;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TableElement::SetRowAlign(int theHAlign, int theVAlign)
{
	if(mCurRow<0 || mCurRow>=mRowVector.size())
		return;

	RowInfo &anInfo = mRowVector[mCurRow];
	anInfo.mHAlign = theHAlign;
	anInfo.mVAlign = theVAlign;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TableElement::SetColumnWidth(int theWidthSpec)
{
	if(mCurCol<0 || mCurCol>=mColumnVector.size())
		return;
	
	ColumnInfo &anInfo = mColumnVector[mCurCol];
	if(theWidthSpec < anInfo.mWeight)
		anInfo.mWeight = theWidthSpec;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TableElement::SetCellAlign(int theHAlign, int theVAlign)
{
	if(mCurCell!=NULL)
	{
		CellInfo *anInfo = GetCellInfoAt(mCurRow,mCurCol);
		anInfo->mHAlign = theHAlign;
		anInfo->mVAlign = theVAlign;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int TableElement::FindRowAt(int theYPos)
{
	mSearchRow.my = theYPos;
	RowVector::iterator anItr = std::lower_bound(mRowVector.begin(), mRowVector.end(), mSearchRow, RowSearchPred());
	if(anItr==mRowVector.end() || anItr->my>theYPos)
	{
		if(anItr!=mRowVector.begin())
			--anItr;
	}

	return anItr - mRowVector.begin();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TableElement::GetCellInfoEx(CellInfoEx &theInfo, int theRow, int theCol, RowInfo &theRowInfo, bool seekUp, bool seekLeft)
{
	CellInfo *aCellInfo = GetCellInfoAt(theRowInfo,theCol);
	if(aCellInfo==NULL)
		return false;

	MultiLineElement *aCell = aCellInfo->mContents;
	if(aCell==NULL) 
	{
		// See if it's a cell which spans multiple rows or columns
		bool spanner = false;
		RowInfo *aRowInfo = &theRowInfo;
		if(seekUp && aCellInfo->mRowSpan<0)
		{
			theRow = theRow + aCellInfo->mRowSpan;
			aRowInfo = &mRowVector[theRow];
			spanner = true;
		}

		if(seekLeft && aCellInfo->mColumnSpan<0)
		{
			theCol = theCol + aCellInfo->mColumnSpan;
			spanner = true;
		}

		if(!spanner)
			return false;

		return GetCellInfoEx(theInfo,theRow,theCol,*aRowInfo);
	}

	ColumnInfo &aColumnInfo = mColumnVector[theCol];

	int cellx = aColumnInfo.mx;
	int celly = theRowInfo.my;
	int cellw = aColumnInfo.mWidth;
	int cellh = theRowInfo.mHeight;

	if(aCellInfo->mColumnSpan>1)
	{
		ColumnInfo &anEndColumn = mColumnVector[theCol + aCellInfo->mColumnSpan - 1];
		cellw = anEndColumn.mx + anEndColumn.mWidth - aColumnInfo.mx;
	}
	if(aCellInfo->mRowSpan>1)
	{
		RowInfo &anEndRow = mRowVector[theRow + aCellInfo->mRowSpan - 1];
		cellh = anEndRow.my + anEndRow.mHeight - theRowInfo.my;
	}

	char anHAlign = aCellInfo->mHAlign, aVAlign = aCellInfo->mVAlign;
	if(anHAlign==-1) anHAlign = theRowInfo.mHAlign;
	if(aVAlign==-1) aVAlign = theRowInfo.mVAlign;

	int dx, dy;
	int aPad = mCellPadding + (mBorder?2:0);
	switch(anHAlign)
	{
		case 1: dx = (cellw-aCell->GetWidth())/2; break;
		case 2: dx = cellw-aCell->GetWidth()-aPad; break;
		default: dx = aPad;
	}

	switch(aVAlign)
	{
		case 1: dy = (cellh-aCell->GetHeight())/2; break;
		case 2: dy = cellh-aCell->GetHeight()-aPad; break;
		default: dy = aPad;
	}


	if(dx<aPad)
		dx = aPad;
	if(dy<aPad)
		dy = aPad;

	theInfo.mCellInfo = aCellInfo;
	theInfo.mBorderX = cellx;
	theInfo.mBorderY = celly;
	theInfo.mBorderWidth = cellw;
	theInfo.mBorderHeight = cellh;
	theInfo.mCellX = cellx+dx;
	theInfo.mCellY = celly+dy;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void DrawShadedRect(Graphics &g, int x, int y, int width, int height, int size, bool up)
{
	g.Translate(x,y);
	width--;
	height--;
	int light = 0xd6d6d6;
	int dark = 0x848484;
	for(int i=0; i<size; i++)
	{
		g.SetColor(up?light:dark);
		g.DrawLine(i,i,width-i,i);
		g.DrawLine(i,i,i,height-i);
		g.SetColor(up?dark:light);
		g.DrawLine(width-i,i,width-i,height);
		g.DrawLine(i,height-i,width,height-i);
	}
	g.Translate(-x,-y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TableElement::PaintCell(Graphics &g, int x, int y, int theRow, int theCol, RowInfo &theRowInfo, ElementPaintInfo &thePaintInfo, bool seekUp)
{
	CellInfoEx aCellInfoEx;
	if(!GetCellInfoEx(aCellInfoEx,theRow,theCol,theRowInfo,seekUp))
		return;

	CellInfo *aCellInfo = aCellInfoEx.mCellInfo;
	MultiLineElement *aCell = aCellInfo->mContents;

	int aBGColor = aCellInfo->mBGColor;
	if(aBGColor==-1) aBGColor = theRowInfo.mBGColor;
	if(aBGColor>=0)
	{
		g.SetColor(aBGColor);
		g.FillRect(x + aCellInfoEx.mBorderX,y + aCellInfoEx.mBorderY,aCellInfoEx.mBorderWidth,aCellInfoEx.mBorderHeight);
	}

	if(mBorder)
		DrawShadedRect(g,x + aCellInfoEx.mBorderX,y + aCellInfoEx.mBorderY,aCellInfoEx.mBorderWidth,aCellInfoEx.mBorderHeight,1,false);

	aCell->Paint(g,x + aCellInfoEx.mCellX,y + aCellInfoEx.mCellY,thePaintInfo);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TableElement::Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo)
{
	if(mBGColor!=-1)
	{
		g.SetColor(mBGColor);
		g.FillRect(x,y,mWidth,mHeight);
	}
	DrawShadedRect(g,x,y,mWidth,mHeight,mBorder,true);

	int aNumColumns = mColumnVector.size();
	int aNumRows = mRowVector.size();
	int aRow, aCol;
	int aBottom = theInfo.ClipBottom() - y;

	
	int aTop = theInfo.ClipTop() - y;
	int aStartRow = 0;
	if(aTop > 0)
		aStartRow = FindRowAt(aTop);

	for(aRow=aStartRow; aRow<aNumRows; aRow++)
	{
		RowInfo &aRowInfo = mRowVector[aRow];
		if(aRowInfo.my > aBottom)
			break;

		for(aCol=0; aCol<aNumColumns; aCol++)
			PaintCell(g,x,y,aRow,aCol,aRowInfo,theInfo,aRow==aStartRow);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TableElement::CalcMinMaxWidths()
{
	mMinWidth = mMaxWidth = 0;
	int aNumColumns = mColumnVector.size();
	int aNumRows = mRowVector.size();
	if(aNumRows==0 || aNumColumns==0)
		return;	

	int aRow, aCol;
	int aPad = mCellPadding*2 + (mBorder?4:0);
	for(aCol=0; aCol<aNumColumns; aCol++)
		mColumnVector[aCol].mMinWidth = mColumnVector[aCol].mMaxWidth = 0;

	// first pass
	for(aCol=0; aCol<aNumColumns; aCol++) 
	{
		ColumnInfo &aColumnInfo = mColumnVector[aCol];
		int aMaxMinWidth = aColumnInfo.mMinWidth;
		int aMaxMaxWidth = aColumnInfo.mMaxWidth;
		for(aRow=0; aRow<aNumRows; aRow++)
		{
			CellInfo *aCellInfo = GetCellInfoAt(aRow,aCol);
			if(aCellInfo!=NULL && aCellInfo->mColumnSpan==1)
			{
				MultiLineElement *aCell = aCellInfo->mContents;
				if(aCell!=NULL)
				{
					int aMinWidth = aCell->GetMinWidth();
					int aMaxWidth = aCell->GetMaxWidth();
					if(aCellInfo->mNoWrap)
						aMinWidth = aMaxWidth; 

					if(aCellInfo->mWidth > 0)
					{
						if(aMinWidth < aCellInfo->mWidth)
							aMaxWidth = aCellInfo->mWidth;
						else
							aMaxWidth = aMinWidth;
					}

					if(aMinWidth > aMaxMinWidth)
						aMaxMinWidth = aMinWidth;

					if(aMaxWidth > aMaxMaxWidth)
						aMaxMaxWidth = aMaxWidth;					
				}
			}
		}

		aColumnInfo.mMinWidth = aMaxMinWidth + aPad;
		aColumnInfo.mMaxWidth = aMaxMaxWidth + aPad;
		mMinWidth += aColumnInfo.mMinWidth;
		mMaxWidth += aColumnInfo.mMaxWidth;
	}

	// second pass
	for(aCol=0; aCol<aNumColumns; aCol++) 
	{
		for(aRow=0; aRow<aNumRows; aRow++)
		{
			CellInfo *aCellInfo = GetCellInfoAt(aRow,aCol);
			if(aCellInfo!=NULL && aCellInfo->mColumnSpan>1)
			{
				MultiLineElement *aCell = aCellInfo->mContents;
				if(aCell!=NULL)
				{
					int aMinWidth = aCell->GetMinWidth();
					int aMaxWidth = aCell->GetMaxWidth();
					if(aCellInfo->mNoWrap)
						aMinWidth = aMaxWidth; 

					if(aCellInfo->mWidth > 0)
					{
						if(aMinWidth < aCellInfo->mWidth)
							aMaxWidth = aCellInfo->mWidth;
						else
							aMaxWidth = aMinWidth;
					}

					// Figure out how much MinWidth and MaxWidth is available from all the columns
					int i;
					int anAvailableMinWidth = 0;
					int aTotalMaxWidth = 0;
					for(i=0; i<aCellInfo->mColumnSpan; i++)
					{
						anAvailableMinWidth += mColumnVector[aCol+i].mMinWidth;
						aTotalMaxWidth += mColumnVector[aCol+i].mMaxWidth;
					}
					anAvailableMinWidth += (aCellInfo->mColumnSpan-1)*mCellSpacing - aPad;

					// Need to distribute more MinWidth to the columns
					int aNeededExtra = aMinWidth - anAvailableMinWidth;
					if(aNeededExtra>0)
					{
						int aPool = aNeededExtra;
						mMinWidth += aNeededExtra;
						for(i=0; i<aCellInfo->mColumnSpan; i++)
						{						
							ColumnInfo &aColumnInfo = mColumnVector[aCol+i];
							int anAmount;
							if(i!=aCellInfo->mColumnSpan-1)
							{
								if(aTotalMaxWidth>0)
									anAmount = (aMinWidth - anAvailableMinWidth)*aColumnInfo.mMaxWidth/aTotalMaxWidth;
								else
									anAmount = (aMinWidth - anAvailableMinWidth)/aCellInfo->mColumnSpan;

								aColumnInfo.mMinWidth += anAmount;
								aPool -= anAmount;
							}
							else
								aColumnInfo.mMinWidth += aPool;
						}
					}

					// Need to distribute more MaxWidth to the columns
					aNeededExtra = aMaxWidth - aTotalMaxWidth;
					if(aNeededExtra > 0)
					{
						int aPool = aNeededExtra;
						mMaxWidth += aNeededExtra;
						for(i=0; i<aCellInfo->mColumnSpan; i++)
						{
							ColumnInfo &aColumnInfo = mColumnVector[aCol+i];
							int anAmount;
							if(i!=aCellInfo->mColumnSpan-1)
							{
								if(aTotalMaxWidth>0)
									anAmount = (aMaxWidth - aTotalMaxWidth)*aColumnInfo.mMaxWidth/aTotalMaxWidth;
								else
									anAmount = (aMaxWidth - aTotalMaxWidth)/aCellInfo->mColumnSpan;

								aPool -= anAmount;
							}
							else
								anAmount = aPool;

							aColumnInfo.mMaxWidth += anAmount;
						}
					}

				}
			}
		}
	}


	mMinWidth += mCellSpacing*(aNumColumns+1) + 2*mBorder;
	mMaxWidth += mCellSpacing*(aNumColumns+1) + 2*mBorder;
	if(mWidthSpec>0)
	{
		if(mWidthSpec<mMinWidth)
			mWidthSpec = mMinWidth;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int TableElement::GetMinWidth() 
{ 
	if(mMinWidth==0)
		CalcMinMaxWidths();

	if(mWidthSpec>0)
		return mWidthSpec;
	else 
		return mMinWidth; 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int TableElement::GetMaxWidth() 
{ 
	if(mMaxWidth==0)
		CalcMinMaxWidths();

	if(mWidthSpec>0)
		return mWidthSpec;
	else
		return mMaxWidth; 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TableElement::CalcRowHeights()
{
	int aRow, aCol;
	int aNumRows = mRowVector.size(), aNumColumns = mColumnVector.size();

	// First pass
	mHeight = mBorder + mCellSpacing;
	for(aRow=0; aRow<aNumRows; aRow++)
	{
		RowInfo &aRowInfo = mRowVector[aRow];
		aRowInfo.my = mHeight;
		int aMaxHeight = 0;
		for(aCol=0; aCol<aNumColumns; aCol++)
		{
			CellInfo *aCellInfo = GetCellInfoAt(aRowInfo,aCol);
			MultiLineElement *aCell = aCellInfo->mContents;
			if(aCell!=NULL)
			{
				ColumnInfo &aColumnInfo = mColumnVector[aCol];
				int aColumnWidth = aColumnInfo.mWidth;
				if(aCellInfo->mColumnSpan>1)
				{
					ColumnInfo &anEndColumn = mColumnVector[aCol + aCellInfo->mColumnSpan - 1];
					aColumnWidth = anEndColumn.mx + anEndColumn.mWidth - aColumnInfo.mx;
				}

				aCell->SetMaxWidth(aColumnWidth - mCellPadding*2);
				if(aCellInfo->mRowSpan==1)
				{
					int aHeight = aCell->GetHeight();
					if(aCellInfo->mHeight>aHeight)
						aHeight = aCellInfo->mHeight;
					if(aHeight>aMaxHeight)
						aMaxHeight = aHeight;
				}
			}
		}
		aRowInfo.mHeight = aMaxHeight+mCellPadding*2 + (mBorder?4:0);
		mHeight += aRowInfo.mHeight + mCellSpacing;
	}
	mHeight+=mBorder;

	// Second pass
	mHeight = mBorder + mCellSpacing;
	for(aRow=0; aRow<aNumRows; aRow++)
	{
		RowInfo &aRowInfo = mRowVector[aRow];
		aRowInfo.my = mHeight;
		int aMaxHeight = aRowInfo.mHeight-mCellPadding*2;
		for(aCol=0; aCol<aNumColumns; aCol++)
		{
			CellInfo *aCellInfo = GetCellInfoAt(aRowInfo,aCol);
			MultiLineElement *aCell = aCellInfo->mContents;
			if(aCell!=NULL && aCellInfo->mRowSpan>1)
			{
				int aHeight = aCell->GetHeight();
				if(aCellInfo->mHeight>aHeight)
					aHeight = aCellInfo->mHeight;

				int i = 0;
				int anAvailableHeight = 0;
				for(i=0; i<aCellInfo->mRowSpan; i++)
					anAvailableHeight += mRowVector[aRow+i].mHeight;

				anAvailableHeight += mCellSpacing*(aCellInfo->mRowSpan-1);
				anAvailableHeight -= mCellPadding*2;
				
				if(aHeight > anAvailableHeight)
					aRowInfo.mHeight += aHeight - anAvailableHeight;
			}
		}
		mHeight += aRowInfo.mHeight + mCellSpacing;
	}

	mHeight+=mBorder;
/**/
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TableElement::CalcColumnWidths(int theWidth)
{
	int aNumColumns = mColumnVector.size();
	if(theWidth<mMinWidth)
		theWidth = mMinWidth;

	int aCol;
	int anExtraSpace = theWidth-mMinWidth, aColumnWeight = 0;
	int anOriginalExtraSpace = anExtraSpace;
	int anAvailableWidth = theWidth - 2*mBorder - (aNumColumns+1)*mCellSpacing;
	int aWidthForPercent = 0;
	int aTotalPercent = 0;
	int aNonPercentMaxWidth = mMaxWidth, aNonPercentMinWidth = mMinWidth;
	bool allColumnsPercent = true;
	for(aCol=0; aCol<aNumColumns; aCol++)
	{
		ColumnInfo &anInfo = mColumnVector[aCol];
		if(anInfo.mWeight < 0)
		{
			int aPercent = -anInfo.mWeight;
			anInfo.mWidth = anAvailableWidth*aPercent/100;
			if(anInfo.mWidth < anInfo.mMinWidth)
				anInfo.mWidth = anInfo.mMinWidth;
//			else
				aTotalPercent += aPercent;

			aWidthForPercent += anInfo.mWidth - anInfo.mMinWidth;
			aNonPercentMinWidth -= anInfo.mMinWidth - mCellSpacing;
			aNonPercentMaxWidth -= anInfo.mMaxWidth - mCellSpacing;
		}
		else
			allColumnsPercent = false;
	}

	if((aWidthForPercent > anExtraSpace) || (allColumnsPercent && aTotalPercent!=100))
	{
		int aLeftOver = anExtraSpace;
		ColumnInfo *aLeftOverColumn = NULL;
		for(aCol=0; aCol<aNumColumns; aCol++)
		{
			ColumnInfo &anInfo = mColumnVector[aCol];
			if(anInfo.mWeight<0 /*&& anInfo.mWidth>anInfo.mMinWidth*/)
			{
				int anAmount = anExtraSpace*-anInfo.mWeight/aTotalPercent;
				anInfo.mWidth = anInfo.mMinWidth + anAmount;
				aLeftOver -= anAmount;
				aLeftOverColumn = &anInfo;
			}
		}
		if(aLeftOverColumn!=NULL && aLeftOver>0)
			aLeftOverColumn->mWidth += aLeftOver;
		
		anExtraSpace = 0;
	}
	else 
		anExtraSpace -= aWidthForPercent;


	if(anExtraSpace>=aNonPercentMaxWidth-aNonPercentMinWidth)
	{
		if(mWidthSpec!=0)
			anExtraSpace -= aNonPercentMaxWidth-aNonPercentMinWidth;
		else
			anExtraSpace = 0;

		for(aCol=0; aCol<aNumColumns; aCol++)
		{
			ColumnInfo &anInfo = mColumnVector[aCol];
			if(anInfo.mWeight>=0)
			{
				anInfo.SetWidthToMax();
				anInfo.mWeight = anInfo.mMaxWidth;
				aColumnWeight += anInfo.mWeight;
			}
		}
	}
	else 
	{
	//	anExtraSpace = theWidth-aNonPercentMinWidth;
		for(aCol=0; aCol<aNumColumns; aCol++)
		{
			ColumnInfo &anInfo = mColumnVector[aCol];
			if(anInfo.mWeight>=0)
			{
				anInfo.SetWidthToMin();
				anInfo.mWeight = anInfo.mMaxWidth - anInfo.mMinWidth;
				aColumnWeight += anInfo.mWeight;
			}
		}
	}

	int aLeftOver = anExtraSpace;
	mWidth = mBorder + mCellSpacing;
	for(aCol=0; aCol<aNumColumns; aCol++)
	{
		ColumnInfo &aColInfo = mColumnVector[aCol];
		if(aColumnWeight>0 && aColInfo.mWeight>=0)
		{
			int anAmount = anExtraSpace*aColInfo.mWeight/aColumnWeight;
			aColInfo.mWidth += anAmount;
			aLeftOver -= anAmount;
		}

		aColInfo.mx = mWidth;
		mWidth += aColInfo.mWidth + mCellSpacing;
	}
	mWidth+=mBorder;
	if(aLeftOver>0 && aLeftOver!=anOriginalExtraSpace)
	{
		ColumnInfo &aColInfo = mColumnVector.back();
		aColInfo.mWidth += aLeftOver;
		mWidth += aLeftOver;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TableElement::SetMaxWidth(int theWidth)
{
	mWidth = 0;
	mHeight = 0;
		
	int aNumColumns = mColumnVector.size();
	int aNumRows = mRowVector.size();
	if(aNumRows==0 || aNumColumns==0)
		return;

	if(mMinWidth==0)
		CalcMinMaxWidths();

	if(mWidthSpec>0)
		theWidth = mWidthSpec;
	else if(mWidthSpec<0) // percentage
		theWidth = (theWidth*-mWidthSpec)/100;

	CalcColumnWidths(theWidth);
	CalcRowHeights();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TableElement::SetViewport(ScrollAreaViewport *theViewport, int x, int y)
{
	LineSegment::SetViewport(theViewport,x,y);
	int aNumColumns = mColumnVector.size();
	int aNumRows = mRowVector.size();
	int aRow, aCol;
	
	for(aRow=0; aRow<aNumRows; aRow++)
	{
		RowInfo &aRowInfo = mRowVector[aRow];
		for(aCol=0; aCol<aNumColumns; aCol++)
		{
			CellInfoEx aCellInfoEx;
			if(GetCellInfoEx(aCellInfoEx,aRow,aCol,aRowInfo))
			{
				MultiLineElement *aCell = aCellInfoEx.mCellInfo->mContents;
				aCell->SetViewport(theViewport,x+aCellInfoEx.mCellX,y+aCellInfoEx.mCellY);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LineSegment* TableElement::GetSegmentAt(int &x, int &y, bool lenient)
{

	int aNumColumns = mColumnVector.size();
	int aNumRows = mRowVector.size();

	int aRow = FindRowAt(y);
	if(aRow>=aNumRows)
		return this;

	RowInfo &aRowInfo = mRowVector[aRow];
	if(!lenient && (aRowInfo.my>y || aRowInfo.my+aRowInfo.mHeight<y))
		return this;

	CellInfoEx aCellInfoEx;
	for(int aCol=0; aCol<aNumColumns; aCol++)
	{
		ColumnInfo &aColumnInfo = mColumnVector[aCol];
		if(aColumnInfo.mx + aColumnInfo.mWidth > x || (lenient && aCol==aNumColumns-1))
		{
			if(GetCellInfoEx(aCellInfoEx,aRow,aCol,aRowInfo,true,true))
			{
				x-=aCellInfoEx.mCellX;
				y-=aCellInfoEx.mCellY;
				return aCellInfoEx.mCellInfo->mContents->GetSegmentAt(x,y,lenient);
			}
			else
				return this;
		}
	}
			
	return this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TableElement::SetSelIndex()
{
	LineSegment::SetSelIndex();
	int aNumColumns = mColumnVector.size();
	int aNumRows = mRowVector.size();
	int aRow, aCol;
	
	for(aRow=0; aRow<aNumRows; aRow++)
	{
		RowInfo &aRowInfo = mRowVector[aRow];
		for(aCol=0; aCol<aNumColumns; aCol++)
		{
			MultiLineElement *aCell = GetCellAt(aRowInfo,aCol);
			if(aCell!=NULL)
				aCell->SetSelIndex();
		}
	}
	mEndSelIndex = mCounter-1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void TableElement::GetSelText(GUIString &theText, ElementSelectionInfo &theInfo)
{

	int aNumColumns = mColumnVector.size();
	int aNumRows = mRowVector.size();
	int aRow, aCol;
	
	bool done = false;
	for(aRow=0; aRow<aNumRows; aRow++)
	{
		RowInfo &aRowInfo = mRowVector[aRow];
		for(aCol=0; aCol<aNumColumns; aCol++)
		{
			MultiLineElement *aCell = GetCellAt(aRowInfo,aCol);
			if(aCell!=NULL && aCell->GetEndSelIndex()>=theInfo.mStartSelSegment)
			{
				done = true; 
				break;
			}
		}

		if(done)
			break;
	}

	done = false;
	for(; aRow<aNumRows; aRow++)
	{
		RowInfo &aRowInfo = mRowVector[aRow];
		for(; aCol<aNumColumns && !done; aCol++)
		{
			MultiLineElement *aCell = GetCellAt(aRowInfo,aCol);
			if(aCell!=NULL)
			{
				if(aCell->mStartSelIndex>=theInfo.mEndSelSegment)
				{
					done = true;
					break;
				}
				else
				{
					aCell->GetSelText(theText,theInfo);
					theText.append(" ");
				}
			}
		}
		aCol = 0;
		if(!done)
			theText.append("\r\n");
		else
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool TableElement::FindStr(ElementFindInfo &theInfo)
{
	int aNumColumns = mColumnVector.size();
	int aNumRows = mRowVector.size();
	int aRow, aCol;
	
	bool done = false;
	for(aRow=0; aRow<aNumRows; aRow++)
	{
		RowInfo &aRowInfo = mRowVector[aRow];
		for(aCol=0; aCol<aNumColumns; aCol++)
		{
			MultiLineElement *aCell = GetCellAt(aRowInfo,aCol);
			if(aCell!=NULL && aCell->GetEndSelIndex()>=theInfo.mStartSelSegment)
			{
				done = true; 
				break;
			}
		}

		if(done)
			break;
	}

	done = false;
	for(; aRow<aNumRows; aRow++)
	{
		RowInfo &aRowInfo = mRowVector[aRow];
		for(; aCol<aNumColumns && !done; aCol++)
		{
			MultiLineElement *aCell = GetCellAt(aRowInfo,aCol);
			if(aCell!=NULL)
			{
				theInfo.mFoundStartSegment = -1;
				if(aCell->mStartSelIndex>=theInfo.mEndSelSegment)
					return false;
				else if(aCell->FindStr(theInfo))
						return true;
			}
		}
		aCol = 0;
	}

	theInfo.mFoundStartSegment = -1;
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Table::Table(LineSegment *theContents)
{
	mContents = theContents;
	SetBorderPadding(0,0,0,0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Table::CopySelection()
{
	GUIString aStr;
	if(mContents.get()!=NULL)
	{
		ElementSelectionInfo anInfo;
		anInfo.mStartSelSegment = mStartSelSegment;
		anInfo.mStartSelSegmentPos = mStartSelSegmentPos;
		anInfo.mEndSelSegment = mEndSelSegment;
		anInfo.mEndSelSegmentPos = mEndSelSegmentPos;
		mContents->GetSelText(aStr,anInfo);
	}

	if(!aStr.empty())
		Clipboard::Set(aStr);	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Table::KeyChar(int theKey)
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
bool Table::KeyDown(int theKey)
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

	AdjustPosition();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Table::MouseAt(int &x, int &y, bool mouseDown)
{
	LineSegment::mTableContext = this;
	if(mContents.get()!=NULL)
	{
		int oldx = x;
		int oldy = y;
		ScrollTranslate(x,y);
		LineSegment *aSeg = mContents->GetSegmentAt(x,y,mouseDown);
		if(mModalCount && aSeg!=NULL && !aSeg->AllowModalInput()) // fix for when combo box popup is up
			aSeg = NULL;

		if(aSeg!=mMouseSegment)
		{
			if(mMouseSegment!=NULL)
			{
				mMouseSegment->MouseExit();
				SetCursor(GetDefaultCursor());
			}

			mMouseSegment = aSeg;
			if(mMouseSegment!=NULL)
			{
				if(mouseDown && (x<0 || y<0 || x>mMouseSegment->GetWidth() || y>mMouseSegment->GetHeight()))
					return;

				mMouseSegment->MouseEnter(x,y);
			}
		}
		
		mMouseSegmentDX = x-oldx;
		mMouseSegmentDY = y-oldy;
	}
	else
		mMouseSegment = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
Component* Table::WMFindChildAt(int x, int y) 
{ 
	if(mContents.get()!=NULL)
	{
		LineSegment::mTableContext = this;
		ScrollTranslate(x,y);
		ComponentLineSegment *aSeg = dynamic_cast<ComponentLineSegment*>(mContents->GetSegmentAt(x,y,false));
		if(aSeg!=NULL)
			return aSeg->mComponent;
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Table::MouseEnter(int x, int y)
{
	Container::MouseEnter(x,y);
	MouseAt(x,y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Table::MouseExit()
{
	Container::MouseExit();
	mSelecting = false;
	if(mMouseSegment!=NULL)
	{
		mMouseSegment->MouseExit();
		mMouseSegment = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Table::MouseDown(int x, int y, MouseButton theButton)
{
	Container::MouseDown(x,y,theButton);
	MouseAt(x,y,true);
	if(mMouseSegment!=NULL)
	{	
		if(theButton==MouseButton_Left)
		{
			bool canStartSelection = mMouseSegment->CanStartSelection();
			if(!canStartSelection)
			{
				if(x<0 || y<0 || x>mMouseSegment->GetWidth() || y>mMouseSegment->GetHeight())
					canStartSelection = true;
			}

			if(canStartSelection && mMouseSegment->mStartSelIndex>=0)
			{
				if(mFocusChild!=NULL)
				{
					mFocusChild->LostFocus();
					mFocusChild = NULL;
				}

				mStartSelSegment = mEndSelSegment = mMouseSegment->mStartSelIndex;
				mStartSelSegmentPos = mEndSelSegmentPos = mMouseSegment->GetSelChar(x,y);
				mBackwardsSel = false;
				mMouseSegment->MouseExit();
				mMouseSegment = NULL;
				mSelecting = true;
				RequestTimer(true);
				Invalidate();
				return;
			}
		}

		mMouseSegment->MouseDown(x,y,theButton);
	}
	
	if(mStartSelSegment>=0)
	{
		mStartSelSegment = mEndSelSegment = -1;
		Invalidate();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Table::MouseUp(int x, int y, MouseButton theButton)
{
	Container::MouseUp(x,y,theButton);

	if(theButton==MouseButton_Left)
		mSelecting = false;

	MouseAt(x,y);
	if(mMouseSegment!=NULL)
	{
		mMouseSegment->MouseUp(x,y,theButton);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Table::MouseMove(int x, int y)
{
	Container::MouseMove(x,y);
	MouseAt(x,y);
	if(mMouseSegment!=NULL)
	{
		mMouseSegment->MouseMove(x,y);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Table::MouseDrag(int x, int y)
{
	Container::MouseDrag(x,y);
	if(mMouseSegment!=NULL)
	{
		LineSegment::mTableContext = this;
		x+=mMouseSegmentDX;
		y+=mMouseSegmentDY;
		mMouseSegment->MouseDrag(x,y);
	}
	else if(mStartSelSegment>=0)
	{
		ScrollTranslate(x,y);
		LineSegment *aSeg = mContents->GetSegmentAt(x,y,true);
		if(aSeg!=NULL && aSeg->mStartSelIndex>=0)
		{
			if(mBackwardsSel)
			{
				mStartSelSegment = aSeg->mStartSelIndex;
				mStartSelSegmentPos = aSeg->GetSelChar(x,y);
			}
			else
			{
				mEndSelSegment = aSeg->mStartSelIndex;
				mEndSelSegmentPos = aSeg->GetSelChar(x,y);
			}

			if(mEndSelSegment<mStartSelSegment || (mEndSelSegment==mStartSelSegment && mEndSelSegmentPos<mStartSelSegmentPos))
			{
				int temp = mStartSelSegment; mStartSelSegment = mEndSelSegment; mEndSelSegment = temp;
				temp = mStartSelSegmentPos; mStartSelSegmentPos = mEndSelSegmentPos; mEndSelSegmentPos = temp;
				mBackwardsSel = !mBackwardsSel;
			}
			Invalidate();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Table::TimerEvent(int theDelta)
{
	bool needTimer = ScrollArea::TimerEvent(theDelta);
	needTimer = needTimer || mSelecting;

	if(!mSelecting)
		return needTimer;

	int x,y;
	GetMousePos(x,y);
	if(x>=0 && x<=Width() && y>=0 && y<=Height())
		return true;

	if(y>Height())
	{
		int aDelta = y-Height();		
		mVertOffset+=aDelta;
	}
	else if(y<0)
	{
		int aDelta = -y;
		mVertOffset-=aDelta;
	}

	if(x>Width())
		mHorzOffset+=(x-Width());
	else if(x<0)
		mHorzOffset+=x;
		
	AdjustPosition();
	MouseDrag(x,y);

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Table::SetContents(LineSegment *theContents)
{
	mViewport->Reset(); // this will end any popup being done by the contents
	if(mContents.get()!=NULL)
	{
		LineSegment::mTableContext = this;
		LineSegment::mCounter = 0;
		mContents->SetViewport(NULL,0,0);  // this will set the parents to null of any component line segment
	}

	mContents = theContents;
	mMouseSegment = NULL;
	mStartSelSegment = mEndSelSegment = -1;
	mSelecting = false;
	SetMaxWidth();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Table::SetMaxWidth(bool adjustPosition)
{
	mMouseSegment = NULL;
	mAnchorMap.clear();
	if(mContents.get()!=NULL)
	{
		mContents->SetMaxWidth(mScrollArea.Width());

		LineSegment::mTableContext = this;
		LineSegment::mCounter = 0;
		mContents->SetViewport(GetViewport(),0,0);

		mTotalWidth = mContents->GetWidth();
		mTotalHeight = mContents->GetHeight();
	}
	else
	{
		mTotalWidth = mTotalHeight = 0;
	}
	
	if(adjustPosition)
		AdjustPosition();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Table::ScrollSizeChanged()
{
	SetMaxWidth(false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Table::ScrollPaint(Graphics &g)
{
	if(mContents.get()==NULL)
		return;

	ElementPaintInfo anInfo;
	anInfo.mClipX = mHorzOffset;
	anInfo.mClipY = mVertOffset;
	anInfo.mClipWidth = mScrollArea.Width();
	anInfo.mClipHeight = mScrollArea.Height();
	anInfo.mStartSelSegment = mStartSelSegment;
	anInfo.mEndSelSegment = mEndSelSegment;
	anInfo.mStartSelSegmentPos = mStartSelSegmentPos;
	anInfo.mEndSelSegmentPos = mEndSelSegmentPos;

	mContents->Paint(g,0,0,anInfo);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void Table::AddAnchor(const GUIString &theName, int x, int y)
{
	mAnchorMap[theName] = AnchorCoordinate(x,y);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Table::GotoAnchor(const GUIString &theName)
{
	AnchorMap::iterator anItr = mAnchorMap.find(theName);
	if(anItr==mAnchorMap.end())
		return false;

//	mHorzOffset = anItr->second.mx;
	mVertOffset = anItr->second.my;
	AdjustPosition();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool Table::FindStr(const GUIString &theStr, bool next, bool caseSensitive)
{
	if(mContents.get()==NULL)
		return false;

	ElementFindInfo anInfo;
	ElementFindInfo aFoundInfo;

	anInfo.mFindText = theStr;
	anInfo.mCaseSensitive = caseSensitive;
	anInfo.mFindPrev = !next;

	bool haveSel = mStartSelSegment>=0 && (mEndSelSegment>mStartSelSegment || mEndSelSegmentPos>mStartSelSegmentPos);
	if(next)
	{
		anInfo.mEndSelSegment = 0x7ffffff;
		anInfo.mEndSelSegmentPos = 0;
		if(!haveSel)
		{
			anInfo.mStartSelSegment = -1;
			anInfo.mStartSelSegmentPos = 0;
		}
		else
		{
			anInfo.mStartSelSegment = mStartSelSegment;
			anInfo.mStartSelSegmentPos = mStartSelSegmentPos+1;
		}
	}
	else // find Prev
	{
		anInfo.mStartSelSegment = -1;
		anInfo.mStartSelSegmentPos = 0;
		if(!haveSel)
		{
			anInfo.mEndSelSegment = 0x7ffffff;
			anInfo.mEndSelSegmentPos = 0;
		}
		else
		{
			anInfo.mEndSelSegment = mEndSelSegment;
			anInfo.mEndSelSegmentPos = mEndSelSegmentPos-1;
		}
	}

	anInfo.mCurFoundLength = 0;
	anInfo.mFoundStartSegment = -1;
	anInfo.mFoundX = 0;
	anInfo.mFoundY = 0;

	if(!mContents->FindStr(anInfo))
		return false;

	mStartSelSegment = anInfo.mFoundStartSegment;
	mEndSelSegment = anInfo.mFoundEndSegment;
	mStartSelSegmentPos = anInfo.mFoundStartChar;
	mEndSelSegmentPos = anInfo.mFoundEndChar;
	int horzOffset = GetHorzOffset();
	int vertOffset = GetVertOffset();
	if(anInfo.mFoundX > horzOffset + mScrollArea.width-20 || anInfo.mFoundX<horzOffset+20)
		horzOffset = anInfo.mFoundX;
	if(anInfo.mFoundY > vertOffset + mScrollArea.height-20 || anInfo.mFoundY<vertOffset+20)
		vertOffset = anInfo.mFoundY;

	SetOffsets(horzOffset,vertOffset);
	Invalidate();
	return true;
}
