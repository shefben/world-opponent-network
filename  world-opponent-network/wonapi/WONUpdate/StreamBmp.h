//----------------------------------------------------------------------------------
// StreamBmp.h
//----------------------------------------------------------------------------------
#ifndef __StreamBmp_H__
#define __StreamBmp_H__
#include "WONCommon/WriteBuffer.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


//----------------------------------------------------------------------------------
// Prototypes.
//----------------------------------------------------------------------------------
WONAPI::ByteBufferPtr StreamBitmap(HDC hDC, HBITMAP hBmp, LPCSTR sFile);

#endif

