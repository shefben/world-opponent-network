// Server.cpp : Defines the entry point for the console application.
// Main() Creates the socket and waits for a client to connect, 
// then routes him off to the thread he is requesting
#include "stdafx.h"
#include "Server.h"
#include "socketTools.h"
#include <stdio.h>
#include <conio.h>


int main(int argc, char* argv[])
{
	printf("Initializing WON.NET Server Manager\n\n");

	ServerManager * serverManager; 
	serverManager = new ServerManager();
	serverManager->startServers();

  if(getchar() == 27)
	 return(0);
}
