#ifndef __WON_SOUND_H__
#define __WON_SOUND_H__
#include "WONCommon/SmartPtr.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum SoundFlags
{
	SoundFlag_Preload			=		0x0001,
	SoundFlag_Music				=		0x0002,
	SoundFlag_Muted				=		0x0004
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SoundDescriptor
{
public:
	int mSoundFlags;
	std::string mFilePath;

public:
	SoundDescriptor();
	SoundDescriptor(const char *thePath);
	SoundDescriptor(const std::string &thePath, int theFlags = 0);

	const std::string& GetFilePath() const { return mFilePath; } 

	bool SoundFlagSet(int theFlag) const { return mSoundFlags&theFlag?true:false; } 
	bool IsPreload() const	{ return mSoundFlags&SoundFlag_Preload?true:false; }
	bool IsMusic() const	{ return mSoundFlags&SoundFlag_Music?true:false; }
	bool IsMuted() const	{ return mSoundFlags&SoundFlag_Muted?true:false; }

	void SetSoundFlags(int theFlags, bool on);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class Sound : public RefCount
{
protected:
	SoundDescriptor mDescriptor;
	bool mIsLooping;
	
	virtual void PlayHook() {}
	virtual void StopHook() {}
	virtual void ResumeHook() {}
	virtual void RewindHook() {}

public:
	Sound(const SoundDescriptor &theDescriptor);

	void Play();
	void Stop();
	void Resume();
	void Rewind();

	void SetMuted(bool isMuted);
	bool GetMuted();
	bool IsLooping() { return mIsLooping; }
	void SetLooping(bool isLooping) { mIsLooping = isLooping; }

	const SoundDescriptor& GetDescriptor() { return mDescriptor; }
};
typedef SmartPtr<Sound> SoundPtr;

}; // namespace WONAPI

#endif