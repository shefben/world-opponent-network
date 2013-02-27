#if 0
#include "stdafx.h"
#include "FactoryServer.h"
#include <stdio.h>
#include <process.h>

FactoryServer::FactoryServer(int port)
{
	sprintf(name,"FactoryServer(%d)",port);
	status=SERVER_STATUS_STOPPED;
	this->port=port;
}

FactoryServer::~FactoryServer()
{
}

int FactoryServer::start()
{
	status=SERVER_STATUS_ERROR;
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
	int result = !_beginthreadex( 0,0, &FactoryServer::waitForConnections, this, 0, &dummy); 

	if (result)
	{ 
		printf("[%s] Failed to start server thread", name);
	} 
	else
	{
		status=SERVER_STATUS_RUNNING;
		printf("[%s] Running on port: %d", name, port);
	}
	return result;
}
#endif