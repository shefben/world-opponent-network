#ifndef __WON_WONSOUND_H__
#define __WON_WONSOUND_H__

#include "WONCommon/CriticalSection.h"
#include "WONCommon/FileReader.h"
#include "WONCommon/Thread.h"
#include "WONGUI/SoundDecoder.h"

#include <set>

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum WONSoundFlags
{
	WONSoundFlag_IsSixteenBit			=		0x0001,
	WONSoundFlag_IsStereo				=		0x0002
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONSoundDataBase : public RefCount
{
protected:
	FileReader mReader;

	virtual ~WONSoundDataBase() { }

public:
	virtual const char* GetData(int &theLen) = 0;
	virtual void Rewind() = 0;

	FileReader& GetReader() { return mReader; }
};
typedef SmartPtr<WONSoundDataBase> WONSoundDataBasePtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONSoundData : public WONSoundDataBase
{
protected:
	int mDataPos;
	int mDataLen;

	enum { BUF_SIZE = 1024 };
	char mFileBuf[BUF_SIZE];

	virtual ~WONSoundData();

public:
	WONSoundData();
	virtual const char* GetData(int &theLen);
	virtual void Rewind();

	void SetDataPos(int thePos, int theLen);
};
typedef SmartPtr<WONSoundData> WONSoundDataPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONSound : public Sound
{
protected:
	WONSoundDataBasePtr mDataSource;
	const char *mCurData;
	int mCurDataLen;

	int mFrequency;
	int mSoundFlags;

	int mNumLoops;
	int mLoopCount;
	bool mIsPlaying;

	static void MixPrv(const char *buf1, int len1, int freq1, int flags1,
					char *buf2, int len2, int freq2, int flags2, int &numMixed1, int &numMixed2);

	void Init();
	void RewindPrv(bool resetLoopCount);

	virtual void PlayHook();
	virtual void ResumeHook();
	virtual void StopHook();
	virtual void RewindHook();

public:
	WONSound(WONSoundDataBase *theDataSource, int theFrequency, int theFlags, const SoundDescriptor &theDesc);

	void Mix(char *theBuf, int theNumBytes, int theFreq, int theFlags);

	bool IsPlaying() { return mIsPlaying; }
}; 
typedef SmartPtr<WONSound> WONSoundPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONSoundChannel : public RefCount
{
protected:
	typedef std::set<WONSoundPtr> SoundSet;
	SoundSet mSoundSet;
	bool mStopped;
	bool mIsMusic;

	int NUM_BUFFERS;
	char			**mSoundBuffer; //[NUM_BUFFERS];
	bool			*mBufferFree; //[NUM_BUFFERS];
	bool			*mBufferSilent;
	int mBufferDuration; // in milliseconds
	int mBufferSize;
	int mCurBuffer;
	int mFrequency;
	bool mIsStereo;
	int mNumBytesPerSample;
	int mSoundFlags;

	virtual bool OpenHook() = 0;
	virtual void StopHook() = 0;
	virtual bool NeedPlaySilence() { return false; }
	virtual void PlaySoundsHook(int theBuf) = 0;
	virtual void SetBuffersHook() = 0;
	virtual void SetVolumeHook(unsigned char theVolume) = 0;
	virtual void CheckFreeBuffers() { }

	bool PlaySounds(int theBuf);

	WONSoundChannel();
	virtual ~WONSoundChannel();

public:
	bool IsMusic() { return mIsMusic; }
	void SetBuffers(int theNumBuffers, int theBufferDuration);
	void SetVolume(unsigned char theVolume);

	void SetIsMusic();
	void Stop();
	bool Reset();
	bool Open();
	bool PlaySounds();
	void AddSound(WONSound *theSound);
	bool AllBuffersFree();
	bool AllBuffersFreeOrSilent();
	void ClearSilentFlags();

	bool HasSound(WONSound *theSound);
	int GetNumSounds();
};
typedef SmartPtr<WONSoundChannel> WONSoundChannelPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONSoundThread : public Thread
{
protected:
	CriticalSection mDataCrit;

	typedef std::set<WONSoundChannelPtr> ChannelSet;
	ChannelSet mFreeChannels;
	ChannelSet mBusyChannels;
	ChannelSet mPendingFreeChannels;

	bool mHaveMusicChannel;
	int mNumChannels;

	virtual void ThreadFunc();
	void AddSoundPrv(WONSound *theSound);
	void RemoveSoundPrv(WONSound *theSound);

	void PlaySounds();
	void StopSounds();

	bool PutSoundInChannel(WONSound *theSound, ChannelSet &theSet);
	
	typedef WONSoundChannel*(*ChannelFactory)();
	static ChannelFactory mChannelFactory;

	WONSoundThread();
	virtual ~WONSoundThread();



public:
	static void AddSound(WONSound *theSound);
	static void RemoveSound(WONSound *theSound);
	static void WakeUp();

	static void Init(ChannelFactory theFactory, int theMaxChannels = 2);
	static void Destroy();
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONWaveDecoder : public SoundDecoder
{
protected:
	virtual SoundPtr DoDecode(const SoundDescriptor &theDesc);

public:
	static void Add(MultiSoundDecoder *theDecoder);
};
typedef SmartPtr<WONWaveDecoder> WONWaveDecoderPtr;


};

#endif