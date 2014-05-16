#include "WONAPI.h"
#include "GetHTTPDocumentOp.h"
#include "WONCommon/StringUtil.h"

using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
// Host & Path: Due to a nasty Run-time error generate under Swat's SXL 
// headers, we cannot initialize strings before WinMain is called.  So this 
// accessor is used to fake out the SXL.
///////////////////////////////////////////////////////////////////////////////
static string& GetHost()
{
	//static string aHost = "www.won.net";
	static string aHost = "www.sierra.com";
	return aHost;
}

static string& GetPath()
{
	static string aPath = "/motd/";
	return aPath;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetHTTPDocumentOp::CheckHostAndPath(HTTPDocType theDocType)
{
	static bool checked = false;
	static CriticalSection aCriticalSection;
	AutoCrit aCrit(aCriticalSection);
	if(checked)
		return;

	checked = true;
	FILE *aFile = NULL;
	
	if (theDocType == HTTPDocType_MOTD)
		fopen( (WONAPICore::GetDefaultFileDirectory() + "_wonmotdpath.txt").c_str(),"r");
	else if (theDocType == HTTPDocType_TOU)
		fopen( (WONAPICore::GetDefaultFileDirectory() + "_wontoupath.txt").c_str(),"r");
	else if (theDocType == HTTPDocType_ADV)
		fopen( (WONAPICore::GetDefaultFileDirectory() + "_wonadvpath.txt").c_str(),"r");

	if(aFile!=NULL)
	{
		char aHost[512];
		char aPath[512];

		aHost[0] = '\0';
		aPath[0] = '\0';

		if(fgets(aHost,500,aFile)!=NULL)
		{
			char *aPtr = strchr(aHost,'\n');
			if(aPtr!=NULL)
				*aPtr = '\0';

			if(strlen(aHost)>0)
				GetHost() = aHost;
		}

		if(fgets(aPath,500,aFile)!=NULL)
		{
			char *aPtr = strchr(aPath,'\n');
			if(aPtr!=NULL)
				*aPtr = '\0';

			if(strlen(aPath)>0)
				GetPath() = aPath;
		}
	
		fclose(aFile);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GetHTTPDocumentOp::GetHTTPDocumentOp(const std::string &theProduct, HTTPDocType theDocType) 
{
	mProduct = theProduct;
	SetDocType(theDocType);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetHTTPDocumentOp::SetDocType(HTTPDocType theDocType)
{
	mDocType = theDocType; 

	CheckHostAndPath(theDocType);

	if (mDocType == HTTPDocType_MOTD)
	{
		mDocumentPath[HTTPDocOwner_Sys]   = WONAPICore::GetDefaultFileDirectory() + "_wonsysmotd.txt";
		mDocumentPath[HTTPDocOwner_Game]  = WONAPICore::GetDefaultFileDirectory() + "_won"+mProduct+"motd.txt";
	}
	else if (mDocType == HTTPDocType_TOU)
	{
		mDocumentPath[HTTPDocOwner_Sys]   = WONAPICore::GetDefaultFileDirectory() + "_wonsystou.txt";
		mDocumentPath[HTTPDocOwner_Game]  = WONAPICore::GetDefaultFileDirectory() + "_won"+mProduct+"tou.txt";
	}
	else if (mDocType == HTTPDocType_ADV)
	{
		mDocumentPath[HTTPDocOwner_Sys]   = WONAPICore::GetDefaultFileDirectory() + "_wonsysadv.txt";
		mDocumentPath[HTTPDocOwner_Game]  = WONAPICore::GetDefaultFileDirectory() + "_won"+mProduct+"adv.txt";
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetHTTPDocumentOp::DoFinish()
{
	if(mDocumentStatus[HTTPDocOwner_Sys]!=WS_Success && mDocType!=HTTPDocType_ADV)
		Finish(WS_GetMOTD_SysNotFound);
	else if(mDocumentStatus[HTTPDocOwner_Game]!=WS_Success)
		Finish(WS_GetMOTD_GameNotFound);
	else	
		Finish(WS_Success);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetHTTPDocumentOp::OpFinished(HTTPGetOp *theOp, int theDocumentOwner)
{
	if(!mTriedDefault[theDocumentOwner] && (theOp->GetStatus()==WS_HTTP_InvalidRedirect || theOp->GetHTTPStatus()==404))
	{
		TryHTTPOp((HTTPDocOwner)theDocumentOwner, true);
		return;
	}

	mDocumentStatus[theDocumentOwner] = theOp->GetStatus();
	mHTTPStatus[theDocumentOwner] = theOp->GetHTTPStatus();
	mModifiedTime[theDocumentOwner] = theOp->GetLastModifiedTime();

	if(theOp->Succeeded())
	{
		mDocumentIsNew[theDocumentOwner] =  theOp->ContentIsNew();

		if (mDocType == HTTPDocType_MOTD)
		{
			// Open file and check first character
			FILE *aFile = fopen(mDocumentPath[theDocumentOwner].c_str(),"r");
			if(aFile!=NULL)
			{	
				int aChar = fgetc(aFile);

				if(aChar==EOF)
					mDocumentIsNew[theDocumentOwner]=false;
				else if(toupper((unsigned char)aChar)!='1') // show always
					mDocumentIsNew[theDocumentOwner]=true;
			}	
			fclose(aFile);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GetHTTPDocumentOp::CallbackHook(AsyncOp *theOp, int theParam)
{
	HTTPGetOp *anOp = (HTTPGetOp*)theOp;
	OpFinished(anOp,theParam);

	if(mOpTracker.GetNumTrack()==0)
		DoFinish();

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetHTTPDocumentOp::TryHTTPOp(HTTPDocOwner theOwner, bool forceDefault)
{
	std::string aDocumentName;
	std::string anExtraConfig = StringToLowerCase(mExtraConfig);
	if(anExtraConfig=="en")
		anExtraConfig.erase(); // don't need extra config for english... english is the default

	if(anExtraConfig.empty() || forceDefault)
	{
		mTriedDefault[theOwner] = true;
		if (mDocType == HTTPDocType_MOTD)
			aDocumentName = "motd.txt";
		else if (mDocType == HTTPDocType_TOU)
			aDocumentName = "tou.txt";
		else if (mDocType == HTTPDocType_ADV)
			aDocumentName = "adv.txt";
	}
	else
	{
		if (mDocType == HTTPDocType_MOTD)
			aDocumentName = "motd_"+ anExtraConfig + ".txt";
		else if (mDocType == HTTPDocType_TOU)
			aDocumentName = "tou_"+ anExtraConfig + ".txt";
		else if (mDocType == HTTPDocType_ADV)
			aDocumentName = "adv_"+ anExtraConfig +".txt";
	}

	std::string aPath;
	if(theOwner==HTTPDocOwner_Sys)
		aPath = GetPath() + "sys/";
	else if (theOwner==HTTPDocOwner_Game)	
		aPath = GetPath() + StringToLowerCase(mProduct)+"/";

	HTTPGetOpPtr anOp = new HTTPGetOp(GetHost(),aPath+aDocumentName);
	anOp->SetDoRangeResume(false);
	anOp->SetLocalPath(mDocumentPath[theOwner]);

	if(IsAsync())
	{
		Track(anOp,theOwner);
		if(mHTTPSession.get()!=NULL)
			mHTTPSession->AddOp(anOp);
		else
			anOp->RunAsync(OP_TIMEOUT_INFINITE);

		return;
	}

	WONStatus aStatus = anOp->Run(OP_MODE_BLOCK,TimeLeft());
	OpFinished(anOp,theOwner);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void GetHTTPDocumentOp::RunHook()
{
	int i;
	for(i=0; i<2; i++)
	{
		mDocumentIsNew[i] = false;
		mDocumentStatus[i] = WS_None;
		mHTTPStatus[i] = 0;
		mModifiedTime[i] = 0;
		mTriedDefault[i] = false;
	}


	if(mDocType == HTTPDocType_ADV)
	{
		// only need game specific document
		TryHTTPOp(HTTPDocOwner_Game,false);
	}
	else
	{
		for(i=0; i<2; i++)
			TryHTTPOp((HTTPDocOwner)i,false);
	}

	if(!IsAsync())
		DoFinish();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ByteBufferPtr GetHTTPDocumentOp::GetDocument(HTTPDocOwner theOwner) const
{
	FILE *aFile = NULL;
	
	if(!mDocumentPath[theOwner].empty()) // fopen will assert if the path is empty in MS crt debug library
		aFile = fopen(mDocumentPath[theOwner].c_str(),"r");
	
	if(aFile==NULL)
	{
		return new ByteBuffer("");
//		return NULL;
	}

	const int SIZE = 1024;
	char aBuf[SIZE];

	WriteBuffer anOverallBuf;

	// Skip the first character in MOTD documents
	if (mDocType == HTTPDocType_MOTD)
	{
		int aChar = fgetc(aFile);
		if(aChar=='<') // optional HTML
			ungetc(aChar,aFile);
	}

	while(!feof(aFile))
	{
		int aNumRead = fread(aBuf,1,SIZE,aFile);
		if(aNumRead>0)
			anOverallBuf.AppendBytes(aBuf,aNumRead);
	}

	anOverallBuf.AppendByte(0);
	fclose(aFile);

	return anOverallBuf.ToByteBuffer();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool GetHTTPDocumentOp::DocumentIsValid(HTTPDocOwner theOwner)
{
	return GetDocumentStatus(theOwner)==WS_Success;
}
