// Won_server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <common/won.h>
#include <Socket/TCPSocket.h>
#include "msg/TServiceTypes.h"
#include "msg/ServerStatus.h"
#include "msg/TMessage.h" //Buffer / Message backbone for whole API
#include "msg/HeaderTypes.h" //identifier header for message type ie encrypted or not
#include "msg/SServiceTypes.h" //smallmessage service type - i.e. directory server, routing server etc
#include "msg/dir/SMsgTypesDir.h" //smallmessage message type - packet requests + replys
#include "msg/dir/DirEntity.h" //class for server and directory entitys
#include "msg/dir/DirG2Flags.h" //flags used to request entities
#include "msg/dir/SMsgDirG2MultiEntityReply.h"
#include "msg/dir/SMsgTypesDir.h"
#include "msg/dir/SMsgDirG2GetEntity.h"
#include "msg/Dir/DirEntity.h"
#include "msg/Dir/SMsgDirG2StatusReply.h"
#include <crypt/Randomizer.h>
#include "msg/tmessage.h"
#include <OSTREAM>
#include "msg/HeaderTypes.h"
#include <iostream>

namespace {
//	using namespace WONDatabase;
	using namespace std;
	using namespace WONAPI;
	using namespace WONMsg;
	using namespace WONCommon;
    using WONCommon::AutoCrit;
    using WONCommon::StringToWString;
    using WONCommon::WStringToString;
//    using WONCommon::WONException;
	using WONMsg::DirEntityList;
//	using WONMsg::TMsgDirStatusReply;
	using WONMsg::SMsgDirG2EntityListBase;
	using WONMsg::BaseMessage;
	using WONMsg::DirEntity;
	using WONMsg::SMsgTypeDir;
	using WONMsg::HeaderType;
	using WONMsg::SmallServiceType;
	using WONMsg::TRawMsg;
    using WONMsg::TMessage;
    using WONMsg::SmallMessage;
	using WONCrypt::EGPrivateKey;
	using WONCrypt::EGPublicKey;
	using WONMsg::TMsgAuth1GetPubKeysReply;
	using WONMsg::TMsgAuth1ChallengeHL;
	using WONMsg::TMsgAuthRawBufferBase;
	using WONMsg::TMsgAuth1GetPubKeys;
	using namespace WONMsg;
};
void PrintData (char* data , int Size)
{
	char a , line[17] , c;
	int j;
	
	//loop over each character and print
	for(int i=0 ; i < Size ; i++)
	{
		c = data[i];
		
		//Print the hex value for every character , with a space. Important to make unsigned
		printf(" %.2x", (unsigned char) c);
		//Add the character to data line. Important to make unsigned
		a = ( c >=32 && c <=128) ? (unsigned char) c : '.';
		
		line[i%16] = a;
		
		//if last character of a line , then print the line - 16 characters in 1 line
		if( (i!=0 && (i+1)%16==0) || i == Size - 1)
		{
			line[i%16 + 1] = '\0';
			
			//print a big gap of 10 characters between hex and characters
			printf("\t");
			
			//Print additional spaces for last lines which might be less than 16 characters in length
			for( j = strlen(line) ; j < 16; j++)
			{
				printf( "   ");
			}
			
			printf("%s \n" , line);
		}
	}
	
	printf("\n");
}
int main(int argc, char* argv[])
{
	const unsigned short SERVER_PORT = 27015;

	TCPSocket aSocket;
	aSocket.Listen(SERVER_PORT);

	while (TRUE)
	{	
		printf("Waiting for connections on port %d.\n", SERVER_PORT);

		// accept the next connection
		TCPSocket* aClientCommSocketP = aSocket.Accept();
		printf("Accepted connection from %s.\n", aClientCommSocketP->GetRemoteAddress().GetAddressString().c_str());
	 
		TMsgAuth1LoginRequestHL BufP;
		// receive client's Auth1Request
		unsigned short aRequestBufLen = 0;
		void* aRequestBufP = new unsigned char[aRequestBufLen];
		aClientCommSocketP->Recv(2, &aRequestBufLen);
		aClientCommSocketP->Recv(aRequestBufLen, aRequestBufP);	

		BufP.SetRawBuf((unsigned const char*) aRequestBufP, aRequestBufLen);
		BufP.Dump(cout);
		printf("Received request. %s\n", aRequestBufP);
		PrintData((char*)aRequestBufP, aRequestBufLen);

/*

 aHeaderType=5;
 aServiceType=12;
	aMessageType=2; // GetPubKeysReply
*/
	SMsgDirG2StatusReply amsg;
	amsg.SetStatus(StatusSuccessCommonStart);
	amsg.Pack();
			aClientCommSocketP->Send(amsg.GetDataLen(), amsg.GetDataPtr());
//		aClientCommSocketP->Send(sizeof msg, msg);
	//	printf("Sent server->client challenge\n");
		amsg.Dump(cout);
		//aClientCommSocketP->Send(2, "ass");l
//.		buff->Dump(cout);
	

	//	getch();
	}

	return 0;
}
