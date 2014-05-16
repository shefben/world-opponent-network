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

namespace {
using WONCrypt::EGPrivateKey;
using WONCrypt::EGPublicKey;
}
using namespace CryptoPP;
using namespace WONCrypt;
using namespace WONAuth;
EGPrivateKey* myPrivKey = new EGPrivateKey(8);
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
		
		PrintData(buf, res);


		EGPrivateKey aKey1(12);
		/*WON_AuthCertificate1* aCert = WON_AuthFactory::NewAuthCertificate1();
		aCert->SetLifespan(time(NULL), 3600);
		aCert->SetUserId(10);
		aCert->SetCommunityId(23);
		aCert->SetTrustLevel(99);
		aCert->SetPublicKey(aKey1.GetPublicKey().GetKey(), aKey1.GetPublicKey().GetKeyLen());
		aCert->Pack() */

		Auth1PublicKeyBlock aBlock(1);
		aBlock.SetLifespan(time(NULL), 3600);
		aBlock.KeyList().push_back(dynamic_cast<const EGPublicKey&>(aKey1.GetPublicKey()));
//		aBlock.PackData();

		strcpy(sendData, "xCA\x02\x02"); 
		strcat(sendData, (const char*)aBlock.GetData());

		//send pkey packet
		send(current_client, sendData, 0, 0);
/* encrypt / decryption */
	//	EGPublicKey::CryptReturn  encrypt(myPubKey->Encrypt(reinterpret_cast<unsigned char*>(iStr), strlen(iStr)+1));
	//	EGPrivateKey::CryptReturn decrypt(myPrivKey->Decrypt(encrypt.first, encrypt.second));
	//	delete encrypt.first;  delete decrypt.first;
	//	delete myPrivKey;
	//	delete myPubKey;
	//	
/* Signing packets */
	//	EGPrivateKey::CryptReturn sig(myPrivKey->Sign(reinterpret_cast<unsigned char*>(iStr), strlen(iStr)+1));
	//	int tstSig = myPubKey->Verify(sig.first, sig.second, reinterpret_cast<unsigned char*>(iStr), strlen(iStr)+1);

		shutdown(current_client, 2);
		closesocket(current_client);	
		// clear buffers
		strcpy(sendData,"");
		strcpy(buf,"");
	}
}
