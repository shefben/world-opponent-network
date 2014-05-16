#ifndef _ObservationAPI_H
#define _ObservationAPI_H

///////////////////////////////////////////////////////////////////////////////////
//
// Observation server Client API
//
// Contains classes for Observation Publisher and Subscriber clients
//
///////////////////////////////////////////////////////////////////////////////////

#include "Socket/WONWS.h" // This needs to be put in for now so that we will not get windows winsock.h
#include "Socket/TMsgSocket.h"
#include "Socket/IPSocket.h"
#include "Socket/TCPSocket.h"
#include "Socket/UDPSocket.h"
#include "msg/ServerStatus.h"
#include "ObservationMessages.h"
#include "SocketMgr.h"

#include <assert.h>
#include <list>

using namespace WONMsg;
using namespace std;

namespace WONAPI {

///////////////////////////////////////////////////////////////////////////////////
// Observation Client Base class
//
// This is an abstract base class.  Instantiate a Publisher or Subscriber Client object
///////////////////////////////////////////////////////////////////////////////////

class ObservationClientBase
{
public:
	// Enums
	enum ClientTypeEnum
	{
		ClientPublisher, 
		ClientSubscriber, 
		ClientBoth			// This type added for future use
	};

	// Internal structures for returns from server communication
	struct PublicationData
	{
		unsigned long	mDataId;
		unsigned short	mDataLength;
		BYTE*			mDataP;
	};

	struct IdReply
	{
		ServerStatus  mStatus;
		unsigned long mId;
	};

	struct ReplyCompletionData
	{
		ReplyCompletionData() :
			mReplyId(0),
			mCompletionP(NULL),
			mReplyResultP(NULL)
		{};

		unsigned long				mReplyId;		// Id of the reply
		CompletionContainerBase*	mCompletionP;	// The completion to call when finished
		void *						mReplyResultP;	// Pointer to put the data from the reply
	};

	typedef list<ReplyCompletionData> ReplyCompletionList;


// Public methods
public:
	ObservationClientBase(ConnectionTypeEnum theConnectionType = TransientTCP);
	virtual ~ObservationClientBase();

	virtual ClientTypeEnum GetClientType() = 0;

	// Public utilities
	//static string GetLocalAddress();

	// Callbacks 
	static void ReceiveLoop(const TMsgSocket::RecvBaseMsgResult& result, ObservationClientBase* This);

	// Inlines
	virtual unsigned long GetClientId()
	{
		return mClientId;
	}

// Protected methods
protected:
	Error ClientStartup(const IPSocket::Address& theServerAddress, const unsigned short theLocalMonitorPort, const string& theClientNameR, const wstring& theClientDescR);
	void ClientShutdown();

	// Utility methods
	const string CreateAddressString(const string& theAddressR, unsigned int thePort) const;
	Error SendMMsgToServer(BaseMessage& theMsgR, const CompletionContainerBase* theCompletionP = NULL, const void* theReplyResultP = NULL);
	bool FillMsgFromResult(BaseMessage& theMsg, const TMsgSocket::RecvBaseMsgResult& result);
	unsigned long GetReplyMsgType(unsigned long theMsgType);
	ReplyCompletionData GetFirstCompletion(unsigned long theReplyId);

// Private methods
private:
	// Message Reply Handlers
	virtual void HandleStatusReply(const MMsgObsStatusReply& theReplyMsg);

// Member Data
protected:
	unsigned long		mClientId;
	string				mClientName;
	wstring				mClientDesc;
	ConnectionTypeEnum	mConnectionType;

	// Socket stuff
	IPSocket::Address	mObsServerAddress;
	IPSocket::Address	mLocalAddress;

	SocketMgr*			mSocketMgrP;

	// Reply Completion container list
	// This list is a list of pairs.  The return message type and the completion container
	ReplyCompletionList mReplyCompletionList;

	WONCommon::CriticalSection		mCriticalSection;
};

///////////////////////////////////////////////////////////////////////////////////
// Observation Publisher Client
///////////////////////////////////////////////////////////////////////////////////

class ObservationPublisherClient : public ObservationClientBase
{
public:
	// Internal structures for returns from server communication
	struct PublicationSubscribedTo
	{
		unsigned long	mPublicationId;
		unsigned long	mSubscriberId;
		string			mSubscriberName;
	};


// Public methods
public:
	ObservationPublisherClient(ConnectionTypeEnum theConnectionType = PersistentTCP);
	virtual ~ObservationPublisherClient();

	virtual ClientTypeEnum GetClientType()
	{
		return ClientPublisher;
	}

	//	Server communication functions
	// The last parameter is to pass in object that will be filled in with the reply information when the reply comes back.
	Error PublisherStartup( const IPSocket::Address& theServerAddress, const unsigned short theLocalMonitorPort, const string& theClientNameR, const wstring& theClientDescR, const bool theAccessPubsByName, const CompletionContainer<const ServerStatus&>& theCompletion = DEFAULT_COMPLETION, const ServerStatus* theServerStatusP = NULL);
	Error PublisherShutdown( const CompletionContainer<const ServerStatus&>& theCompletion = DEFAULT_COMPLETION, const ServerStatus* theServerStatusP = NULL); 

	// Templated functions that send messages to server
	template <class privsType>
	Error PublisherStartup( const IPSocket::Address& theServerAddress, const unsigned short theLocalMonitorPort, const string& theClientNameR, const wstring& theClientDescR, void (*f)(const ServerStatus&, privsType), privsType t )
	{ return PublisherStartup(theServerAddress, theLocalMonitorPort, theClientNameR, theClientDescR, new CompletionWithContents<const ServerStatus&, privsType>(t, f, true)); }

	template <class privsType>
	Error PublisherShutdown( void (*f)(const IdReply&, privsType), privsType t )
	{ return PublisherShutdown( new CompletionWithContents<const ServerStatus&, privsType>(t, f, true)); }

	// Allow user to pass in completion and data pointer to be filled in when opertatio completes
	Error PublisherStartup( const IPSocket::Address& theServerAddress, const unsigned short theLocalMonitorPort, const string& theClientNameR, const wstring& theClientDescR, const ServerStatus* theResultP, const CompletionContainer<const ServerStatus&>& theCompletion = DEFAULT_COMPLETION );



// Member Data
private:
	// Notification message Compleation Containers
	CompletionContainer<const string&>					mPublishRequestedCompletion;
	CompletionContainer<const PublicationSubscribedTo&> mPublicationSubscribedToCompletion;

	// Notification message Data Pointers
	string*						mPublishRequestedDataP;
	PublicationSubscribedTo*	mPublicationSubscribedToNotifyDataP;
};

}

#endif