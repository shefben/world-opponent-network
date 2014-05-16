/********************************************************************
	created:	2012/04/13
	filename: 	c:\SIERRA\Development\Newer WonApi\Samples\ServerTest\DirectoryServer.cpp
	author:		Benjamin Shefte 
		Code from Steamcooker
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include "DirectoryServer.h"
#include <msg/TMessage.h> //Buffer / Message backbone for whole API
#include <msg/HeaderTypes.h> //identifier header for message type ie encrypted or not
#include <msg/SServiceTypes.h> //smallmessage service type - i.e. directory server, routing server etc
#include <msg/Dir/SMsgTypesDir.h> //smallmessage message type - packet requests + replys
#include "msg/Dir/DirEntity.h" //class for server and directory entitys
#include <msg/dir/DirG2Flags.h> //flags used to request entities
#include "msg/Dir/SMsgDirG2MultiEntityReply.h"
#include "msg/Dir/SMsgDirG2SingleEntityReply.h" //Single Entity Packet functions
#include "msg/ServerStatus.h"

#include <stdio.h>
#include <memory.h>
namespace {
	using WONMsg::SMsgDirG2EntityListBase;
	using WONMsg::BaseMessage;
	using WONMsg::DirEntity;
	using WONMsg::TRawMsg;
	using WONMsg::SMsgTypeDir;
	using WONMsg::HeaderType;
	using WONMsg::SmallServiceType;
};
using namespace WONMsg;
using namespace WONCommon;

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
		 
		if(res <= 0)
		{
			printf("[DIR] closing connection\n");
			closesocket(current_client);
			ExitThread(0);
		}
		
		printf("[DIR] Recieved registration packet\n length: %d\t information:\n",res);
		PrintData(buf, res);
		
		WONCommon::RawBuffer rbuf;
		SMsgDirG2MultiEntityReply msg;

		SMsgDirG2MultiEntityReply msg(theMsgR);
        DirEntityList& dirEntities = msg.Entities();
(HWONDATAOBJECT)new DataObject(DataObject::DataType((unsigned char*)type, typeLen), DataObject::Data((unsigned char*)data, dataLen));

		// Data is the valid versions
		/*WONCommon::DataObjectTypeSet data;
		static const WONCommon::RawBuffer dataValidVersions = (U8 *) "1110	1441";
		data.insert(WONCommon::DataObject(dataValidVersions));
		*/
		msg.Pack();
		rbuf.assign((unsigned char*)msg.GetDataPtr(), msg.GetDataLen());

		/* //This is to form the packet as <packed size> 00 00 00 <buffer>
		memset(sendData, rbuf.size(), 1); //set packet size
		memset(sendData + 1, '\0', 3); //3 null bytes
		memcpy(sendData + 4,msg.GetDataPtr(), rbuf.size() ); //set rawbuffer
	
		  
		PrintData(sendData, rbuf.size()+ 4);
		send(current_client, sendData, sizeof sendData, 0);  */

		send(current_client, (const char*)rbuf.data(), rbuf.size(), 0);
		PrintData((char*)rbuf.data(), rbuf.size());
		closesocket(current_client);	

		// clear buffers
		strcpy(sendData,"");
		strcpy(buf,"");
	}
}   

		/*	//This is for registering coolpool server with directory server
			if(count == 1)
				sendSocket(current_client, "0b 00 00 00 05 02 00 01 00 00 00");
			if(count == 2)
				sendSocket(current_client,"0b 00 00 00 05 02 00 01 00 4e fb");
			if(count == 3)
				sendSocket(current_client, "0b 00 00 00 05 02 00 01 00 00 00");
		*/

/*
// find directory entity and grab valid versions object from it
WONMsg::DirEntityList::iterator aEntityItr = dirEntities.begin();
for ( ; aEntityItr != dirEntities.end(); ++aEntityItr)
{
    if (aEntityItr->mType == WONMsg::DirEntity::ET_DIRECTORY)
    {
        WONCommon::DataObjectTypeSet::iterator aObjItr = aEntityItr->mDataObjects.find(WONCommon::DataObject(VALIDVERSIONS_OBJ));
        if (aObjItr != aEntityItr->mDataObjects.end())
        {
            titanGotValidVersionStrings(const_cast<char*>(reinterpret_cast<const char*>(aObjItr->GetData().c_str())));
            break;
        }
    }
} 
*/