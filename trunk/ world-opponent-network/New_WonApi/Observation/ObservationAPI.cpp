
///////////////////////////////////////////////////////////////////////////////////
//
// Observation server Client API
//
// Contains classes for Observation Publisher and Subscriber clients
//
///////////////////////////////////////////////////////////////////////////////////

#include <string>
#include "ObservationAPI.h"
#include "msg/Obs/MMsgTypesObs.h"

using namespace std;
using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////////
// Constants

///////////////////////////////////////////////////////////////////////////////////
//
// Observation Client Base class
//
// This is an abstract base class.  Instantiate a Publisher or Subscriber Client object
//
///////////////////////////////////////////////////////////////////////////////////

ObservationClientBase::ObservationClientBase(ConnectionTypeEnum theConnectionType) :
	mClientId(0),
	mClientName(),
	mClientDesc(),
	mConnectionType(theConnectionType),
	mObsServerAddress(),
	mLocalAddress(),
	mSocketMgrP(NULL),
	mReplyCompletionList(),
	mCriticalSection()
{
}

ObservationClientBase::~ObservationClientBase() 
{
	if (mSocketMgrP)
	{
		mSocketMgrP->SetShuttingDown(true); // Do this here in case they did not call shutdown
		delete mSocketMgrP;
		mSocketMgrP = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////////
// Public Methods

// Public utilities
/*
string
ObservationClientBase::GetLocalAddress()
{
	char aBuf[256];
	string aAddress;

	if (gethostname(aBuf,255) == 0)
	{
		HOSTENT* aHostEntP = gethostbyname(aBuf);
		if (aHostEntP != NULL)
		{
			aAddress = inet_ntoa(*(IN_ADDR*)aHostEntP->h_addr_list[0]);
		}
	}

	return aAddress;
}*/

// Callback

// Called whenever a message is received on the Socket.  Processes the message and then
// initiates another async recv with itself as the completion routine.
// Put receive loop for both publisher and subscriber in base class for symplicity
void ObservationClientBase::ReceiveLoop(const TMsgSocket::RecvBaseMsgResult& result, ObservationClientBase* This)
{
	if (result.msg)
	{
		unsigned long aMessageType = result.msg->GetMessageType();
		switch (aMessageType)
		{
			// Reply messages
			case ObsMsg_StatusReply:
			{
				MMsgObsStatusReply aReplyMsg;
				if (This->FillMsgFromResult(aReplyMsg, result))
				{
					This->HandleStatusReply(aReplyMsg);
				}
			}
			break;

			default:
				assert(0); // Error unknown message type
				// ** - Output debug message 
			break;
		}
	}
	
	// Don't want to do this is socket is closing or closed
	//if ( result.theSocket->IsRecvOpen() )
	if ( !result.closed )
	{
		result.theSocket->RecvBaseMsgEx((WONMsg::BaseMessage**)NULL, -1, true, ReceiveLoop, This);
	}
	/*else
	{
#ifdef _DEBUG
	OutputDebugString("Receive loop socket closed");
#endif
	}*/

	delete result.msg;
}

//////////////////////////////////////////////////////////////////////////////
// Protected Methods

Error 
ObservationClientBase::ClientStartup(const IPSocket::Address& theServerAddress, const unsigned short theLocalMonitorPort, const string& theClientNameR, const wstring& theClientDescR)
{
	mClientName = theClientNameR;
	mClientDesc = theClientDescR;

	mObsServerAddress = theServerAddress;
	
	if (mSocketMgrP == NULL)
	{
		mSocketMgrP = new SocketMgr( theServerAddress, mConnectionType );

		mSocketMgrP->SetReceiveFunction(ReceiveLoop, this);

		if (theLocalMonitorPort != 0)
		{
			mLocalAddress.InitFromLocalIP(theLocalMonitorPort);
			// Start monitoring
			mSocketMgrP->Listen(mLocalAddress.GetPort());
		}
		else
		{
			mLocalAddress.Set(0,0);
		}
	}
	else
	{
		mSocketMgrP->SetShuttingDown(false);
	}

	return Error_Success;
}

void
ObservationClientBase::ClientShutdown()
{
	if (mSocketMgrP)
		mSocketMgrP->SetShuttingDown(true);
	//delete mSocketMgrP;
	//mSocketMgrP = NULL;
}

//////////////////////////////////////////////////////////////////////////////
// Utility Methods

const string 
ObservationClientBase::CreateAddressString(const string& theAddressR, unsigned int thePort) const
{
	WONCommon::wonstringstream aStrStrm;
	aStrStrm << theAddressR.c_str() << ":" << thePort;
	return aStrStrm.str();
}

// Send Titan message to Observation Server
Error 
ObservationClientBase::SendMMsgToServer(BaseMessage& theMsgR, const CompletionContainerBase* theCompletionP, const void* theReplyResultP)
{
	ReplyCompletionData aReplyCompletionData;
	aReplyCompletionData.mReplyId = GetReplyMsgType(theMsgR.GetMessageType());
	aReplyCompletionData.mCompletionP = const_cast<CompletionContainerBase*>(theCompletionP);
	aReplyCompletionData.mReplyResultP = const_cast<void*>(theReplyResultP);

	mCriticalSection.Enter();
	mReplyCompletionList.push_back(aReplyCompletionData);
	mCriticalSection.Leave();
	return mSocketMgrP->SendMMsgToServer(theMsgR);
}

bool 
ObservationClientBase::FillMsgFromResult(BaseMessage& theMsg, const TMsgSocket::RecvBaseMsgResult& result)
{
	bool aReturn = true;

	theMsg.AppendBytes(result.msg->GetDataLen(), result.msg->GetDataPtr());
	try
	{ 
		theMsg.Unpack(); 
	}
	catch (WONMsg::BadMsgException&)
	{ 
		// Might want to log a message here..
		aReturn = false; 
	}
	return aReturn;
}

unsigned long
ObservationClientBase::GetReplyMsgType(unsigned long theMsgType)
{
	unsigned long aReturn = 0;

	switch (theMsgType)
	{
		case ObsMsg_RemovePublisher:
		case ObsMsg_RemovePublication:
		case ObsMsg_CancelSubscription:
		case ObsMsg_RemoveSubscriber:
		case ObsMsg_UpdateDataPool:
		case ObsMsg_SendStream:
		case ObsMsg_ReestablishPersistentTCP:
			aReturn = ObsMsg_StatusReply;
		default:
			assert(0);
			// Unknown message type
		break;
	}
	return aReturn;
}

// Get first completion routine in the list for this reply type
// This is a FIFO style in that we will get the 
// Remove the completion routine from the list as well 
// Note: if the server sends us a reply that we do not expect
// or does not send us a reply we will get out of sinc...
// Will need to add some mechanism to handle this if it
// gets to be a problem.  We might want to add a function to 
// clean out the list when the socket goes of line.
// For UDP we might want to only allow one completion per reply type.
ObservationClientBase::ReplyCompletionData
ObservationClientBase::GetFirstCompletion(unsigned long theReplyId)
{
	ReplyCompletionList::iterator aItr;
	ReplyCompletionData aReturn;

	mCriticalSection.Enter();
	// Get first instance of completion for this reply type 
	for (aItr = mReplyCompletionList.begin(); aItr != mReplyCompletionList.end(); aItr++)
	{
		if (aItr->mReplyId == theReplyId)
		{
			aReturn = *aItr;
			mReplyCompletionList.erase(aItr); // Remove completion
			break;
		}
	}
	mCriticalSection.Leave();
	return aReturn;
}

//////////////////////////////////////////////////////////////////////////////
// Private Methods
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Message Reply Handlers
//
// Note - Classes derrived from Publisher or Subscriber clients can override 
// these functions to provide their own completion message handlers.
//////////////////////////////////////////////////////////////////////////////

void 
ObservationClientBase::HandleStatusReply(const MMsgObsStatusReply& theReplyMsg)
{
	ReplyCompletionData aReplyCompletionData = GetFirstCompletion(ObsMsg_StatusReply);
	if (aReplyCompletionData.mReplyResultP != NULL) // Fill in data in structure provided
	{
		*((ServerStatus*)aReplyCompletionData.mReplyResultP) = theReplyMsg.GetStatus();
	}
	if (aReplyCompletionData.mCompletionP != NULL)
	{
		(reinterpret_cast<CompletionContainer<const ServerStatus&>*>(aReplyCompletionData.mCompletionP))->Complete(theReplyMsg.GetStatus());
		delete aReplyCompletionData.mCompletionP; 
	}
}

//////////////////////////////////////////////////////////////////////////////
// Public Methods

//////////////////////////////////////////////////////////////////////////////
// PublisherStartup
//
// Method called to identify publisher client to Observation server and establish
// groundword for communication between them.
//
// Important:	The reply for this function must be returned before any other
//				publisher functions can be called.  Any functions called
//				before the reply has returned will return a general failure error.
//////////////////////////////////////////////////////////////////////////////
Error 
ObservationPublisherClient::
PublisherStartup( const IPSocket::Address& theServerAddress,					// Address and port of Observation server to publish on
				  const unsigned short theLocalMonitorPort,						// Local port which to monitor for communications from server
				  const string& theClientNameR,									// Unique name of client publishing on Observation server
				  const wstring& theClientDescR,								// User readable description of client publishing on server
				  const bool theAccessPubsByName,								// Determines wheather publication names will be unique acros this publisher. 
				  const CompletionContainer<const ServerStatus&>& theCompletion, // Completion for this operation - Function pointer will be called or event will be signaled when this operation completes
				  const ServerStatus* theServerStatusP )
{
	ClientStartup(theServerAddress, theLocalMonitorPort, theClientNameR, theClientDescR);

	if (mSocketMgrP != NULL)
	{
		mSocketMgrP->SetClientType(ResourcePublisher);

		// Register with Observation server
		MMsgObsAddPublisher aMsg;
		aMsg.SetPublisherName( mClientName );
		aMsg.SetPublisherDescription( mClientDesc );
		aMsg.SetConnectionType(	mConnectionType );
		aMsg.SetNetAddress( mLocalAddress.GetAddressString() ); // Our net address IP:Port
		aMsg.SetAccessPubsByName(theAccessPubsByName); 
		aMsg.SetOnDemandPublishing(false); // Don't support this in the API for now

		aMsg.Pack(); // May not be neccessary...

		return SendMMsgToServer(aMsg, new CompletionContainer<const ServerStatus&>(theCompletion), theServerStatusP);
	}
	else
	{
		return Error_GeneralFailure;
	}
}

//////////////////////////////////////////////////////////////////////////////
// PublisherShutdown
//
// Method called to shutdown the Publisher client.  Sends message to Observation
// Server to remove this publisher from it's table of publishers.  All publications
// published by this publisher will also be removed.
//
// Returns success or failure on compleation.
//////////////////////////////////////////////////////////////////////////////
Error
ObservationPublisherClient::
PublisherShutdown( const CompletionContainer<const ServerStatus&>& theCompletion, 
				   const ServerStatus* theServerStatusP) // Completion for this operation - Function pointer will be called or event will be signaled when this operation completes
{
	Error aReturn = Error_Success;

	ClientShutdown();

	if (mClientId != 0) 
	{
		MMsgObsRemovePublisher aMsg;

		aMsg.SetPublisherId(mClientId);
		
		aMsg.Pack();

		aReturn = SendMMsgToServer(aMsg, new CompletionContainer<const ServerStatus&>(theCompletion), theServerStatusP);
		mClientId = 0;
		mSocketMgrP->SetClientId(mClientId);
		//delete mSocketMgrP;
		//mSocketMgrP = NULL;
	}
	else // Complete with error
	{
		aReturn = Error_GeneralFailure;
		theCompletion.Complete(StatusObs_UnknownPublisher);
	}

	return aReturn;
}


//////////////////////////////////////////////////////////////////////////////
// Private Methods
//////////////////////////////////////////////////////////////////////////////


