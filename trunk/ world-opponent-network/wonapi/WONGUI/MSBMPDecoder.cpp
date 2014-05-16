//#include "WONCommon/Platform.h"
//#ifdef WIN32_NOT_XBOX


#include "MSBMPDecoder.h"
#include "WONCommon/FileReader.h"
#include "MSNativeImage.h"
#include "MSDisplayContext.h"
#include "DisplayContext.h"
#include <io.h>

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static bool gUseMSDecoder = true;
void MSBMPDecoder::SetUseMSDecoder(bool useMS)
{
	gUseMSDecoder = useMS;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeImagePtr MSBMPDecoder::DoDecode(DisplayContext *theContext)
{
	if(!gUseMSDecoder)
		return BMPDecoder::DoDecode(theContext);

	try
	{
		// Read file header		
		BITMAPFILEHEADER aHeader;
		mFileReader->ReadBytes(&aHeader,sizeof(aHeader));
		if (aHeader.bfType != ((unsigned short) ('M' << 8) | 'B'))
			return NULL;

		int aSize = aHeader.bfSize - sizeof(aHeader);
		if(aSize<=0)
			return NULL;

		char *aData = new char[aSize];
		std::auto_ptr<char> aDelData(aData);

		mFileReader->ReadBytes(aData,aSize);
		BITMAPINFOHEADER &bmiHeader = *(LPBITMAPINFOHEADER)(aData);
		BITMAPINFO &bmInfo = *(LPBITMAPINFO)(aData);

		// If bmiHeader.biClrUsed is zero we have to infer the number
		// of colors from the number of bits used to specify it.
		int nColors = bmiHeader.biClrUsed ? bmiHeader.biClrUsed : 
							1 << bmiHeader.biBitCount;

		LPVOID lpDIBBits;
		if( bmInfo.bmiHeader.biBitCount > 8 )
			lpDIBBits = (LPVOID)((LPDWORD)(bmInfo.bmiColors + bmInfo.bmiHeader.biClrUsed) + 
				((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
		else
			lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);

		HBITMAP hBmp = CreateDIBitmap( ((MSDisplayContext*)theContext)->mDC,		// handle to device context 
					&bmiHeader,	// pointer to bitmap size and format data 
					CBM_INIT,	// initialization flag 
					lpDIBBits,	// pointer to initialization data 
					&bmInfo,	// pointer to bitmap color-format data 
					DIB_RGB_COLORS);		// color-data usage 

		MSNativeImagePtr anImage = new MSNativeImage(theContext, hBmp);	
//		if(mTransparentColor>=0)
//			anImage->SetTransparentColor(mTransparentColor);

		return anImage.get();
	}
	catch(FileReaderException&)
	{
	}

	return NULL;
}
//#endif WIN32_NOT_XBOX
