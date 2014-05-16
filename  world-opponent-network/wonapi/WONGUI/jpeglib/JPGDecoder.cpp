#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

#include "JPGDecoder.h"
#include "../RawImage.h"
using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void JPGDecoder::Add(MultiImageDecoder *theDecoder)
{
	JPGDecoder *aDecoder = new JPGDecoder;
	theDecoder->AddDecoder(aDecoder,"jpg");
	theDecoder->AddDecoder(aDecoder,"jpe");
	theDecoder->AddDecoder(aDecoder,"jpeg");
	theDecoder->AddDecoder(aDecoder,"pjpeg");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
namespace WONJPG
{
extern "C"
{
#ifdef WONJPG_USE_EXTERNAL
#undef FAR // prevent compiler warning with far pointers
#define HAVE_BOOLEAN
typedef unsigned char boolean;
//#pragma pack(push,1) // ensure proper alignment on the mRefCount member
#include "jpeglib6a/jpeglib.h"
#include "jpeglib6a/jinclude.h"
#include "jpeglib6a/jerror.h"
//#pragma pack(pop)

#else

#include "jpeglib.h"
#include "jinclude.h"
#include "jerror.h"

#endif // WONJPG_USE_EXTERNAL

	struct my_error_mgr 
	{
		struct jpeg_error_mgr pub;	/* "public" fields */

		jmp_buf setjmp_buffer;	/* for return to caller */
	};

	typedef struct my_error_mgr * my_error_ptr;

	/*
	 * Here's the routine that will replace the standard error_exit method:
	 */

	METHODDEF(void)
	my_error_exit (j_common_ptr cinfo)
	{
	  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	  my_error_ptr myerr = (my_error_ptr) cinfo->err;

	  /* Always display the message. */
	  /* We could postpone this until after returning, if we chose. */
	  (*cinfo->err->output_message) (cinfo);

	  /* Return control to the setjmp point */
	  longjmp(myerr->setjmp_buffer, 1);
	}

	const int BUFFER_SIZE = 4096;

	typedef struct 
	{
	  struct jpeg_source_mgr pub;	/* public fields */
	  WONAPI::FileReader *mReader;
	  boolean start_of_file;	/* have we gotten any data yet? */
	  JOCTET buffer[BUFFER_SIZE];
	} my_source_mgr;
	typedef my_source_mgr * my_src_ptr;

	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	METHODDEF(void)
	init_source (j_decompress_ptr cinfo)
	{
	  my_src_ptr src = (my_src_ptr) cinfo->src;

	  /* We reset the empty-input-file flag for each image,
	   * but we don't clear the input buffer.
	   * This is correct behavior for reading a series of images from one source.
	   */
	  src->start_of_file = TRUE;
	}

	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	METHODDEF(boolean)
	fill_input_buffer (j_decompress_ptr cinfo)
	{
	  my_src_ptr src = (my_src_ptr) cinfo->src;
	  size_t nbytes;

	  nbytes = src->mReader->ReadMaxBytes(src->buffer,BUFFER_SIZE);
	  if (nbytes <= 0) {
		if (src->start_of_file)	/* Treat empty input file as fatal error */
		  ERREXIT(cinfo, JERR_INPUT_EMPTY);
		WARNMS(cinfo, JWRN_JPEG_EOF);
		/* Insert a fake EOI marker */
		src->buffer[0] = (JOCTET) 0xFF;
		src->buffer[1] = (JOCTET) JPEG_EOI;
		nbytes = 2;
	  }

	  src->pub.next_input_byte = src->buffer;
	  src->pub.bytes_in_buffer = nbytes;
	  src->start_of_file = FALSE;

	  return TRUE;
	}

	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	METHODDEF(void)
	skip_input_data (j_decompress_ptr cinfo, long num_bytes)
	{
	  my_src_ptr src = (my_src_ptr) cinfo->src;
	  if(num_bytes <= src->pub.bytes_in_buffer)
	  {
		  src->pub.bytes_in_buffer -= num_bytes;
		  src->pub.next_input_byte += num_bytes;
	  }
	  else
	  {
		  num_bytes -= src->pub.bytes_in_buffer;
		  src->pub.bytes_in_buffer = 0;
		  src->mReader->SkipBytes(num_bytes);
	  }
	}


	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	METHODDEF(void)
	term_source (j_decompress_ptr cinfo)
	{
	  /* no work necessary here */
	}

	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	GLOBAL(void)
	jpeg_filereader_src (j_decompress_ptr cinfo, FileReader *theReader)
	{
	  my_src_ptr src;

	  /* The source object and input buffer are made permanent so that a series
	   * of JPEG images can be read from the same file by calling jpeg_stdio_src
	   * only before the first one.  (If we discarded the buffer at the end of
	   * one image, we'd likely lose the start of the next one.)
	   * This makes it unsafe to use this manager and a different source
	   * manager serially with the same JPEG object.  Caveat programmer.
	   */
	  if (cinfo->src == NULL) {	/* first time for this JPEG object? */
		cinfo->src = (struct jpeg_source_mgr *)
		  (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
					  SIZEOF(my_source_mgr));
		src = (my_src_ptr) cinfo->src;
	  }

	  src = (my_src_ptr) cinfo->src;
	  src->mReader = theReader;
	  src->pub.init_source = init_source;
	  src->pub.fill_input_buffer = fill_input_buffer;
	  src->pub.skip_input_data = skip_input_data;
	  src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
	  src->pub.term_source = term_source;
	  src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
	  src->pub.next_input_byte = NULL; /* until buffer loaded */
	}

}
};

using namespace WONJPG;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool JPGDecoder::DoGetAttrib(ImageAttributes &theAttrib)
{
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if (setjmp(jerr.setjmp_buffer)) 
	{
		jpeg_destroy_decompress(&cinfo);
		return NULL;
	}

	jpeg_create_decompress(&cinfo);
	jpeg_filereader_src(&cinfo, mFileReader);
//	jpeg_stdio_src(&cinfo,mFileReader->GetFileHandle());

	(void) jpeg_read_header(&cinfo, TRUE);
	(void) jpeg_start_decompress(&cinfo);

	theAttrib.mWidth = cinfo.output_width;
	theAttrib.mHeight = cinfo.output_height;

	jpeg_destroy_decompress(&cinfo);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RawImagePtr JPGDecoder::DoDecodeRaw()
{
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;

	JSAMPARRAY buffer;		/* Output row buffer */
	int row_stride;		/* physical row width in output buffer */

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if (setjmp(jerr.setjmp_buffer)) 
	{
		jpeg_destroy_decompress(&cinfo);
		return NULL;
	}

	jpeg_create_decompress(&cinfo);
	jpeg_filereader_src(&cinfo, mFileReader);
//	jpeg_stdio_src(&cinfo,mFileReader->GetFileHandle());

	(void) jpeg_read_header(&cinfo, TRUE);
	(void) jpeg_start_decompress(&cinfo);

	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	int aWidth = cinfo.output_width;
	int aHeight = cinfo.output_height;
	RawImage32Ptr anImage = CreateRawImage32(aWidth,aHeight);
	anImage->StartRowTraversal(0);

	while (cinfo.output_scanline < cinfo.output_height) 
	{
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);
		unsigned char *aPtr = buffer[0];
		if(cinfo.out_color_space==JCS_GRAYSCALE)
		{
			for(int x = 0; x < aWidth; x++, aPtr++)
			{
				DWORD aColor = aPtr[0] | (aPtr[0]<<8) | (aPtr[0]<<16);
				anImage->SetRowPixel(aColor);
				anImage->NextRowPixel();
			}
		}
		else
		{
			for(int x = 0; x < aWidth; x++, aPtr+=3)
			{
				DWORD aColor = aPtr[2] | (aPtr[1]<<8) | (aPtr[0]<<16);
				anImage->SetRowPixel(aColor);
				anImage->NextRowPixel();
			}
		}
		anImage->NextRow();
//		put_scanline_someplace(buffer[0], row_stride);
	}

	(void) jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	return anImage.get();
}
