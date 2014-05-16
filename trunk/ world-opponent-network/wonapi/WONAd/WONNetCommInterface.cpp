
#include "WONNetCommInterface.h"
#include "WONMisc/HTTPGetOp.h"
#include <string>

using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AdReqOp::RunHook()
{
	mSocket = new QueueSocket;
	
	mSocket->QueueOp((SocketOp*)Track(new ConnectOp(mAddr)));
	mSocket->QueueOp((SocketOp*)Track(new SendBytesOp(mRequest)));
	mSocket->QueueOp((SocketOp*)Track(new RecvBytesOp(0)));
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void AdReqOp::CleanupHook()
{
	AsyncOpWithTracker::CleanupHook();
	if(mSocket.get()!=NULL)
	{
		mSocket->Close();
		mSocket = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool AdReqOp::CallbackHook(AsyncOp *theOp, int theParam)
{
	if(!theOp->Succeeded())
	{
		Kill();
		return true;
	}

	RecvBytesOp *anOp = dynamic_cast<RecvBytesOp*>(theOp);
	if(anOp!=NULL)
	{
		ByteBufferPtr aBuf = anOp->GetBytes();
		if(aBuf!=NULL)
		{
			mActualResponseBuf = new unsigned char[aBuf->length()];
			memcpy(mActualResponseBuf,aBuf->data(),aBuf->length());
			*mResponseBuf = mActualResponseBuf;
			*mResponseLen = aBuf->length();
		}
		else
		{
			*mResponseBuf = NULL;
			*mResponseLen = 0;
		}

		Finish(WS_Success);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONNetCommInterface::WONNetCommInterface() :
	NetCommInterface()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONNetCommInterface::~WONNetCommInterface()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool
WONNetCommInterface::SendHTTPRequest( const char* theHostName,  unsigned short thePort,  
									 const char* theRequest, long theTimeOut, 
									 unsigned char** theHTTPRequestResultP, 
									 unsigned long* theHTTPRequestResultSizeP, 
									 NetCommCallback theNetCommCallbackP, void* theCallbackDataP)
{
	if (theHostName == NULL || 
		thePort == 0 || 
		theRequest == NULL || 
		theHTTPRequestResultP == NULL || 
		theNetCommCallbackP == NULL)
	{
		return false;
	}

	AdReqOpPtr anOp = new AdReqOp(IPAddr(theHostName,thePort),new ByteBuffer(theRequest),
		theHTTPRequestResultP, theHTTPRequestResultSizeP);

	anOp->SetCompletion(new AdCompletion(theNetCommCallbackP,theCallbackDataP));
	anOp->RunAsync(theTimeOut);
	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONNetCommInterface::HTTPGet(const char* theServer, unsigned short thePort, 
								  const char* theServerPath, const char* theDestPath, 
								  long theRequestTimeout, NetCommCallback theNetCommCallbackP, 
								  void* theCallbackDataP)
{
	if (theServer == NULL ||
		thePort	== 0 ||
		theServerPath == NULL ||
		theDestPath == NULL ||
		theNetCommCallbackP == NULL)
	{
		return false;
	}

	char aBuf[50];
	string aHost = theServer + string(":") + _itoa(thePort,aBuf,10);
	HTTPGetOpPtr anOp = new HTTPGetOp(aHost,theServerPath);
	anOp->SetLocalPath(theDestPath);
	anOp->SetCompletion(new AdCompletion(theNetCommCallbackP,theCallbackDataP));
	anOp->RunAsync(theRequestTimeout);
	return true;
}

