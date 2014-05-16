/********************************************************************
	created:	2012/04/13
	filename: 	c:\SIERRA\Development\Newer WonApi\Samples\ServerTest\DirectoryServer.cpp
	author:		Benjamin Shefte 
		Servermanager code from Steamcooker
	purpose:	
*********************************************************************/

#include "stdafx.h"
#include <stdio.h>
#include <memory.h>
#include <string>
#include <process.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include "DirectoryServer.h"
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
#include "common/ThreadBase.h"
#include "common/WONException.h"
#include "common/won.h"
#include "common/DataObject.h"
#include "db/dbconstants.h"
namespace {
	using namespace WONDatabase;
	using namespace std;
	using namespace WONAPI;
	using namespace WONMsg;
	using namespace WONCommon;
    using WONCommon::AutoCrit;
    using WONCommon::StringToWString;
    using WONCommon::WStringToString;
    using WONCommon::WONException;
	using WONMsg::DirEntityList;
	using WONMsg::SMsgDirG2EntityListBase;
	using WONMsg::BaseMessage;
	using WONMsg::DirEntity;
	using WONMsg::SMsgTypeDir;
	using WONMsg::HeaderType;
	using WONMsg::SmallServiceType;
	using WONMsg::TRawMsg;
    using WONMsg::TMessage;
    using WONMsg::SmallMessage;
};

const wchar_t* AUTH_SERV     = L"AuthServer"; // Service name for auth servers
const wchar_t* CHATROOM_SERV = L"TitanRoutingServer"; // Service name for chat rooms (chat Routing Servers)
const wchar_t* FACTORY_SERV  = L"TitanFactoryServer"; // Service name for factory servers
const wchar_t* FIREWALL_SERV = L"TitanFirewallDetector"; // Service name for firewall servers
const wchar_t* EVENT_SERV    = L"TitanEventServer"; // Service name for event servers
const WONCommon::RawBuffer VALIDVERSIONS_OBJ(reinterpret_cast<unsigned char*>("SampleValidVersions")); // Data object that contains valid Homeworld version strings    (attached to TitanServers dir)

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
		
		//Sleep(10);
		 
		if(res <= 0)
		{
			printf("[DIR] closing connection\n");
			closesocket(current_client);
			ExitThread(0);
		}
		
		// ,printf("[DIR] Recieved registration packet\n length: %d\t information:\n",res);
		PrintData(buf, res); //print recieved packet, ascii and hex
		WONCommon::RawBuffer rbuf;

		SMsgDirG2MultiEntityReply msg;

		msg.Pack();
//		msg.Append_PW_STRING(mPath);
		msg.Append_PW_STRING(AUTH_SERV);
		char myip[14] = "192.168.1.115";
	/*	msg.AppendByte(5); //small mssg
		msg.AppendShort(2); //dir server
		msg.AppendShort(3); //multi entity reply
		msg.AppendShort(0); //status
		msg.AppendByte(0); //sequence 
		msg.AppendLong(0x00000010 | 0x04000000 );//flags
		msg.AppendShort(1);//number of entities
		msg.AppendByte('S'); //S = service D= directory
		msg.AppendByte(14); //size of address
		msg.AppendBytes(14,myip);//actual address 
*/
		//msg.Pack();

/*
		filebuf fb;
		fb.open ("test.txt",ios::out);
		ostream os(&fb);
		msg.Dump(os);//This code is used to dump all the information from the msg, and anything else, just call <object>.dump(os);
		fb.close();

*/
		 //This is to form the packet as <packed size> 00 00 00 <flags> <buffer>
		/*memset(sendData, msg.GetDataLen(), 1); //set packet size
		memset(sendData + 1, '\0', 3); //3 null bytes
		memcpy(sendData + 4,msg.GetDataPtr(), msg.GetDataLen() ); //set rawbuffer

		PrintData((char*)msg.GetDataPtr(),msg.GetDataLen());
		send(current_client, (const char*)msg.GetBodyPtr(), (int)msg.GetDataLen(),0);//sendData, sizeof sendData, 0);  
*/

		PrintData((char*)msg.GetDataPtr(),msg.GetDataLen());
		send(current_client, (const char*)msg.GetBodyPtr(), msg.GetDataLen(),0);//sendData, sizeof sendData, 0);  

	}


	// clear buffers
	strcpy(sendData,"");
	strcpy(buf,"");
}   
//anObject.mDataType=="_cs" //checksum
//anObject.mDataType=="_ps" // size
//A1.1.1 - Coolpool dir server registration reply
/*	//This is for registering coolpool server with directory server
	if(count == 1)
		sendSocket(current_client, "0b 00 00 00 05 02 00 01 00 00 00");
	if(count == 2)
		sendSocket(current_client,"0b 00 00 00 05 02 00 01 00 4e fb");
	if(count == 3)
		sendSocket(current_client, "0b 00 00 00 05 02 00 01 00 00 00");
*/

		//WONCommon::DataObjectTypeSet aDOSet;
		/* ValidVersion */
		// (short) number of dataobjects -	1
		// (byte)length of type
		// (string) dataobject type  - <game>validversion 
		// (short) length of data
		// (Rawbuffer/String) dataobject data - versions seperated by \t
		//msg.SetDataTypes(VALIDVERSIONS_OBJ.c_str(), VALIDVERSIONS_OBJ.size());
		//aDOSet.insert(WONCommon::DataObject(VALIDVERSIONS_OBJ, WONCommon::RawBuffer(dataValidVersions)));

		/*	
		WONMsg::DirEntityList dirEntities;
		WONMsg::DirEntityList::iterator aEntityItr = dirEntities.begin();

		aEntityItr->mType = WONMsg::DirEntity::ET_DIRECTORY;
		WONCommon::DataObjectTypeSet::iterator aObjItr = aEntityItr->mDataObjects.begin();
		//aObjItr = aEntityItr->mDataObjects.insert(WONCommon::DataObject(VALIDVERSIONS_OBJ, WONCommon::RawBuffer(dataValidVersions)));
		aObjItr->SetDataType(VALIDVERSIONS_OBJ);
		aObjItr->SetData(dataValidVersions);
		aObjItr++;
		aObjItr = aEntityItr->mDataObjects.end();
		aEntityItr++;


		aEntityItr->mType = WONMsg::DirEntity::ET_SERVICE;
		aEntityItr->mName = AUTH_SERV;
		aEntityItr++;

		aEntityItr = dirEntities.end();
		*/
		/*
		unsigned char aTypeLen = VALIDVERSIONS_OBJ.size();
		msg.AppendByte(aTypeLen);
		msg.AppendBytes(aTypeLen, VALIDVERSIONS_OBJ.data());
		unsigned char aTypeLen2 = dataValidVersions.size();
		msg.AppendByte(aTypeLen2);
		msg.AppendBytes(aTypeLen2, dataValidVersions.data());
		*/

