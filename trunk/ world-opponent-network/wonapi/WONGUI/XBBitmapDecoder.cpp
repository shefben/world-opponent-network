#include "BitmapDecoder.h"
#include "WONCommon/FileReader.h"
#include "NativeImage.h"
#include "DisplayContext.h"


using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeImagePtr BitmapDecoder::Decode(DisplayContext *theContext, const char *theFileWithPath)
{
	D3DTexture *theTexture;

    // Use D3DX to create a texture from a file based image
    if( FAILED( D3DXCreateTextureFromFileA( theContext->mDevice, theFileWithPath,
                                            &theTexture) ) )
	{
        return NULL;
    }
 
	HANDLE aFileHandle = CreateFile(theFileWithPath, GENERIC_READ, GENERIC_ALL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

	if (aFileHandle == INVALID_HANDLE_VALUE)
		return NULL;

	// Read in the width and height
	DWORD bytes_read =0;
	unsigned char buf[18];
	int testw = 640;
	int testh = 400;
	int width =0;
	int height =0;

	ReadFile(aFileHandle,buf,18,&bytes_read,NULL); // discarded bytes
	ReadFile(aFileHandle,&width,4,&bytes_read,NULL);
	ReadFile(aFileHandle,&height,4,&bytes_read,NULL);

	CloseHandle(aFileHandle);

	return new NativeImage(theContext, width, height, theTexture);	
}
