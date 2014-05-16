#include "MSSound.h"
#include "WONSound.h"
#include "WONCommon/FileReader.h"

#include <Mmsystem.h>

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSSound* MSSound::mLastPlayedSound = NULL; 

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSSound::MSSound(const SoundDescriptor &theDesc) : Sound(theDesc)
{
	if (theDesc.IsPreload())
	{
		FileReader aSrc(theDesc.GetFilePath().c_str());
		if (aSrc.IsOpen())
		{
			aSrc.ReadIntoMemory();
			aSrc.SetOwnMemData(false);
			mSound = new ByteBuffer(aSrc.GetMemData(), aSrc.length(), true);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSSound::PlayHook()
{
	int aFlags = SND_ASYNC | SND_NODEFAULT;
	if(mIsLooping)
		aFlags |= SND_LOOP;

	if(mSound.get()!=NULL)
		PlaySound(mSound->data(), NULL, SND_MEMORY | aFlags);
	else if(!mDescriptor.GetFilePath().empty())
		PlaySound(mDescriptor.GetFilePath().c_str(), NULL, SND_FILENAME | aFlags);
	else
		return;

	mLastPlayedSound = this;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSSound::StopHook()
{
	// If the first parameter isn't null then, windows plays the sound even with the purge flag set.
	// Since PlaySound only plays one sound at once, it's no problem just to stop all the sounds

	if(mLastPlayedSound==this)
		PlaySound(NULL,NULL,SND_PURGE); 

	/*	if(mSound.get()!=NULL)
		PlaySound(mSound->data(), NULL, SND_MEMORY | SND_PURGE);
	else if(!mFilePath.empty())
		PlaySound(mFilePath.c_str(), NULL, SND_FILENAME | SND_PURGE);*/
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SoundPtr MSSoundDecoder::DoDecode(const SoundDescriptor &theDesc)
{
	return new MSSound(theDesc);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class MSSoundChannel : public WONSoundChannel
{
protected:
	HWAVEOUT		mWaveOut;
	WAVEHDR			*mWaveHeader; //[NUM_BUFFERS]

	static void CALLBACK waveOutProc(HWAVEOUT hwo,UINT uMsg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2);
	void waveOutProcPrv(HWAVEOUT hwo,UINT uMsg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2);
	bool CalcSettings();

	virtual bool OpenHook();
	virtual void SetBuffersHook();
	virtual void StopHook();
	virtual void PlaySoundsHook(int theBuf);
	virtual void SetVolumeHook(unsigned char theVolume);

	MSSoundChannel();
	virtual ~MSSoundChannel();

public:
	static WONSoundChannel* Factory() { return new MSSoundChannel; }
};
typedef SmartPtr<MSSoundChannel> MSSoundChannelPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSSoundChannel::MSSoundChannel()
{
	mWaveHeader = NULL;
	mWaveOut = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MSSoundChannel::~MSSoundChannel()
{
	if(mWaveOut!=NULL)
	{
		waveOutReset(mWaveOut);
		waveOutClose(mWaveOut);
	}

	if(mWaveHeader!=NULL)
		delete mWaveHeader;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CALLBACK MSSoundChannel::waveOutProc(HWAVEOUT hwo,UINT uMsg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2)
{
	MSSoundChannel *aChannel = (MSSoundChannel*)dwInstance;
	aChannel->waveOutProcPrv(hwo,uMsg,dwInstance,dwParam1,dwParam2);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSSoundChannel::waveOutProcPrv(HWAVEOUT hwo,UINT uMsg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2)
{
	if(uMsg==WOM_DONE)
	{
		for(int i=0; i<NUM_BUFFERS; i++)
		{
			if(dwParam1==(DWORD)&mWaveHeader[i])
			{
				mBufferFree[i] = true;
				break;
			}
		}

		WONSoundThread::WakeUp();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MSSoundChannel::CalcSettings()
{
	WAVEOUTCAPS aCaps;
	MMRESULT aResult = waveOutGetDevCaps(WAVE_MAPPER,&aCaps,sizeof(aCaps));
	if(aResult!=MMSYSERR_NOERROR)
		return false;

	
/*	int flags[12] = { WAVE_FORMAT_4S16, WAVE_FORMAT_4S08, WAVE_FORMAT_4M16, WAVE_FORMAT_4M08, 
					WAVE_FORMAT_2S16, WAVE_FORMAT_2S08, WAVE_FORMAT_2M16, WAVE_FORMAT_2M08,
					WAVE_FORMAT_1S16, WAVE_FORMAT_1S08, WAVE_FORMAT_1M16, WAVE_FORMAT_1M08 };*/

	int frequency[12] = {	44100, 22050, 11025, 44100,
							22050, 11025, 44100, 22050,
							11025, 44100, 22050, 11025 };

	bool stereo[12] = {	true, true, true, true,
						true, true, false, false,
						false, false, false, false };

	int numbytespersample[12] = {	2, 2, 2, 1,
									1, 1, 2, 2,
									2, 1, 1, 1 };

	for(int i=0; i<12; i++)
	{
		WAVEFORMATEX	wfx;
		wfx.wFormatTag = 1;		// PCM standard
		wfx.nChannels = stereo[i]?2:1;		
		wfx.nSamplesPerSec = frequency[i];
		wfx.nAvgBytesPerSec = frequency[i]*numbytespersample[i];
		wfx.nBlockAlign = numbytespersample[i];
		wfx.wBitsPerSample = numbytespersample[i]*8;
		wfx.cbSize = sizeof(wfx);

		aResult = waveOutOpen(NULL,WAVE_MAPPER,&wfx,0,0,WAVE_FORMAT_QUERY);
		if(aResult==MMSYSERR_NOERROR)
		{		
			mFrequency = frequency[i];
			mIsStereo = stereo[i];
			mNumBytesPerSample = numbytespersample[i];

			return true;
		}
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MSSoundChannel::OpenHook()
{
	MMRESULT aResult;

	if(!CalcSettings())
		return false;

	mSoundFlags = 0;
	if(mIsStereo)
		mSoundFlags |= WONSoundFlag_IsStereo;
	if(mNumBytesPerSample==2)
		mSoundFlags |= WONSoundFlag_IsSixteenBit;

	WAVEFORMATEX	wfx;
	wfx.wFormatTag = 1;		// PCM standard
	wfx.nChannels = mIsStereo?2:1;		
	wfx.nSamplesPerSec = mFrequency;
	wfx.nAvgBytesPerSec = mFrequency*mNumBytesPerSample;
	wfx.nBlockAlign = mNumBytesPerSample;
	wfx.wBitsPerSample = mNumBytesPerSample*8;
	wfx.cbSize = sizeof(wfx);
	aResult = waveOutOpen(			&mWaveOut,
									WAVE_MAPPER,
									&wfx,
									(DWORD)waveOutProc,
									(DWORD)this,					// User data.
									(DWORD)CALLBACK_FUNCTION);
//									(DWORD)NULL,					// User data.
//									(DWORD)CALLBACK_NULL);

	if (aResult!= MMSYSERR_NOERROR) 
		return false;


	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSSoundChannel::SetBuffersHook()
{
	mWaveHeader = new WAVEHDR[NUM_BUFFERS];

	mBufferSize = ((mBufferDuration * mFrequency) / 1000) * mNumBytesPerSample;
	
	if(mIsStereo)
		mBufferSize*=2;


	// Alloc the sample buffers.
	for (int i=0; i<NUM_BUFFERS; i++)
	{
		mSoundBuffer[i] = new char[mBufferSize];
		mWaveHeader[i].lpData = mSoundBuffer[i];
		mWaveHeader[i].dwBufferLength = mBufferSize;
		mWaveHeader[i].dwFlags = 0;
		mWaveHeader[i].dwLoops = 0;

		waveOutPrepareHeader(mWaveOut,&mWaveHeader[i],sizeof(WAVEHDR));
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSSoundChannel::SetVolumeHook(unsigned char theVolume)
{
	if(mWaveOut!=NULL)
	{
		unsigned long aVolume = theVolume<<8 | theVolume<<24;
		waveOutSetVolume(mWaveOut,aVolume);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSSoundChannel::StopHook()
{
	if(mWaveOut!=NULL)
		waveOutReset(mWaveOut);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MSSoundChannel::PlaySoundsHook(int theBuf)
{
	if (mWaveHeader[theBuf].dwFlags&WHDR_PREPARED)
		waveOutUnprepareHeader(mWaveOut,&mWaveHeader[theBuf],sizeof(WAVEHDR));

	mWaveHeader[theBuf].lpData = mSoundBuffer[theBuf];
	mWaveHeader[theBuf].dwBufferLength = mBufferSize;
	waveOutPrepareHeader(mWaveOut,&mWaveHeader[theBuf],sizeof(WAVEHDR));

	waveOutWrite(mWaveOut,&mWaveHeader[theBuf],sizeof(WAVEHDR));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::WONSound_InitMS()
{
	WONSoundThread::Init(MSSoundChannel::Factory);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONAPI::WONSound_DestroyMS()
{
	WONSoundThread::Destroy();
}

