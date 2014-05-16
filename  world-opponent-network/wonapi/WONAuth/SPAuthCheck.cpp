#define __WON_MASTER_CPP__
#include "SPAuthCheck.h"

#include "WONAPI.h"
#include "WONAuth/AuthContext.h"
#include "WONAuth/GetCertOp.h"
#include "WONCrypt/Blowfish.h"
#include "WONCommon/AsyncOpTracker.h"
#include "WONCommon/FileReader.h"
#include "WONCommon/FileWriter.h"
#include "WONCommon/StringUtil.h"
#include "WONCommon/WONFile.h"
#include "WONDir/GetMultiDirOp.h"
#include "WONMisc/DNSTestOp.h"
#include "WONSocket/AsyncSocket.h"

using namespace WONAPI;

static const char* gSPAuthCheck_FileName = "_wonspchk.dat";

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SPAuthCheckPrv : public RefCount
{
public:
	WONAPICoreEx *mAPI;
	WONStatus mStatus;
	Blowfish mEncryptKey;
	bool mStartedChecking;
	bool mOnlyForceCheck;

	CDKey mCDKey;
	std::wstring mCommunity;
	std::wstring mProductDir;
	AuthContextPtr mAuthContext;

	WONTypes::StringList mDirServers;
	AsyncOpTracker mOpTracker;
	
	time_t mStartTime;
	int mGameSecondsBeforeNextCheck;

public:
	SPAuthCheckPrv(const char *theProduct);
	void ReadCheckFile();
	void WriteCheckFile();

	virtual ~SPAuthCheckPrv();
};
typedef SmartPtr<SPAuthCheckPrv> SPAuthCheckPrvPtr;
static SPAuthCheckPrvPtr gSPAuthCheck;
static CriticalSection gSPAuthCheckCrit;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class SPAuthCompletion : public OpCompletionBase
{
public:
	typedef void(*Callback)(AsyncOp*, SPAuthCheckPrv *theCheck);
	Callback mCallback;

public:
	SPAuthCompletion(Callback theCallback) : mCallback(theCallback) { }

	virtual void Complete(AsyncOpPtr theOp)
	{
		AutoCrit aCrit(gSPAuthCheckCrit);
		SPAuthCheckPrvPtr aCheckRef = gSPAuthCheck;
		aCrit.Leave();

		if(aCheckRef.get()==NULL)
			return;

		if(!aCheckRef->mOpTracker.Untrack(theOp))
			return;

		mCallback(theOp,aCheckRef);
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void SPAuthCheck_RunOp(AsyncOp *theOp, SPAuthCheckPrv *theCheck, SPAuthCompletion::Callback theCallback, DWORD theTimeout = 60000, bool isTimer = false)
{
	if(theCheck==NULL)
	{
		AsyncOpPtr anOp = theOp; // make sure op is reference counted
		return;
	}

	theCheck->mOpTracker.Track(theOp);
	theOp->SetCompletion(new SPAuthCompletion(theCallback));
	if(isTimer)
		theOp->RunAsTimer(theTimeout);
	else
		theOp->RunAsync(theTimeout);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SPAuthCheckPrv::SPAuthCheckPrv(const char *theProduct)
{
	mAPI = NULL;
	mStatus = WS_None;
	mStartedChecking = false;
	mOnlyForceCheck = false;
	mStartTime = time(NULL);
	mGameSecondsBeforeNextCheck = 1;

	WriteBuffer aBuf;
	aBuf.Reserve(8);
	aBuf.AppendLong(0x87ab3215);
	aBuf.AppendLong(CDKey::GetMachineId());
	mEncryptKey.SetKey(aBuf.data(),aBuf.length());
	
	mAuthContext = new AuthContext;
	mCommunity = StringToWString(theProduct);
	mProductDir = L"/" + mCommunity;

	mCDKey.SetProductString(theProduct);
	mCDKey.LoadFromRegistry();
	if(!mCDKey.IsValid())
	{
		mStatus = WS_AuthServ_InvalidCDKey;
		return;
	}
		
	if(!AsyncSocket::HasInternetConnection())
	{
		mStatus = WS_NoInternetConnection;
		return;
	}

	if(WONAPICoreEx::GetInstance()==NULL)
	{
		mAPI = new WONAPICoreEx;
		mAPI->SetDoPumpThread(true);
		mAPI->Startup();
	}	

	ReadCheckFile();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SPAuthCheckPrv::ReadCheckFile()
{
	// Read check prevention file
	mGameSecondsBeforeNextCheck = 1; // default -> check

	try
	{
		FileReader aReader;
		if(!aReader.Open(gSPAuthCheck_FileName))
			return;

		unsigned short aNumBytes = aReader.ReadShort();
		if(aNumBytes==0 || aReader.Available()<aNumBytes)
			return;

		char *aBuf = new char[aNumBytes];
		std::auto_ptr<char> aDelBuf(aBuf);

		aReader.ReadBytes(aBuf,aNumBytes);
		aReader.Close();

		ByteBufferPtr aDecrypt = mEncryptKey.Decrypt(aBuf,aNumBytes);
		if(aDecrypt.get()==NULL)
			return;

		WONFile aFile(gSPAuthCheck_FileName);

		ReadBuffer aReadBuf(aDecrypt->data(),aDecrypt->length());		
		std::string aSig;
		aReadBuf.ReadString(aSig);
		if(aSig!="magic")
			return;

		time_t aCreateTime = aFile.GetCreateTime();
		time_t aCompareCreateTime = aReadBuf.ReadLong();
		if(aCompareCreateTime != aCreateTime)
			return;

		mGameSecondsBeforeNextCheck = aReadBuf.ReadLong();
		if(mGameSecondsBeforeNextCheck==0) // don't remove file in this case
			return;
		
		aFile.Remove();

	}
	catch(FileReaderException&)
	{
	}
	catch(ReadBufferException&)
	{
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SPAuthCheckPrv::WriteCheckFile()
{
	
	if(mGameSecondsBeforeNextCheck==1) // Remove file to force check next time
	{
		WONFile aFile(gSPAuthCheck_FileName);
		aFile.Remove();
		return;
	}

	// Write new file
	try
	{
		FileWriter aWriter;
		if(!aWriter.Open(gSPAuthCheck_FileName))
			return;

		WONFile aFile(gSPAuthCheck_FileName);

		WriteBuffer anEncrypt;
		anEncrypt.AppendString("magic");
		anEncrypt.AppendLong(aFile.GetCreateTime()); // file creation time
		anEncrypt.AppendLong(mGameSecondsBeforeNextCheck);

		ByteBufferPtr aBuf = mEncryptKey.Encrypt(anEncrypt.data(),anEncrypt.length());
		if(aBuf.get()==NULL)
			return;

		aWriter.WriteShort(aBuf->length());
		aWriter.WriteBytes(aBuf->data(),aBuf->length());
	}
	catch(FileWriterException&)
	{
	}


}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SPAuthCheckPrv::~SPAuthCheckPrv()
{
	mOpTracker.KillAll();
	if(mAPI!=NULL)
	{
		mAPI->Shutdown();
		delete mAPI;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SPAuthCheck::Init(const char *theProduct)
{
	assert(gSPAuthCheck.get()==NULL);

	AutoCrit aCrit(gSPAuthCheckCrit);
	gSPAuthCheck = new SPAuthCheckPrv(theProduct);
	aCrit.Leave();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SPAuthCheck::SetCommunity(const wchar_t* theCommunity)
{
	if(gSPAuthCheck.get()!=NULL)
		gSPAuthCheck->mCommunity = theCommunity;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SPAuthCheck::SetProductDir(const wchar_t* thePath)
{
	if(gSPAuthCheck.get()!=NULL)
		gSPAuthCheck->mProductDir = thePath;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SPAuthCheck::SetOnlyForceCheck(bool onlyForceCheck)
{
	if(gSPAuthCheck.get()!=NULL)
		gSPAuthCheck->mOnlyForceCheck = onlyForceCheck;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SPAuthCheck::AddDirServer(const char *theAddr)
{
	if(gSPAuthCheck.get()!=NULL)
		gSPAuthCheck->mDirServers.push_back(theAddr);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void SPAuthCheck_RunGetCertOp(SPAuthCheckPrv *theCheck);
static void SPAuthCheck_TimerCompletion(AsyncOp *theOp, SPAuthCheckPrv *theCheck)
{
	SPAuthCheck_RunGetCertOp(theCheck);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void SPAuthCheck_GetCertCompletion(AsyncOp *theOp, SPAuthCheckPrv *theCheck)
{
	if(!theOp->Succeeded())
	{
		theCheck->mStatus = theOp->GetStatus();
		return;
	}
	
	time_t aRefreshWaitTime = theCheck->mAuthContext->GetPeerData()->GetExpireDelta();
	aRefreshWaitTime-=120;
	if(aRefreshWaitTime<120)
		aRefreshWaitTime=120;

	AsyncOpPtr aTimer = new AsyncOp;
	SPAuthCheck_RunOp(aTimer,theCheck,SPAuthCheck_TimerCompletion,aRefreshWaitTime*1000,true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void SPAuthCheck_RunGetCertOp(SPAuthCheckPrv *theCheck)
{
	GetCertOpPtr anOp = new GetCertOp(theCheck->mAuthContext);
	SPAuthCheck_RunOp(anOp,theCheck,SPAuthCheck_GetCertCompletion);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void SPAuthCheck_GetDirCompletion(AsyncOp *theOp, SPAuthCheckPrv *theCheck)
{
	if(!theOp->Succeeded())
	{
		theCheck->mStatus = theOp->GetStatus();
		return;
	}

	// Step 3, AuthServer login
	theCheck->mAuthContext->SetCommunity(theCheck->mCommunity);
	theCheck->mAuthContext->SetCDKey(theCheck->mCommunity,theCheck->mCDKey);

	GetMultiDirOp *anOp = (GetMultiDirOp*)theOp;
	const GetEntityRequestList &aList = anOp->GetRequestList();
	GetEntityRequestList::const_iterator anItr = aList.begin();
	int aCount = 0;
	while(anItr!=aList.end())
	{
		GetEntityRequest *aRequest = *anItr;
		const DirEntityList &aDirList = aRequest->GetDirEntityList();

		switch(aCount)
		{
			case 0: 
				theCheck->mAuthContext->GetServerContext()->AddAddressesFromDir(aDirList); 
				break;

			case 1:
			{
				if(!aDirList.empty())
				{
					const DirEntity *anEntity = aDirList.front();
					if(!anEntity->mDataObjects.empty())
					{
						const DirDataObject &anObject = anEntity->mDataObjects.front();
						if(anObject.mData.get()!=NULL && anObject.mData->length()>=1)
						{
							unsigned char aDuration = anObject.mData->data()[0];
							theCheck->mGameSecondsBeforeNextCheck = aDuration*3600;
						}
					}

				}
			}
			break;

		}

		++aCount;
		++anItr;
	}

	SPAuthCheck_RunGetCertOp(theCheck);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void SPAuthCheck_DNSCompletion(AsyncOp *theOp, SPAuthCheckPrv *theCheck)
{
	if(!theOp->Succeeded())
	{
		theCheck->mStatus = theOp->GetStatus();
		return;
	}

	// Step 2, AuthServer lookup
	ServerContextPtr aDirServers = new ServerContext;
	WONTypes::StringList::iterator anItr;
	for(anItr = theCheck->mDirServers.begin(); anItr != theCheck->mDirServers.end(); ++anItr)
		aDirServers->AddAddress(*anItr);

	GetMultiDirOpPtr anOp = new GetMultiDirOp(aDirServers);
	anOp->SetFlags(DIR_GF_DECOMPSERVICES | DIR_GF_SERVADDNETADDR);
	anOp->AddPath(L"/TitanServers/Auth");

	anOp->SetFlags(DIR_GF_DECOMPROOT | DIR_GF_ADDDODATA);
	if(theCheck->mCDKey.IsForceCheck())
		anOp->AddDataType("_SPF");
	else
		anOp->AddDataType("_SP");

	anOp->AddPath(theCheck->mProductDir);

	SPAuthCheck_RunOp(anOp,theCheck,SPAuthCheck_GetDirCompletion);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SPAuthCheck::StartChecking()
{
	if(gSPAuthCheck.get()==NULL)
		return;

	if(gSPAuthCheck->mStartedChecking)
		return;

	if(gSPAuthCheck->mStatus!=WS_None) // already got an error
		return;

	if(gSPAuthCheck->mDirServers.empty())
	{
		gSPAuthCheck->mStatus = WS_ServerReq_NoServersSpecified;
		return;
	}

	if(!gSPAuthCheck->mCDKey.IsForceCheck()) // not a force check key -> maybe don't need to check
	{
		if(gSPAuthCheck->mOnlyForceCheck) 
			return;

//		if(gSPAuthCheck->mGameSecondsBeforeNextCheck==0 || gSPAuthCheck->mGameSecondsBeforeNextCheck>1) // don't need to check this time
//			return;
	}

	if(gSPAuthCheck->mGameSecondsBeforeNextCheck==0 || gSPAuthCheck->mGameSecondsBeforeNextCheck>1) // don't need to check this time
		return;

	gSPAuthCheck->mStartedChecking = true;

	// Step 1, DNS test 
	DNSTestOpPtr anOp = new DNSTestOp(gSPAuthCheck->mDirServers.front());
	SPAuthCheck_RunOp(anOp, gSPAuthCheck, SPAuthCheck_DNSCompletion, 30000);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void SPAuthCheck::Destroy()
{
	SPAuthCheckPrvPtr aCheck = gSPAuthCheck; // make sure destructor doesn't happen while in gSPAuthCheckCrit

	AutoCrit aCrit(gSPAuthCheckCrit); 
	gSPAuthCheck = NULL;
	aCrit.Leave();

	time_t anElapsed = time(NULL) - aCheck->mStartTime;
	if(aCheck->mGameSecondsBeforeNextCheck!=0) // is checking permanently turned off?
	{
		if(anElapsed>0 && anElapsed<aCheck->mGameSecondsBeforeNextCheck)
			aCheck->mGameSecondsBeforeNextCheck -= anElapsed; // still more time left before next check
		else
			aCheck->mGameSecondsBeforeNextCheck = 1; // signals that file can just be erased
	}
	aCheck->WriteCheckFile();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool SPAuthCheck::HadError()
{
	if(gSPAuthCheck.get()!=NULL)
		return gSPAuthCheck->mStatus!=WS_None;
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus SPAuthCheck::GetStatus()
{
	if(gSPAuthCheck.get()!=NULL)
		return gSPAuthCheck->mStatus;
	else
		return WS_None;
}