/* //Dataobject w/ appending function snipit from avp wonapi
AppendShort(mDataObjects.size());
WONCommon::DataObjectTypeSet::iterator anItr(mDataObjects.begin());
for (; anItr != mDataObjects.end(); anItr++)
{
	unsigned char aTypeLen = anItr->GetDataType().size();
	AppendByte(aTypeLen);
	if (aTypeLen > 0)
		AppendBytes(aTypeLen, anItr->GetDataType().data());
	 unsigned short aDataLen = anItr->GetData().size();
	 AppendShort(aDataLen);
	 if (aDataLen > 0)
		AppendBytes(aDataLen, anItr->GetData().data());
}
	*/
//A1.1.2 - Factory Server information receiving snipit -  homeworld function
/* 
if (aEntityItr->mName == FACTORY_SERV)
{
// save server name
wcsncpy(tpServerList.tpServers[i].ServerName, aEntityItr->mDisplayName.c_str(), MAX_SERVER_NAME_LEN-1);
tpServerList.tpServers[i].ServerName[MAX_SERVER_NAME_LEN-1] = L'\0';

  // unpack data objects (flags & description)
  tpServerList.tpServers[i].flags = 0;
  tpServerList.tpServers[i].reliability = 0;
  tpServerList.tpServers[i].ping = 0;
  WONCommon::DataObjectTypeSet::iterator aObjItr = aEntityItr->mDataObjects.begin();
  for (; aObjItr != aEntityItr->mDataObjects.end(); ++aObjItr)
  {
  if (aObjItr->GetDataType() == DESCRIPTION_OBJ)
  {
  wcsncpy(tpServerList.tpServers[i].ServerDescription, reinterpret_cast<const wchar_t*>(aObjItr->GetData().data()), MAX_SERVER_DESCRIPTION_LEN-1);
  unsigned int aStringEnd = aObjItr->GetData().size() / 2;
  if (aStringEnd > MAX_SERVER_DESCRIPTION_LEN-1)
  aStringEnd = MAX_SERVER_DESCRIPTION_LEN-1;
  tpServerList.tpServers[i].ServerDescription[aStringEnd] = L'\0';
  }
  else if (aObjItr->GetDataType() == SERVER_UPTIME_OBJ)
  tpServerList.tpServers[i].reliability = *(unsigned long*)aObjItr->GetData().data();
  else if (aObjItr->GetDataType() == FACT_CUR_SERVER_COUNT_OBJ)
  tpServerList.tpServers[i].flags += *(unsigned long*)aObjItr->GetData().data();
  //else if (aObjItr->GetDataType() == FACT_TOTAL_SERVER_COUNT_OBJ)
  //tpServerList.tpServers[i].flags += *(unsigned long*)aObjItr->GetData().data();
                }
*/

//A1.1.3 - Validversion function from homeworld
/*
if (AUTHSERVER_NUM == 0)
{
if (dirEntities.size() > 0)
{
titanDebug("TitanInterface::HandleDirMultiEntityReply Got auth server list and valid version info.");

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
  // handle the auth server services
  HandleTitanServerList(dirEntities);
  if(mNeedToAuthenticateAfterGettingAuthDirectory && AUTHSERVER_NUM>0) {
  mNeedToAuthenticateAfterGettingAuthDirectory = false;
  Authenticate(mLoginName,mPassword,mNewPassword,mCreateAccount);
  }
  }
            }
*/
