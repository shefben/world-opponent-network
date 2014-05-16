#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

#include "PNGDecoder.h"
#include "../RawImage.h"

#ifdef WONPNG_USE_EXTERNAL
#include "pnglib1.0.5/png.h"
#define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)
#else
#include "png.h"
#endif

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PNGDecoder::Add(MultiImageDecoder *theDecoder)
{
	PNGDecoder *aDecoder = new PNGDecoder;
	theDecoder->AddDecoder(aDecoder,"png");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void png_filereader_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
   int check = ((FileReader*)(png_ptr->io_ptr))->ReadMaxBytes(data,length);
   if (check != length)
      png_error(png_ptr, "Read Error");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool PNGDecoder::DoGetAttrib(ImageAttributes &theAttrib)
{
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type;
	int png_transforms = 0;

	/* Create and initialize the png_struct with the desired error handler
	* functions.  If you want to use the default stderr and longjump method,
	* you can supply NULL for the last three parameters.  We also supply the
	* the compiler header file version, so that we know if the application
	* was compiled with a compatible version of the library.  REQUIRED
	*/
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL,
		(png_error_ptr)NULL, (png_error_ptr)NULL);

	if (png_ptr == NULL)
		return false;

	/* Allocate/initialize the memory for image information.  REQUIRED. */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
	  png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
	  return false;
	}

	/* Set error handling if you are using the setjmp/longjmp method (this is
	* the normal method of doing things with libpng).  REQUIRED unless you
	* set up your own error handlers in the png_create_read_struct() earlier.
	*/

	if (setjmp(png_jmpbuf(png_ptr)))
	{
	  /* Free all of the memory associated with the png_ptr and info_ptr */
	  png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
	  /* If we get here, we had a problem reading the file */
	  return false;
	}

	/* Set up the input control if you are using standard C streams */
//	png_init_io(png_ptr, mFileReader->GetFileHandle());
	png_set_read_fn(png_ptr,mFileReader,png_filereader_read_data);

	/*
	* If you have enough memory to read in the entire image at once,
	* and you need to specify only transforms that can be controlled
	* with one of the PNG_TRANSFORM_* bits (this presently excludes
	* dithering, filling, setting background, and doing gamma
	* adjustment), then you can read the entire image (including
	* pixels) into the info structure with this call:
	*/
   /* OK, you're doing it the hard way, with the lower-level functions */

   /* The call to png_read_info() gives us all of the information from the
    * PNG file before the first IDAT (image data chunk).  REQUIRED
    */
   png_read_info(png_ptr, info_ptr);

   png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
       &interlace_type, NULL, NULL);

   theAttrib.mWidth = width;
   theAttrib.mHeight = height;

	/* clean up after the read, and free any memory allocated - REQUIRED */
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

   return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RawImagePtr PNGDecoder::DoDecodeRaw()
{
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int sig_read = 0;
	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type;
	int png_transforms = 0;

	/* Create and initialize the png_struct with the desired error handler
	* functions.  If you want to use the default stderr and longjump method,
	* you can supply NULL for the last three parameters.  We also supply the
	* the compiler header file version, so that we know if the application
	* was compiled with a compatible version of the library.  REQUIRED
	*/
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)NULL,
		(png_error_ptr)NULL, (png_error_ptr)NULL);

	if (png_ptr == NULL)
	{
		return NULL;
	}

	/* Allocate/initialize the memory for image information.  REQUIRED. */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
	  png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
	  return NULL;
	}

	/* Set error handling if you are using the setjmp/longjmp method (this is
	* the normal method of doing things with libpng).  REQUIRED unless you
	* set up your own error handlers in the png_create_read_struct() earlier.
	*/

	if (setjmp(png_jmpbuf(png_ptr)))
	{
	  /* Free all of the memory associated with the png_ptr and info_ptr */
	  png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
	  /* If we get here, we had a problem reading the file */
	  return NULL;
	}

	/* Set up the input control if you are using standard C streams */
//	png_init_io(png_ptr, mFileReader->GetFileHandle());
	png_set_read_fn(png_ptr,mFileReader,png_filereader_read_data);

	/*
	* If you have enough memory to read in the entire image at once,
	* and you need to specify only transforms that can be controlled
	* with one of the PNG_TRANSFORM_* bits (this presently excludes
	* dithering, filling, setting background, and doing gamma
	* adjustment), then you can read the entire image (including
	* pixels) into the info structure with this call:
	*/
   /* OK, you're doing it the hard way, with the lower-level functions */

   /* The call to png_read_info() gives us all of the information from the
    * PNG file before the first IDAT (image data chunk).  REQUIRED
    */
   png_read_info(png_ptr, info_ptr);

   png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
       &interlace_type, NULL, NULL);

	/**** Set up the data transformations you want.  Note that these are all
	**** optional.  Only call them if you want/need them.  Many of the
	**** transformations only work on specific types of images, and many
	**** are mutually exclusive.
	****/

	/* tell libpng to strip 16 bit/color files down to 8 bits/color */
	png_set_strip_16(png_ptr);

	/* Extract multiple pixels with bit depths of 1, 2, and 4 from a single
	* byte into separate bytes (useful for paletted and grayscale images).
	*/
	png_set_packing(png_ptr);

	/* Expand paletted colors into true RGB triplets */
	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_expand(png_ptr);

	/* Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel */
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand(png_ptr);

	/* Expand paletted or RGB images with transparency to full alpha channels
	* so the data will be available as RGBA quartets.
	*/
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_expand(png_ptr);

	/*For some uses, you may want a grayscale image to be represented as
	RGB.  This code will do that conversion:*/
	int aRowSize = width;
    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
	{
//		png_set_gray_to_rgb(png_ptr);
	}
	else
	{
	/* Add filler (or alpha) byte (before/after each RGB triplet) */
		png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
		aRowSize *= 4;
	}


	png_bytepp row_pointers = new png_bytep[height];

	int x,y;
	int aRowBytes = png_get_rowbytes(png_ptr,info_ptr);
	for (y = 0; y < height; y++)
	{
		row_pointers[y] = new png_byte[aRowSize];
	}	

	/* Now it's time to read the image.  One of these methods is REQUIRED */
	png_read_image(png_ptr, row_pointers);


	RawImage32Ptr anImage = CreateRawImage32(width,height);
	if((color_type&PNG_COLOR_MASK_ALPHA) || png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		anImage->SetDoTransparency(true);

    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
	{
		anImage->SetDoTransparency(false);

		anImage->StartRowTraversal(0);
		for (y = 0; y < height; y++)
		{
			unsigned char *aPtr = row_pointers[y];
			for(x = 0; x < width; x++, aPtr++)
			{
				DWORD aColor = (aPtr[0]<<16) | (aPtr[0]<<8) | (aPtr[0]);
				anImage->SetRowPixel(aColor);
				anImage->NextRowPixel();
			}
			anImage->NextRow();
		}
	}
	else
	{
		anImage->StartRowTraversal(0);
		for (y = 0; y < height; y++)
		{
			unsigned char *aPtr = row_pointers[y];
			for(x = 0; x < width; x++, aPtr+=4)
			{
				DWORD aColor = (aPtr[0]<<16) | (aPtr[1]<<8) | (aPtr[2]) | (aPtr[3]<<24);
				anImage->SetRowPixel(aColor);
				anImage->NextRowPixel();
			}
			anImage->NextRow();
		}
	}

	for (y = 0; y < height; y++)
	{
		delete [] row_pointers[y];
	}	
	delete [] row_pointers;
	/* clean up after the read, and free any memory allocated - REQUIRED */
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

	return anImage.get();
}

