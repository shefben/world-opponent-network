#ifndef __WON_SOUNDDECODER_H__
#define __WON_SOUNDDECODER_H__


#include "Sound.h"
#include "WONCommon/StringUtil.h"

#include <map>

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SoundDecoder : public RefCount
{
protected:
	virtual SoundPtr DoDecode(const SoundDescriptor &theDesc) = 0;

public:
	SoundDecoder();
	virtual ~SoundDecoder();

	SoundPtr Decode(const SoundDescriptor &theDesc);
};
typedef SmartPtr<SoundDecoder> SoundDecoderPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MultiSoundDecoder : public SoundDecoder
{
protected:
	typedef std::map<std::string,SoundDecoderPtr,StringLessNoCase> DecoderMap;
	DecoderMap mDecoderMap;

	virtual SoundPtr DoDecode(const SoundDescriptor &theDesc);

public:
	void AddDecoder(SoundDecoder *theDecoder, const char *theExtension);

};
typedef SmartPtr<MultiSoundDecoder> MultiSoundDecoderPtr;


} // namespace WONAPI

#endif