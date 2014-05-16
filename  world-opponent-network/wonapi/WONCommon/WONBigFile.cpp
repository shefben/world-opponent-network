#define __WON_MASTER_CPP__

#include "WONBigFile.h"
#include "FileReader.h"

using namespace WONAPI;

const unsigned long BIGFILE_VERSION = 1;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONBigFile::Init()
{
	mMemDataP = NULL;
	mOwnMemData = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONBigFile::WONBigFile(const const char *theFilePath)
{
	Init();

	mFilePath = theFilePath;

	FileReader aReader(theFilePath);
	Read(aReader);
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONBigFile::WONBigFile(const void *theData, unsigned long theLength, bool ownData)
{
	Init();
	
	mMemDataP = (char*)theData;
	mOwnMemData = ownData;

	FileReader aReader(theData,theLength);
	Read(aReader);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifdef WIN32_NOT_XBOX
WONBigFile::WONBigFile(const char *theResName, const char *theResType)
{
	Init();

	HMODULE aModule = GetModuleHandle(NULL);

	HRSRC aResource = FindResource(aModule,theResName,theResType);
	if(aResource==NULL)
		return;

	HGLOBAL aGlobal = LoadResource(aModule,aResource);
	if(aGlobal==NULL)
		return;

	mMemDataP = (char*)LockResource(aGlobal);
	if(mMemDataP==NULL)
		return;

	DWORD aMemSize = SizeofResource(aModule, aResource);
	FileReader aReader;
	aReader.Open(mMemDataP,aMemSize);
	Read(aReader);
}
#endif // WIN32_NOT_XBOX

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONBigFile::~WONBigFile()
{
	if(mOwnMemData)
		delete [] mMemDataP;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONBigFile::Read(FileReader &theReader)
{
	if(!theReader.IsOpen())
		return false;

	unsigned long aFileLength = theReader.length();
	try
	{
		char aSig[4];
		theReader.ReadBytes(aSig,3);
		aSig[3] = '\0';
		if(strcmp(aSig,"WBF")!=0)
			return false;

		unsigned long aVersion = theReader.ReadLong();
		if(aVersion!=BIGFILE_VERSION)
			return false;

		int aNumEntries = theReader.ReadLong();

		unsigned long aTablePos = theReader.ReadLong();
		theReader.SetPos(aTablePos);

		for(int i=0; i<aNumEntries; i++)
		{
			std::string aPath;
			unsigned long aStartPos, aLength;
			theReader.ReadString(aPath);
			aStartPos = theReader.ReadLong();
			aLength = theReader.ReadLong();

			if(aStartPos + aLength > aFileLength)
				return false;

			mFileMap[aPath] = FilePiece(aStartPos,aLength);
		}	
	}
	catch(FileReaderException&)
	{
		return false;
	}

	return true;
}
