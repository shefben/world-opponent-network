#include "stdafx.h"
#include "RoutingServer.h"
#include <process.h>
#include <string.h>
#include <stdlib.h>

RoutingServer::RoutingServer(int port)
{
	sprintf(name,"AuthServer(%d)",port);
	status=SERVER_STATUS_STOPPED;
	this->port=port;
}

RoutingServer::~RoutingServer()
{
}
char * RoutingServer::getName(){
	return name;
}
int RoutingServer::getStatus()
{
	return status;
}
int RoutingServer::start()
{

	status=SERVER_STATUS_ERROR;
	// Initialize Winsock.
	WSADATA wsaData;
	int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
	if ( iResult != NO_ERROR )
	{
		printf("[%s] Winsock failed to initialize\n", name);
		return iResult;
	}
	// Create a socket.
	listeningSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_IP );

	if ( listeningSocket == INVALID_SOCKET ) {
		WSACleanup();
		printf("[%s] Failed to create Socket\n", name); 
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
		printf("[%s] failed to bind socket to port %d\n", name, port);
		return -1;
	}

	if ( listen( listeningSocket, 10 ) == SOCKET_ERROR )
	{ 
		printf("[%s] Failed to listen on socket\n", name);
		return -1;
	}

	unsigned int dummy;
	//int result = 

	if ( !_beginthreadex( 0,0, &RoutingServer::waitForConnections, this, 0, &dummy) )
	{ 
		printf("[%s] Failed to start server thread\n", name);
		return -1;
	} 
	else
	{
		status=SERVER_STATUS_RUNNING;;

		printf("[Authentication] Running on port: %d\n", port);
	}
	return 1;
}

unsigned int RoutingServer::waitForConnections(void * param)
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
		connectedSocket = accept( ((RoutingServer*)param)->listeningSocket, (sockaddr*)&addr, &sockaddrlen);

		printf("Client connected\r\n");
				   
		// create our recv_cmds thread and parse client socket as a parameter
		CreateThread(NULL, 0,receive_cmds,(LPVOID)connectedSocket, 0, &thread); 
	 }		
	return 0;
}

DWORD WINAPI receive_cmds(LPVOID lpParam) 
{
	printf("thread created\r\n");
	
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
			printf("Error with connection");
			closesocket(current_client);
			ExitThread(0);
		}
		printf("hex:\t %#x\n", buf);
		printf("Recieved:\t %s\n", buf);
		/*
		if(strstr(buf,"hello"))
		{ // greet this user
			printf("\nrecived hello cmd");
			
			strcpy(sendData,"hello, greetz from KOrUPt\n");
			Sleep(10);
			send(current_client,sendData,sizeof(sendData),0); 
		}								
		else if(strstr(buf,"bye"))   
		{ // dissconnected this user
			printf("\nrecived bye cmd\n");
			
			strcpy(sendData,"cya\n");
			Sleep(10);
			send(current_client,sendData,sizeof(sendData),0);
			
			// close the socket associted with this client and end this thread
			closesocket(current_client);
			ExitThread(0);
		}
		else
		{
			strcpy(sendData,"Invalid cmd\n");
			Sleep(10);
			send(current_client,sendData,sizeof(sendData),0);
		} 		
		*/

		// clear buffers
		strcpy(sendData,"");
		strcpy(buf,"");
	}
}   

	void PrintUnknownPacket(const unsigned char * buffer,  unsigned int bytesreceived) {
		
		unsigned int i = 0;
		char lineinhex[80] = "";
		char lineintext[80] = "";
		char temp[5] = "";
		
		while(i < bytesreceived) {
			sprintf(temp, "%2x ", buffer[i]);
			strcat(lineinhex, temp);
			if(isprint(buffer[i])) {
				strncat(lineintext, (const char *)buffer+i, 1);
			}
			else {
				strcat(lineintext, ".");
			}
			i++;
			if(i == bytesreceived) {
				while(i%10 != 0) {
					strcat(lineinhex, "   ");
					strcat(lineintext, " ");
					i++;
				}
			}
			if(i%10 == 0) {
				printf("0x %s %s | %i\n", lineinhex, lineintext, i);
				lineinhex[0] = '\0';
				lineintext[0] = '\0';
			}
		}
	}

/*
void RoutingServer::doConnect(SOCKET socket,DWORD clientIp, char * serverName)
{
	printf("RoutingServer : incoming packet:\n");
	
	/* if( time = 300sec)
	 * waitForConnections
	 
    char buffer[1000];
	recv(socket,buffer,1000,0);

	//	itoa(x, buffer, 10);
		printf("%.2x ", *buffer);

    shutdown(socket, 0);
	closesocket(socket);
	_endthreadex(0);

}
*/
/*
void Stringtest()
{
	
	char portch[33];
	itoa(port,portch,10);
	int len = strlen(name);
	int plen = strlen(portch);
	
	char* finalname = name + len - (plen+1);
	printf("String test: found the port to be %s\n", finalname );
	
}*/


