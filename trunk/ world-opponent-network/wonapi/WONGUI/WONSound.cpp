#include "WONSound.h"

#include <assert.h>

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONSoundThread::ChannelFactory WONSoundThread::mChannelFactory = NULL;
static WONSoundThread *gWONSoundThread = NULL;
static int gMaxSoundChannels = 2;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONSoundData::WONSoundData()
{
	mDataPos = mDataLen = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONSoundData::~WONSoundData()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSoundData::SetDataPos(int thePos, int theLen)
{
	mDataPos = thePos;
	mDataLen = theLen;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const char* WONSoundData::GetData(int &theLen)
{
	if(!mReader.IsOpen())
		return NULL;

	int aBytesLeft = mDataPos + mDataLen - mReader.pos();
	if(aBytesLeft<=0)
		return NULL;

	if(mReader.GetMemData()!=NULL)
	{
		const char *aData = mReader.GetMemData() + mReader.pos();
		theLen = aBytesLeft;
		mReader.SkipBytes(theLen);
		return aData;
	}

	int aNumToRead = aBytesLeft>BUF_SIZE ? BUF_SIZE : aBytesLeft;
	theLen = mReader.ReadMaxBytes(mFileBuf,aNumToRead);
	return mFileBuf;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSoundData::Rewind()
{
	if(mDataLen==0)
		return;

	try
	{
		mReader.SetPos(mDataPos);
	}
	catch(FileReaderException&)
	{
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSound::Init()
{
	mNumLoops = 0;
	mLoopCount = 0;

	mCurData = NULL;
	mCurDataLen = 0;
	mIsPlaying = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONSound::WONSound(WONSoundDataBase* theDataSource, int theFrequency, int theFlags, const SoundDescriptor &theDesc) : 
	Sound(theDesc)
{
	Init();
	mFrequency = theFrequency;
	mSoundFlags = theFlags;
	mDataSource = theDataSource;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSound::PlayHook()
{
	if(mIsPlaying)
		Stop();

	Rewind();
	mNumLoops = mIsLooping?0:1;
	mIsPlaying = true;
	WONSoundThread::AddSound(this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSound::ResumeHook()
{
	mIsPlaying = true;
	WONSoundThread::AddSound(this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
inline void ClampAdd(short *theVal, int theAdd)
{
	int aVal = *theVal + theAdd;
	if(aVal>32767)
		aVal = 32767;
	if(aVal<-32768)
		aVal = -32768;
 
	*theVal = aVal;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
inline void ClampAdd(unsigned char *theVal, int theAdd)
{
	int aVal = *theVal + theAdd;
	if(aVal>255)
		aVal = 255;
	if(aVal<0)
		aVal = 0;

	*theVal = aVal;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSound::MixPrv(const char *buf1, int len1, int freq1, int flags1,
					char *buf2, int len2, int freq2, int flags2,
					int &numMixed1, int &numMixed2)
{
	int pos1 = 0;
	bool isStereo1 = flags1&WONSoundFlag_IsStereo?true:false;
	bool isSixteenBit1 = flags1&WONSoundFlag_IsSixteenBit?true:false;
	int sampSize1 = isSixteenBit1?2:1;
	if(isStereo1)
		sampSize1 *= 2;


	int pos2 = 0;
	bool isSixteenBit2 = flags2&WONSoundFlag_IsSixteenBit?true:false;
	bool isStereo2 = flags2&WONSoundFlag_IsStereo?true:false;
	int sampSize2 = isSixteenBit2?2:1;
	if(isStereo2)
		sampSize2 *= 2;

	int aLeftSample;
	int aRightSample;
	int aFreqCount = freq2-freq1;

	while(pos1+sampSize1<=len1 && pos2+sampSize2<=len2)
	{
		aFreqCount += freq1;
		if(aFreqCount >= freq2)
		{
			// Get Mix Data
			if(isSixteenBit1)
				aLeftSample = *(short*)(buf1 + pos1);
			else
				aLeftSample = ((*(unsigned char*)(buf1 + pos1) - 128) << 8);

			if(isStereo1)
			{
				if(isSixteenBit1)
					aRightSample = *(short*)(buf1 + pos1 + 2);
				else
					aRightSample = ((*(unsigned char*)(buf1 + pos1 + 1) - 128) << 8);
			}
			else
				aRightSample = aLeftSample;

			if(!isStereo2)
				aLeftSample = (aLeftSample + aRightSample)>>1;

			while(aFreqCount>=freq2 && pos1+sampSize1<=len1)
			{
				aFreqCount -= freq2;
				pos1+=sampSize1;
			}
		}

		// Put Mix Data
		if(isSixteenBit2)
			ClampAdd((short*)(buf2 + pos2),aLeftSample);
		else
			ClampAdd((unsigned char*)(buf2 + pos2),(aLeftSample>>8));

		if(isStereo2)
		{
			if(isSixteenBit2)
				ClampAdd((short*)(buf2 + pos2 + 2),aRightSample);
			else
				ClampAdd((unsigned char*)(buf2 + pos2 + 1),(aRightSample>>8));
		}
		pos2 += sampSize2;
	}

	numMixed1 = pos1;
	numMixed2 = pos2;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSound::Mix(char *theBuf, int theNumBytes, int theFreq, int theFlags)
{
	while(true)
	{
		if(mCurDataLen<=0)
		{
			mCurData = mDataSource->GetData(mCurDataLen);
			if(mCurData==NULL || mCurDataLen==0) // should we loop?
			{
				if(mNumLoops<=0 || ++mLoopCount<mNumLoops)
				{
					RewindPrv(false);
					continue;
				}
				else
				{
					mIsPlaying = false;
					break;
				}
			
			}
		}

		int mix1, mix2;
		MixPrv(mCurData, mCurDataLen, mFrequency, mSoundFlags,theBuf,theNumBytes,theFreq,theFlags,mix1,mix2);		
		mCurData += mix1;
		mCurDataLen -= mix1;
		if(mix2<theNumBytes)
		{
			theBuf+=mix2;
			theNumBytes-=mix2;
		}
		else
			break;
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSound::StopHook()
{
	if(mIsPlaying)
	{
		mIsPlaying = false;
		WONSoundThread::RemoveSound(this);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSound::RewindPrv(bool resetLoopCount)
{
	mCurData = NULL;
	mCurDataLen = 0;
	mDataSource->Rewind();

	if(resetLoopCount)
		mLoopCount = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSound::RewindHook()
{
	RewindPrv(true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONSoundChannel::WONSoundChannel()
{
	mBufferFree = NULL;
	mBufferSilent = NULL;
	mSoundBuffer = NULL;

	mIsMusic = false;
	mStopped = false;
	mIsStereo = false;
	NUM_BUFFERS = 0;
	mBufferDuration = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSoundChannel::SetIsMusic()
{
	mIsMusic = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONSoundChannel::~WONSoundChannel()
{
	delete [] mBufferFree;
	delete [] mBufferSilent;
	
	if(mSoundBuffer!=NULL)
	{
		for(int i=0; i<NUM_BUFFERS; i++)
			delete [] mSoundBuffer[i];

		delete [] mSoundBuffer;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONSoundChannel::Open()
{

	return OpenHook();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSoundChannel::SetBuffers(int theNumBuffers, int theBufferDuration)
{
	NUM_BUFFERS = theNumBuffers;
	mBufferDuration = theBufferDuration;

	mBufferFree = new bool[NUM_BUFFERS]; 
	mBufferSilent = new bool[NUM_BUFFERS];
	mSoundBuffer = new char*[NUM_BUFFERS];
	for(int i=0; i<NUM_BUFFERS; i++)
	{
		mBufferFree[i] = true;
		mBufferSilent[i] = false;
		mSoundBuffer[i] = NULL;
	}

	SetBuffersHook();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSoundChannel::SetVolume(unsigned char theVolume)
{
	SetVolumeHook(theVolume);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSoundChannel::Stop()
{
	mStopped = true;
	mSoundSet.clear();
	StopHook();

	for(int i=0; i<NUM_BUFFERS; i++)
	{
		mBufferFree[i] = true;
		mBufferSilent[i] = false;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONSoundChannel::Reset()
{
	CheckFreeBuffers();
	if(NeedPlaySilence())
	{
		if(AllBuffersFreeOrSilent())
		{
			Stop();
			mStopped = false; 
			return true;
		}

		for(int i=0; i<NUM_BUFFERS; i++)
		{
			if(mBufferFree[i])
			{
				if(mSoundFlags&WONSoundFlag_IsSixteenBit)
					memset(mSoundBuffer[i],0,mBufferSize);
				else
					memset(mSoundBuffer[i],128,mBufferSize);

				mBufferFree[i] = false;
				mBufferSilent[i] = true;
				PlaySoundsHook(i);
			}
		}
	}
	else
	{
		if(AllBuffersFree())
		{
			mSoundSet.clear();
			mStopped = false;
			return true;
		}
	}
		
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONSoundChannel::PlaySounds(int theBuf)
{
	// Send the buffer the the WaveOut queue
	SoundSet::iterator anItr = mSoundSet.begin();
	if(mSoundFlags&WONSoundFlag_IsSixteenBit)
		memset(mSoundBuffer[theBuf],0,mBufferSize);
	else
		memset(mSoundBuffer[theBuf],128,mBufferSize);

	bool playing = false;
	while(anItr!=mSoundSet.end())
	{
		WONSound *aSound = *anItr;
		if(!mStopped && aSound->IsPlaying())
		{
			playing = true;
			aSound->Mix(mSoundBuffer[theBuf],mBufferSize,mFrequency,mSoundFlags);
			++anItr;
		}
		else
			anItr = mSoundSet.erase(anItr);
	}

	if(playing)
	{
		mBufferFree[theBuf] = false;
		PlaySoundsHook(theBuf);
	}

	return playing;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONSoundChannel::PlaySounds()
{
	CheckFreeBuffers();

	bool playing = false;
	bool channelFree = false;
	for(int i=0; i<NUM_BUFFERS; i++)
	{
		if(mBufferFree[i])
		{
			channelFree = true;
			if(PlaySounds(i))
				playing = true;
		}
	}

	return playing || !channelFree;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSoundChannel::AddSound(WONSound *theSound)
{
	mSoundSet.insert(theSound);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONSoundChannel::AllBuffersFree()
{
	for(int i=0; i<NUM_BUFFERS; i++)
	{
		if(!mBufferFree[i])
			return false;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONSoundChannel::AllBuffersFreeOrSilent()
{
	for(int i=0; i<NUM_BUFFERS; i++)
	{
		if(!mBufferFree[i] && !mBufferSilent[i])
			return false;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSoundChannel::ClearSilentFlags()
{
	for(int i=0; i<NUM_BUFFERS; i++)
		mBufferSilent[i] = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONSoundChannel::HasSound(WONSound *theSound)
{
	return mSoundSet.find(theSound)!=mSoundSet.end();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int WONSoundChannel::GetNumSounds()
{
	return mSoundSet.size();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONSoundThread::WONSoundThread()
{
	int aNumChannels = gMaxSoundChannels;
	mHaveMusicChannel = false;
	mNumChannels = 0;
	for(int i=0; i<aNumChannels; i++)
	{
		WONSoundChannelPtr aChannel = mChannelFactory();
		if(aChannel->Open())
		{
			mNumChannels++;
			if(mNumChannels==2) // make the second channel a music channel
			{
				aChannel->SetIsMusic();
		//		aChannel->SetVolume(20);
				mHaveMusicChannel = true;
			}

			mFreeChannels.insert(aChannel);
		}
		else
			break;
	}


	for(ChannelSet::iterator anItr = mFreeChannels.begin(); anItr!=mFreeChannels.end(); ++anItr)
	{
		WONSoundChannel *aChannel = *anItr;
		if(mNumChannels==1)
		{
			aChannel->SetBuffers(2,200);
			SetPriority(ThreadPriority_High);
		}
		else if(aChannel->IsMusic())
			aChannel->SetBuffers(2,500); 
		else
			aChannel->SetBuffers(2,100); 
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONSoundThread::~WONSoundThread()
{
	Stop();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSoundThread::PlaySounds()
{
	AutoCrit aCrit(mDataCrit);
	ChannelSet::iterator anItr = mBusyChannels.begin();
	while(anItr!=mBusyChannels.end())
	{
		WONSoundChannel *aChannel = *anItr;

		if(aChannel->PlaySounds())
		{
			++anItr;
		}
		else
		{
			mPendingFreeChannels.insert(aChannel);
			anItr = mBusyChannels.erase(anItr);
		}
	}

	anItr = mPendingFreeChannels.begin();
	while(anItr!=mPendingFreeChannels.end())
	{
		WONSoundChannel *aChannel = *anItr;
		if(aChannel->Reset())
		{
			mFreeChannels.insert(aChannel);
			anItr = mPendingFreeChannels.erase(anItr);
		}
		else
			++anItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSoundThread::StopSounds()
{
	AutoCrit aCrit(mDataCrit);
	ChannelSet::iterator anItr = mBusyChannels.begin();
	while(anItr!=mBusyChannels.end())
	{
		WONSoundChannel *aChannel = *anItr;
		aChannel->Stop();
		++anItr;
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSoundThread::ThreadFunc()
{
	while(!mStopped)
	{
		mSignalEvent.WaitFor(50);
		if(mStopped)
			break;

		PlaySounds();
	}

	StopSounds();
	while(!mBusyChannels.empty())
	{
		mSignalEvent.WaitFor(50);
		PlaySounds();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONSoundThread::PutSoundInChannel(WONSound *theSound, ChannelSet &theSet)
{
	// Should already be in DataCrit
	bool isMusic = theSound->GetDescriptor().IsMusic();

//	if(isMusic && theSet==mBusyChannels) // can't add music to an already playing channel
//		return false;

	for(ChannelSet::iterator anItr = theSet.begin(); anItr!=theSet.end(); ++anItr)
	{
		WONSoundChannel *aChannel = *anItr;
		if(!mHaveMusicChannel || aChannel->IsMusic()==isMusic)
		{
			if(theSet!=mBusyChannels)
			{
				mBusyChannels.insert(aChannel);
				theSet.erase(aChannel);

				aChannel->ClearSilentFlags();
			}
			aChannel->AddSound(theSound);
			return true;
		}
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSoundThread::AddSoundPrv(WONSound *theSound)
{
	AutoCrit aCrit(mDataCrit);
	bool needSignal = false;
	if(PutSoundInChannel(theSound,mFreeChannels))
	{
		aCrit.Leave();
		Signal(); // wake up, we can play it right now
		return;
	}
	
	if(PutSoundInChannel(theSound,mPendingFreeChannels))
		return;
	
	if(PutSoundInChannel(theSound,mBusyChannels))
		return;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSoundThread::RemoveSoundPrv(WONSound *theSound)
{
	AutoCrit aCrit(mDataCrit);
	ChannelSet::iterator anItr = mBusyChannels.begin();
	while(anItr!=mBusyChannels.end())
	{
		WONSoundChannel *aChannel = *anItr;
		if(aChannel->HasSound(theSound))
		{
			if(aChannel->GetNumSounds()==1)
				aChannel->Stop();

			break;
		}
		
		++anItr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSoundThread::AddSound(WONSound *theSound)
{
	if(gWONSoundThread!=NULL)
		gWONSoundThread->AddSoundPrv(theSound);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSoundThread::RemoveSound(WONSound *theSound)
{
	if(gWONSoundThread !=NULL)
		gWONSoundThread->RemoveSoundPrv(theSound);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSoundThread::WakeUp()
{
	if(gWONSoundThread !=NULL)
		gWONSoundThread->Signal();

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSoundThread::Init(ChannelFactory theFactory, int theMaxChannels)
{
	assert(gWONSoundThread==NULL);

	gMaxSoundChannels = theMaxChannels;
	mChannelFactory = theFactory;
	gWONSoundThread = new WONSoundThread;
	gWONSoundThread->SetPriority(ThreadPriority_High);
	gWONSoundThread->Start();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONSoundThread::Destroy()
{
	if(gWONSoundThread!=NULL)
	{
		gWONSoundThread->Stop(true);
		delete gWONSoundThread;
		gWONSoundThread = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static int WONWaveSoundDecoder_FindChunk(FileReader &theReader, const char *theChunk, bool mustBeNext = false)
{
	char aChunkId[5];
	aChunkId[4] = '\0';

	while(true)
	{
		int aChunkSize = 0;
		theReader.ReadBytes(aChunkId,4);
		aChunkSize = theReader.ReadLong();
		if(strcmp(aChunkId,theChunk)==0)
			return aChunkSize;
		else if(mustBeNext)
			throw FileReaderException("Didn't find chunk.");

		theReader.SkipBytes(aChunkSize);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SoundPtr WONWaveDecoder::DoDecode(const SoundDescriptor &theDesc)
{
	WONSoundDataPtr aData = new WONSoundData;
	FileReader &aReader = aData->GetReader();
	if(!aReader.Open(theDesc.mFilePath.c_str()))
		return NULL;

	try
	{
		char aChunkId[5];
		aChunkId[4] = '\0';

		WONWaveSoundDecoder_FindChunk(aReader,"RIFF",true);

		aReader.ReadBytes(aChunkId,4);
		if(strcmp(aChunkId,"WAVE")!=0)
			return NULL;

		int aFmtSize = WONWaveSoundDecoder_FindChunk(aReader,"fmt ");

		int aFmtPos = aReader.pos();
		unsigned short aFormat = aReader.ReadShort();
		unsigned short aNumChannels = aReader.ReadShort();
		unsigned long aSampleRate = aReader.ReadLong();
		unsigned long aByteRate = aReader.ReadLong();
		unsigned short aBlockAlign = aReader.ReadShort();
		unsigned short aBitsPerSample = aReader.ReadShort();
		aReader.SetPos(aFmtPos+aFmtSize);

		int aDataLen = WONWaveSoundDecoder_FindChunk(aReader,"data");
		int aDataPos = aReader.pos();

		int aFlags = 0;
		if(aBitsPerSample==16)
			aFlags |= WONSoundFlag_IsSixteenBit;
		if(aNumChannels==2)
			aFlags |= WONSoundFlag_IsStereo;

		aData->SetDataPos(aDataPos,aDataLen);
		return new WONSound(aData,aSampleRate,aFlags,theDesc);
	}
	catch(FileReaderException&)
	{
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONWaveDecoder::Add(MultiSoundDecoder *theDecoder)
{
	theDecoder->AddDecoder(new WONWaveDecoder,"wav");
}
