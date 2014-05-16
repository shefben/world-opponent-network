#include "MP3Decoder.h"
#include "WONGUI/WONSound.h"

extern "C"
{
#include "decoder.h"
}

using namespace WONAPI;

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MP3DataSource : public WONSoundDataBase
{
protected:
	~MP3DataSource();

public:
	mad_stream	Stream;
	mad_frame	Frame;
	mad_synth	Synth;
	bool mInited;

	enum { INPUT_BUFFER_SIZE = (5*8192) };
	enum { OUTPUT_BUFFER_SIZE = 5*8192 };

	unsigned char InputBuffer[INPUT_BUFFER_SIZE];
	unsigned char OutputBuffer[OUTPUT_BUFFER_SIZE];
	unsigned char *OutputPtr;
	const unsigned char	*OutputBufferEnd;

	MP3DataSource();

	int GetBytes(unsigned char *theBuf, int theAmount);
	virtual const char* GetDataPrv(int &theLen,bool synth);

	virtual const char* GetData(int &theLen) { return GetDataPrv(theLen,true); }
	virtual void Rewind();

	bool Init(int &freqOut, int &flagsOut);
};
typedef SmartPtr<MP3DataSource> MP3DataSourcePtr;

} // namespace WONAPI

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MP3DataSource::MP3DataSource()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MP3DataSource::~MP3DataSource()
{
	if(mInited)
	{
		mad_stream_finish(&Stream);
		mad_frame_finish(&Frame);
		mad_synth_finish(&Synth);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MP3DataSource::Init(int &freqOut, int &flagsOut)
{
	mInited = true;
	mad_stream_init(&Stream);
	mad_frame_init(&Frame);
	mad_synth_init(&Synth);

	OutputPtr=OutputBuffer;
	OutputBufferEnd=OutputBuffer+OUTPUT_BUFFER_SIZE;

	int aSize = GetBytes(InputBuffer,INPUT_BUFFER_SIZE);
	mad_stream_buffer(&Stream,InputBuffer,aSize);
	Stream.error=(mad_error)0;
	if (mad_header_decode(&Frame.header, &Stream) == -1) 
		return NULL;

	Rewind();

	

	freqOut = Frame.header.samplerate;
	flagsOut = WONSoundFlag_IsSixteenBit | WONSoundFlag_IsStereo;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int MP3DataSource::GetBytes(unsigned char *theBuf, int theAmount)
{
	return mReader.ReadMaxBytes(theBuf,theAmount);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MP3DataSource::Rewind()
{
	mReader.Rewind();
	mad_stream_buffer(&Stream,NULL,0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static unsigned short MadFixedToUshort(mad_fixed_t Fixed)
{
	/* A fixed point number is formed of the following bit pattern:
	 *
	 * SWWWFFFFFFFFFFFFFFFFFFFFFFFFFFFF
	 * MSB                          LSB
	 * S ==> Sign (0 is positive, 1 is negative)
	 * W ==> Whole part bits
	 * F ==> Fractional part bits
	 *
	 * This pattern contains MAD_F_FRACBITS, one should alway use this
	 * macro when working on the bits of a fixed point number. It is
	 * not guaranteed to be constant over the different platforms
	 * supported by libmad.
	 *
	 * The unsigned short value is formed by the least significant
	 * whole part bit, followed by the 15 most significant fractional
	 * part bits.
	 */
	Fixed=Fixed>>(MAD_F_FRACBITS-15);
	return((unsigned short)Fixed);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/*
 * NAME:	clip()
 * DESCRIPTION:	gather signal statistics while clipping
 */
static inline
void clip(mad_fixed_t *sample)
{
  enum {
    MIN = -MAD_F_ONE,
    MAX =  MAD_F_ONE - 1
  };

    if (*sample > MAX) 
      *sample = MAX;
    else if (*sample < MIN) 
      *sample = MIN;
}

/*
 * NAME:	audio_linear_round()
 * DESCRIPTION:	generic linear sample quantize routine
 */
static inline
signed long audio_linear_round(unsigned int bits, mad_fixed_t sample)
{
  /* round */
  sample += (1L << (MAD_F_FRACBITS - bits));

  /* clip */
  clip(&sample);

  /* quantize and scale */
  return sample >> (MAD_F_FRACBITS + 1 - bits);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const char* MP3DataSource::GetDataPrv(int &theLen, bool synth)
{
	OutputPtr=OutputBuffer;

	while(true)
	{
		if(Stream.buffer==NULL || Stream.error==MAD_ERROR_BUFLEN)
		{
			size_t			ReadSize,
							Remaining;
			unsigned char	*ReadStart;

			/* libmad does not consume all the buffer it's given. Some
			 * datas, part of a truncated frame, is left unused at the
			 * end of the buffer. Those datas must be put back at the
			 * beginning of the buffer and taken in account for
			 * refilling the buffer. This means that the input buffer
			 * must be large enough to hold a complete frame at the
			 * highest observable bit-rate (currently 448 kb/s). XXX=XXX
			 * Is 2016 bytes the size of the largest frame?
			 * (448000*(1152/32000))/8
			 */
			if(Stream.next_frame!=NULL)
			{
				Remaining=Stream.bufend-Stream.next_frame;
				memmove(InputBuffer,Stream.next_frame,Remaining);
				ReadStart=InputBuffer+Remaining;
				ReadSize=INPUT_BUFFER_SIZE-Remaining;
			}
			else
				ReadSize=INPUT_BUFFER_SIZE,
					ReadStart=InputBuffer,
					Remaining=0;
			
			/* Fill-in the buffer. If an error occurs print a message
			 * and leave the decoding loop. If the end of stream is
			 * reached we also leave the loop but the return status is
			 * left untouched.
			 */
			if(ReadSize>0)
			{
				ReadSize=GetBytes(ReadStart,ReadSize);
				if(ReadSize<=0)
					return NULL;
			}
			else
			{
				int x = 0;
				++x;
			}


			/* Pipe the new buffer content to libmad's stream decoder
			 * facility.
			 */
			mad_stream_buffer(&Stream,InputBuffer,ReadSize+Remaining);
			Stream.error=(mad_error)0;
		}

		/* Decode the next mpeg frame. The streams is read from the
		 * buffer, its constituents are break down and stored the the
		 * Frame structure, ready for examination/alteration or PCM
		 * synthesis. Decoding options are carried in the Frame
		 * structure from the Stream structure.
		 *
		 * Error handling: mad_frame_decode() returns a non zero value
		 * when an error occurs. The error condition can be checked in
		 * the error member of the Stream structure. A mad error is
		 * recoverable or fatal, the error status is checked with the
		 * MAD_RECOVERABLE macro.
		 *
		 * When a fatal error is encountered all decoding activities
		 * shall be stopped, except when a MAD_ERROR_BUFLEN is
		 * signaled. This condition means the mad_frame_decode()
		 * function needs more input to achieve it's work. One shall
		 * refill the buffer and repeat the mad_frame_decode() call.
		 *
		 * Recoverable errors are caused by malformed bit-streams in
		 * this case one can call mad_frame_decode() in order to skip
		 * the faulty frame and re-sync to the next frame.
		 */
		if(mad_frame_decode(&Frame,&Stream))
		{
			if(MAD_RECOVERABLE(Stream.error))
				continue;
			else
			{
				if(Stream.error==MAD_ERROR_BUFLEN)
					continue;
				else
					return NULL;
			}
		}

		if(!synth)
			return (const char*)OutputBuffer;

		/* The characteristics of the stream's first frame is printed
		 * on stderr. The first frame is representative of the entire
		 * stream.
		 */
/*		if(FrameCount==0)
		{
			if(PrintFrameInfo(stderr,&Frame.header))
			{
				Status=1;
				break;
			}
		}*/
				
		/* Once decoded the frame is synthesized to PCM samples. No errors
		 * are reported by mad_synth_frame();
		 */
		mad_synth_frame(&Synth,&Frame);

		/* Synthesized samples must be converted from mad's fixed
		 * point number to the consumer format. Here we use unsigned
		 * 16 bit big endian integers on two channels. Integer samples
		 * are temporarily stored in a buffer that is flushed when
		 * full.
		 */
		int i;
		for(i=0;i<Synth.pcm.length;i++)
		{
			unsigned short	Sample;

			/* Left channel */
//			Sample=MadFixedToUshort(Synth.pcm.samples[0][i]);
			Sample = audio_linear_round(16, Synth.pcm.samples[0][i]);
			*(OutputPtr++)=Sample&0xff;
			*(OutputPtr++)=Sample>>8;

			/* Right channel. If the decoded stream is monophonic then
			 * the right output channel is the same as the left one.
			 */
			if(MAD_NCHANNELS(&Frame.header)==2)
//				Sample=MadFixedToUshort(Synth.pcm.samples[1][i]);
				Sample = audio_linear_round(16, Synth.pcm.samples[1][i]);

			*(OutputPtr++)=Sample&0xff;
			*(OutputPtr++)=Sample>>8;

			/* Flush the buffer if it is full. */
/*			if(OutputPtr==OutputBufferEnd)
			{
				if(fwrite(OutputBuffer,1,OUTPUT_BUFFER_SIZE,OutputFp)!=OUTPUT_BUFFER_SIZE)
				{
					fprintf(stderr,"%s: PCM write error (%s).\n",
							ProgName,strerror(errno));
					Status=2;
					break;
				}
				OutputPtr=OutputBuffer;
			}*/
		}
		theLen = Synth.pcm.length*4;
		return (const char*)OutputBuffer;
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SoundPtr WONMP3Decoder::DoDecode(const SoundDescriptor &theDesc)
{
	MP3DataSourcePtr aData = new MP3DataSource;
	FileReader &aReader = aData->GetReader();
	if(!aReader.Open(theDesc.mFilePath.c_str()))
		return NULL;	

	int aFreq, aFlags;
	if(!aData->Init(aFreq,aFlags))
		return NULL;

	return new WONSound(aData,aFreq,aFlags,theDesc);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONMP3Decoder::Add(MultiSoundDecoder *theDecoder)
{
	theDecoder->AddDecoder(new WONMP3Decoder,"mp3");
}
