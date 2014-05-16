#ifndef __WON_XCURSOR_H__
#define __WON_XCURSOR_H__
#include "Cursor.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class  XCursor : public Cursor
{
protected:
	virtual ~XCursor() { }

public:
	::Cursor mNativeCursor;
	
	XCursor(::Cursor theCursor) : Cursor(NULL), mNativeCursor(theCursor) { }

	virtual Cursor* GetNative() { return this; }
};
typedef WONAPI::SmartPtr<XCursor> XCursorPtr;

};	// namespace WONAPI

#endif