========================================================================
       CONSOLE APPLICATION : Servertest
========================================================================


Steps:

1.) Create A MultiThreaded TCP (or udp) socket server using WONAPI for:
	a. Authentication
	b. Directory
2.) Create basic server functionality with wonapi
	a. Authentication
		1. Use authtest for authentication procedure
			a. refer to Hl message c files
	b. Directory
		1. First server called for games
			a. Hold only local server IPs (just for testing)
		2. Refer to Silencer routing server

Server insutrctions:

Authentication:

// Step 1: recieve client Auth1Request
// Step 2: process Auth1Request 
//		   generate response (Auth1Challenge1 or Auth1Complete)
// Step 3: receive Auth1Challenge2
// Step 4: process Auth1Challenge2
//         generate & send Auth1Complete

Routing:  refer to homeworld source and hwds

Directory: refer to homeworld source and hwds

Factory:


/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named Servertest.pch and a precompiled types file named StdAfx.obj.


/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////
