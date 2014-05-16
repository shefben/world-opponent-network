#include "RoutingClientJoinedServerOp.h"
using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void RoutingClientJoinedServerOp::ParseExClientData(ReadBuffer &theMsg, RoutingClientInfo *theClient)
{
	unsigned char aNumItems = theMsg.ReadByte();
	for(int i=0; i<aNumItems; i++)
	{
		unsigned char aType = theMsg.ReadByte();
		unsigned char aLength = theMsg.ReadByte();
		switch(aType)
		{
			case 1: theClient->mWONUserId = theMsg.ReadLong(); break;
			case 2: theClient->mCDKeyId = theMsg.ReadLong(); break;
			default:
				theMsg.ReadBytes(aLength);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus RoutingClientJoinedServerOp::HandleReply(unsigned char theMsgType, ReadBuffer &theMsg)
{
	if(theMsgType!=RoutingClientJoinedServer)
		return WS_RoutingOp_DontWantReply;

	mClient = GetNewClientInfo();

	mClient->mId = theMsg.ReadShort();
	theMsg.ReadWString(mClient->mName);
	mClient->mFlags = theMsg.ReadLong();
	if(theMsg.Available()>0)
		ParseExClientData(theMsg,mClient);

	return WS_Success;
}

