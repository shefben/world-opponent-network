#ifndef __WON_MP3DECODER_H__
#define __WON_MP3DECODER_H__

#include "WONGUI/SoundDecoder.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONMP3Decoder : public SoundDecoder
{
protected:
	virtual SoundPtr DoDecode(const SoundDescriptor &theDesc);

public:
	static void Add(MultiSoundDecoder *theDecoder);
};
typedef SmartPtr<WONMP3Decoder> WONMP3DecoderPtr;

} // namespace WONAPI

#endif