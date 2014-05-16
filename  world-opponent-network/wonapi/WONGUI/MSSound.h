#ifndef __WON_MSSOUND_H__
#define __WON_MSSOUND_H__

#include "Sound.h"
#include "SoundDecoder.h"
#include "WONCommon/ByteBuffer.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Microsoft Sound, plays a a wave file

class  MSSound : public WONAPI::Sound
{
protected:
	WONAPI::ByteBufferPtr mSound;

	virtual void PlayHook();
	virtual void StopHook();
	virtual void ResumeHook() { Play(); }
	virtual void Rewind() {}

	static MSSound* mLastPlayedSound; 

public:
	MSSound(const SoundDescriptor &theDesc);
	virtual ~MSSound() {}
};
typedef WONAPI::SmartPtr<MSSound> MSSoundPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSSoundDecoder : public SoundDecoder
{
public:
	virtual SoundPtr DoDecode(const SoundDescriptor &theDesc);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSound_InitMS();			// init MS implementation of WONSound
void WONSound_DestroyMS();		// destory MS implementation of WONSound



};	// namespace WONAPI

#endif