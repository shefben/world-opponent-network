#ifndef __WON_XBDISPLAYCONTEXT_H__
#define __WON_XBDISPLAYCONTEXT_H__

#include "WONCommon/SmartPtr.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class DisplayContext : public WONAPI::RefCount
{
public:
	IDirect3DDevice8* mDevice;
	Graphics *mGraphics;

protected:
	virtual ~DisplayContext() { }

public:
	DisplayContext() : mDevice(NULL), mGraphics(NULL) { }
	DisplayContext(D3DDevice *c) : mDevice(c), mGraphics(NULL) { }
};
typedef WONAPI::SmartPtr<DisplayContext> DisplayContextPtr;

}; // namespace WONAPI

#endif