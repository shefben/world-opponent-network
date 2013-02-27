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
Directoryserver.cpp commented code- incorrectly used
//WONCommon::DataObjectTypeSet aDOSet;
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