/********************************************************************
	created:	2012/04/13
	filename: 	c:\SIERRA\Development\Newer WonApi\Samples\ServerTest\DirectoryServer.cpp
	author:		Benjamin Shefte 
		Code from Steamcooker
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include "DirectoryServer.h"
//#include <msg/ServerStatus.h>
#include <msg/TMessage.h> //common packet building and other message stuff
#include <msg/HeaderTypes.h> //identifier header for message type ie encrypted or not
#include <msg/SServiceTypes.h> //smallmessage service type - i.e. directory server routing server etc
#include <msg/Dir/SMsgTypesDir.h> //smallmessage message type - packet requests + replys
#include <msg/dir/Direntity.h> //class for server and directory entitys
#include <msg/dir/DirG2Flags.h> //flags used to request entities
//#include <msg/dir/DirACLs.h>
namespace {
//	using WONCommon::DataObject;
//	using WONCommon::DataObjectTypeSet;
//	using WONAuth::Permission;
//	using WONAuth::PermissionACL;
//	using WONMsg::DirACLList;
	using WONMsg::BaseMessage;
	using WONMsg::DirEntity;
};



DirectoryServer::DirectoryServer(int port)
{
	sprintf(name,"DirectoryServer(%d)",port);

	this->port=port;
}

DirectoryServer::~DirectoryServer()
{
}

int DirectoryServer::start()
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
		listeningSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_IP );

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

		int result = !_beginthreadex( 0,0, &DirectoryServer::waitForConnections, this, 0, &dummy); 

		if (result)
		{ 
			printf("[%s] Failed to start server thread", name);
		} 
		else
		{
			printf("[Directory] Running on port: %d\n", port);
		}
	return result;
}


unsigned int DirectoryServer::waitForConnections(void * param)
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
		connectedSocket = accept( ((DirectoryServer*)param)->listeningSocket, (sockaddr*)&addr, &sockaddrlen);

		printf("[DIR]Client connected\r\n");
				   
		// create our recv_cmds thread and parse client socket as a parameter
		CreateThread(NULL, 0,dirRecv,(LPVOID)connectedSocket, 0, &thread); 
	 }		
	return 0;
}

DWORD WINAPI dirRecv(LPVOID lpParam) 
{
	printf("[DIR] thread created\r\n");
	
	// set our socket to the socket passed in as a parameter   
	SOCKET current_client = (SOCKET)lpParam; 
	
	// buffer to hold our recived data
	char buf[100];
	// buffer to hold our sent data
	char sendData[100];
	// for error checking 
	int res;

	// our recv loop
	while(true) 
	{
		res = recv(current_client,buf,sizeof(buf),0); // recv cmds
		
		Sleep(10);
		 
		
		if(res == 0)
		{
			printf("[DIR] Error with connection");
			closesocket(current_client);
			ExitThread(0);
		}

		printf("[DIR] Recieved packet\n length: %d\t information:\n",res);
		
		PrintData(buf, res);

//The first directory packet from a client is authserver list and/or validversion dataobject
		//so for now we are going to send a default packet that contains the authservs service entity
		DirEntity* dirEntity;
		dirEntity->mType = 'S';

		/*	DirectoryData* dirData = new DirectoryData;
		 *				dirData->opType = op_findService;
		dirData->resultType = multi_result;
		dirData->error = Error_Timeout;
		dirData->path = path.GetUnicodeString();
		dirData->name = name.GetUnicodeString();
		dirData->displayName = displayName.GetUnicodeString();
		dirData->timeout = timeout;
		dirData->autoDelete = async;
		dirData->dirEntityListResultCompletion = completion;
		dirData->doService = true;
		dirData->destServer = foundOnDirServer;
		dirData->callback = callback;
		dirData->callbackPrivData = callbackPrivData;
		dirData->findMatchMode = findMatchMode;
		dirData->findFlags = findFlags;
		dirData->dataObjects = dataObjects;
		dirData->getFlags = getFlags;
		
		  dirData->entityList = result ? result : &(dirData->tempEntityList);
		  
			for (unsigned int i = 0; i < numAddrs; i++)
			dirData->directories.push_back(directoryServers[i]);
			
			  dirData->curDirectory = dirData->directories.begin();
*/ 
		//SMsgDirG2EntityBase directory message!!!
		SetServiceType(WONMsg::SmallDirServerG2);
		SetMessageType(WONMsg::DirG2DirectorySetDataObjects);
		SMsgDirG2EntityBase::Pack();


		//send(current_client, (char *)ptr, res, 0);

		//printf("Sent auth server info\n");
	
		// clear buffers
		strcpy(sendData,"");
		strcpy(buf,"");
	}
}   
