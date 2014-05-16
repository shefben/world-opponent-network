#ifndef __WON_MSCURSOR_H__
#define __WON_MSCURSOR_H__
#include "Cursor.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class  MSCursor : public Cursor
{
protected:
	virtual ~MSCursor() 
	{ 
		if(mDoDestroy) DestroyCursor(mNativeCursor);
	}

public:
	HCURSOR mNativeCursor;
	bool mDoDestroy;
	
	MSCursor(HCURSOR theCursor, bool doDestroy = false) : mNativeCursor(theCursor), mDoDestroy(doDestroy) { }

	virtual Cursor* GetNative() { return this; }
};
typedef WONAPI::SmartPtr<MSCursor> MSCursorPtr;

};	// namespace WONAPI

#endif