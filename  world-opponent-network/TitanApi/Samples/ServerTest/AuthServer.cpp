/********************************************************************
	created:	2012/04/15
	filename: 	c:\SIERRA\Development\Newer WonApi\Samples\ServerTest\AuthServer.cpp
	author:		Benjamin Shefte

	purpose:	Authentication Server class
*********************************************************************/
#include "stdafx.h"
#include "AuthServer.h"
#include <crypt/EGPrivateKey.h>
#include <common/won.h>
#include <crypt/EGPublicKey.h>
#include <msg/Auth/TMsgAuth1PeerToPeer.h>
#include <crypt/Randomizer.h>
#include "msg/tmessage.h"
#include "auth/WON_AuthCertificate1.h"
#include "auth/WON_AuthFactory.h"
#include "auth/Auth1PublicKeyBlock.h"
#include "auth/AuthPublicKeyBlockBase.h"
#include "msg/Auth/TMsgAuth1GetPubKeys.h"
#include <OSTREAM>
#include "msg/Auth/TMsgTypesAuth.h"
#include "msg/Auth/TMsgAuth1LoginHL.h"
#include "msg/HeaderTypes.h"
#include <iostream>
#include "msg/Auth/TMsgAuth1LoginReplyHL.h"

namespace {
using WONCrypt::EGPrivateKey;
using WONCrypt::EGPublicKey;
using WONMsg::TMsgAuth1GetPubKeysReply;
using WONMsg::TMsgAuth1ChallengeHL;
using WONMsg::TMsgAuthRawBufferBase;
	using WONMsg::TMsgAuth1GetPubKeys;
	using namespace WONMsg;
}
using namespace CryptoPP;
using namespace WONCrypt;
using namespace WONAuth;
using namespace std;
EGPrivateKey* myPrivKey = new EGPrivateKey(12);
EGPublicKey*  myPubKey  = new EGPublicKey(*myPrivKey);

AuthServer::AuthServer(int port)
{
	sprintf(name,"AuthServer(%d)",port);
	this->port=port;

}

AuthServer::~AuthServer()
{

}

int AuthServer::start()
{
	// Initialize Winsock.
	WSADATA wsaData;
	int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
	if ( iResult != NO_ERROR )
	{
		printf("[%s] Winsock failed to initialize", name);
		return iResult;
	}
	// Create a socket.
	listeningSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP  );

	if ( listeningSocket == INVALID_SOCKET ) {
		WSACleanup();
		printf("[%s] Failed to create Socket", name); 
		return -1;
	}

	// Bind the socket.
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr =  ADDR_ANY ;
	service.sin_port = htons(port);

	if ( bind( listeningSocket, (SOCKADDR*) &service, sizeof(service) ) == SOCKET_ERROR ) 
	{
		closesocket(listeningSocket);	
		printf("[%s] failed to bind socket to port %d", name, port);
		return -1;
	}

	if ( listen( listeningSocket, 10 ) == SOCKET_ERROR )
	{ 
		printf("[%s] Failed to listen on socket", name);
		return -1;
	}

	unsigned int dummy;
	//int result = 

	if ( !_beginthreadex( 0,0, &AuthServer::waitForConnections, this, 0, &dummy) )
	{ 
		printf("[%s] Failed to start server thread\n", name);
		return -1;
	} 
	else
	{
		printf("[Authentication] Running on port: %d\n", port);
	}
	return 1;
}
unsigned int AuthServer::waitForConnections(void * param)
{
	SOCKET connectedSocket;
	DWORD thread;
	connectedSocket = SOCKET_ERROR;
	sockaddr_in addr;
    int sockaddrlen=sizeof(struct sockaddr_in);

	 // loop forever
	while(true)
	 {
	    // accept connections
		connectedSocket = accept( ((AuthServer*)param)->listeningSocket, (sockaddr*)&addr, &sockaddrlen);

		printf("[AUTH] Client connected\r\n");
				   
		// create our recv_cmds thread and parse client socket as a parameter
		CreateThread(NULL, 0,authRecv,(LPVOID)connectedSocket, 0, &thread); 
	 }		
	return 0;
}

DWORD WINAPI authRecv(LPVOID lpParam) 
{
	printf("[AUTH] thread created\r\n");
	
	// set our socket to the socket passed in as a parameter   
	SOCKET current_client = (SOCKET)lpParam; 
	
	// buffer to hold our recived data
	char buf[256];
	// buffer to hold our sent data
	char sendData[256];
	// for error checking 
	int res;
	// our recv loop
	while(true) 
	{

		res = recv(current_client,buf,sizeof(buf),0); // recv cmds
		Sleep(10);
		 
		if(res <= 0)
		{
			printf("[AUTH] Error with connection");
			closesocket(current_client);
			ExitThread(0);
		}

		printf("[AUTH] Recieved packet\n length: %d\t information:\n",res);
		
		
		TMsgAuth1LoginRequestHL buf;
		buf.Dump(cout);
	//	buf.Unpack();yh
		cout<< buf.GetNeedKey();
		printf("\n");
		cout<< buf.GetKeyBlockId();
		printf("\n");
		PrintData((char*)buf.GetDataPtr(), res);

	//	TMsgAuthRawBufferBase mbuffer;
		TMsgAuth1LoginBase2 msg;
//		Auth1PublicKeyBlock(msg.GetRawBuf(),msg.GetRawBufLen());

		EGPrivateKey aKey1(12);
		Auth1PublicKeyBlock aBlock(1); 

	/*	aBlock.SetBlockId(1);
		aBlock.SetLifespan(time(NULL), 3600);
		aBlock.KeyList().push_back(dynamic_cast<const EGPublicKey&>(aKey1.GetPublicKey()));
		aBlock.Pack(aKey1); 
		
		mbuffer.SetRawBuf(aBlock.GetData(),aBlock.GetDataLen());
		mbuffer.Pack();

		msg.SetRawBuf(mbuffer.GetRawBuf(),mbuffer.GetRawBufLen());
	*/

		msg.Pack();
		msg.Dump(cout);
		
	//	PrintData((char*)msg.GetDataPtr(),msg.GetDataLen());
		send(current_client, (const char*)msg.GetBodyPtr(), (int)msg.GetDataLen(),0);//sendData, sizeof sendData, 0);  
		

		shutdown(current_client, 2);
		closesocket(current_client);	
		// clear buffers
		strcpy(sendData,"");
	//	strcpy(buf,"");
	}
}

/*A1.1.1 - private / public key functions
aKeySet.insert(new EGPrivateKey(CryptKeyBase::KEYLEN_DEF));
aKeySet.insert(new EGPrivateKey(4));
EGPrivateKey aPvKey(12);
EGPrivateKey aPvKey1(0);
aKeySet.insert(new EGPrivateKey(aPvKey.GetKeyLen(), aPvKey.GetKey()));
aKeySet.insert(new EGPrivateKey(aPvKey1));

		const EGPublicKey& aPubKey = dynamic_cast<const EGPublicKey&>(aPvKey1.GetPublicKey());
		aKeySet.insert(new EGPublicKey(aPvKey));
		aKeySet.insert(new EGPublicKey(aPubKey));
*/
