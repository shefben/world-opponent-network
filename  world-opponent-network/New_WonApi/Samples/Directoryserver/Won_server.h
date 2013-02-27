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
#include "directorymessages.h"
#include "SocketMgr.h"

#include <assert.h>
#include <list>

using namespace WONMsg;
using namespace std;

namespace WONAPI {

public:
		// Internal structures for returns from server communication
		struct PublicationData
		{
			unsigned long	mDataId;
			unsigned short	mDataLength;
			BYTE*			mDataP;
		};
		struct PublisherListReply 
		{
			ServerStatus mStatus;			// Request status
			PublisherList mPublisherList;	// List of publishers
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
		
public:
	ObservationClientBase(ConnectionTypeEnum theConnectionType = TransientTCP);
	virtual ~ObservationClientBase();
	
	// Public utilities
	//static string GetLocalAddress();
	
	//	Server communication functions
	// The last parameter is to pass in object that will be filled in with the reply information when the reply comes back.
	Error GetPublisherId(const string& thePublisherNameR, const CompletionContainer<const IdReply&>& theCompletion = DEFAULT_COMPLETION, const IdReply* theIdReplyP = NULL);

	// Templated functions that send messages to server
	template <class privsType>
		Error GetPublisherId( const string& thePublisherNameR, void (*f)(const IdReply&, privsType), privsType t )
	{ return GetPublisherId(thePublisherNameR, new CompletionWithContents<const IdReply&, privsType>(t, f, true)); }
	// Callbacks 
	
	static void ReceiveLoop(const TMsgSocket::RecvBaseMsgResult& result, ObservationClientBase* This);
protected:
	Error ClientStartup(const IPSocket::Address& theServerAddress, const unsigned short theLocalMonitorPort, const string& theClientNameR, const wstring& theClientDescR);
	void ClientShutdown();
	
	// Utility methods
	const string CreateAddressString(const string& theAddressR, unsigned int thePort) const;
	Error SendSMsgToServer(BaseMessage& theMsgR, const CompletionContainerBase* theCompletionP = NULL, const void* theReplyResultP = NULL);
	bool FillMsgFromResult(BaseMessage& theMsg, const TMsgSocket::RecvBaseMsgResult& result);
	unsigned long GetReplyMsgType(unsigned long theMsgType);

private:
	virtual void HandlePublishReply(const MMsgObsPublishReply& theReplyMsg) {assert(0);}
	// Member Data
protected:
	ConnectionTypeEnum	mConnectionType;
	
	// Socket stuff
	IPSocket::Address	mObsServerAddress;
	IPSocket::Address	mLocalAddress;
	
	SocketMgr*			mSocketMgrP;
	
	// Reply Completion container list
	// This list is a list of pairs.  The return message type and the completion container
	ReplyCompletionList mReplyCompletionList;
	
	WONCommon::CriticalSection		mCriticalSection;

public:
	ObservationSubscriberClient(ConnectionTypeEnum theConnectionType = TransientTCP);
	virtual ~ObservationSubscriberClient();
	Error SubscriberShutdown( const CompletionContainer<const ServerStatus&>& theCompletion = DEFAULT_COMPLETION, const ServerStatus* theServerStatusP = NULL );
	Error SubscriberStartup( const IPSocket::Address& theServerAddress, const unsigned short theLocalMonitorPort, const string& theClientNameR, const wstring& theClientDescR, const CompletionContainer<const ServerStatus&>& theCompletion = DEFAULT_COMPLETION, const ServerStatus* theServerStatusP = NULL);
	// Templated functions that send messages to server
	template <class privsType>
		Error SubscriberStartup( const IPSocket::Address& theServerAddress, const unsigned short theLocalMonitorPort, const string& theClientNameR, const wstring& theClientDescR, void (*f)(const ServerStatus&, privsType), privsType t )
	{ return SubscriberStartup(theServerAddress, theLocalMonitorPort, theClientNameR, theClientDescR, new CompletionWithContents<const ServerStatus&, privsType>(t, f, true)); }
	
	template <class privsType>
		Error SubscriberShutdown( void (*f)(const ServerStatus&, privsType), privsType t )
	{ return SubscriberShutdown( new CompletionWithContents<const ServerStatus&, privsType>(t, f, true)); }
}