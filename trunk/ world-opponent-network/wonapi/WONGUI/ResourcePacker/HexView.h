#ifndef __WON_HEXVIEW_H__
#define __WON_HEXVIEW_H__

#include "WONCommon/ByteBuffer.h"
#include "WONGUI/ScrollArea.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class HexView : public ScrollArea
{
protected:
	const char *mData;
	unsigned long mDataLen;
	ByteBufferPtr mBuf;
	FontPtr mFont;
	int mTextColor;

	int mNumBytesPerLine;

	bool PaintLine(Graphics &g, int theLineNum, int theYPos);

public:
	virtual void SizeChanged();
	virtual void ScrollPaint(Graphics &g);
	virtual void AdjustPosition(bool becauseOfScrollbar = false);

public:
	HexView();

	void SetData(const ByteBuffer *theData);
	void SetData(const void *theData, unsigned long theDataLen);
	void SetTextColor(int theColor);
};
typedef SmartPtr<HexView> HexViewPtr;

}; // namespace WONAPI;

#endif