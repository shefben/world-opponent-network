#include "HexView.h"
#include "WONGUI/ColorScheme.h"

using namespace WONAPI;
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
HexView::HexView()
{
	mData = NULL;
	mDataLen = NULL;
	mFont = GetFont(FontDescriptor("Courier",FontStyle_Plain,8));

	mTextColor = -1;
	mNumBytesPerLine = 1;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HexView::SetData(const void *theData, unsigned long theDataLen)
{
	mBuf = NULL;
	mData = (const char*)theData;
	mDataLen = theDataLen;
	AdjustPosition();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HexView::SetData(const ByteBuffer *theData)
{
	mBuf = theData;
	if(theData!=NULL)
	{
		mData = theData->data();
		mDataLen = theData->length();
	}
	else
	{
		mData = NULL;
		mDataLen = 0;
	}
	AdjustPosition();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HexView::SetTextColor(int theColor)
{
	mTextColor = theColor;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HexView::SizeChanged()
{
	ScrollArea::SizeChanged();
	int aCharWidth = mFont->GetCharWidth('M');

	mNumBytesPerLine = (mScrollArea.Width()-11*aCharWidth)/(4*aCharWidth);
	if(mNumBytesPerLine<=0)
		mNumBytesPerLine = 1;


	AdjustPosition();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HexView::AdjustPosition(bool becauseOfScrollbar)
{
	int aNumLines = mDataLen/mNumBytesPerLine;
	if(mDataLen%mNumBytesPerLine!=0)
		aNumLines++;

	mTotalHeight = aNumLines*mFont->GetHeight();
	ScrollArea::AdjustPosition(becauseOfScrollbar);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static char GetHexChar(unsigned char theVal)
{
	if(theVal>=0 && theVal<=9)
		return '0' + theVal; 
	else
		return 'A' + theVal-10;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static char GetPrintChar(unsigned char theVal)
{
	if(isgraph(theVal))
		return theVal;
	else
		return '.';
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool HexView::PaintLine(Graphics &g, int theLine, int theYPos)
{
	int aBytePos = theLine*mNumBytesPerLine;
	int aNumBytes = mDataLen - aBytePos;
	if(aNumBytes<=0)
		return false;

	if(aNumBytes>mNumBytesPerLine)
		aNumBytes = mNumBytesPerLine;

	int i;
	int x = 0, y = theYPos;
	int aCharWidth = mFont->GetCharWidth('M');
	
	char aBuf[20];
	sprintf(aBuf,"%08X",aBytePos);
	g.DrawString(aBuf,x,y);
	x+=10*aCharWidth;

	GUIString aStr(mNumBytesPerLine*3);
	for(i=0; i<aNumBytes; i++)
	{
		unsigned char aByte = mData[aBytePos+i];
		aStr.append(GetHexChar((aByte>>4)&0x0f));
		aStr.append(GetHexChar(aByte&0x0f));
		aStr.append(' ');
	}

	g.DrawString(aStr,x,y);

	x += 3*mNumBytesPerLine*aCharWidth + aCharWidth;
	aStr.resize(0);
	for(i=0; i<aNumBytes; i++)
	{
		unsigned char aByte = mData[aBytePos+i];
		aStr.append(GetPrintChar(aByte));
	}

	g.DrawString(aStr,x,y);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void HexView::ScrollPaint(Graphics &g)
{
	int aLineNum = mVertOffset/mFont->GetHeight();
	int aYPos = mVertOffset - mVertOffset%mFont->GetHeight();

	if(mTextColor<0)
		g.ApplyColorSchemeColor(StandardColor_Text);
	else
		g.SetColor(mTextColor);
	g.SetFont(mFont);
	while(aYPos < mVertOffset + mScrollArea.Height())
	{
		if(!PaintLine(g,aLineNum,aYPos))
			break;
		
		aLineNum++;
		aYPos += mFont->GetHeight();
	}
}

