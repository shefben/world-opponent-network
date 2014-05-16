#define __WON_MASTER_CPP__

#include "FileReader.h"

#include "CriticalSection.h"
#include "LittleEndian.h"
#include "WONBigFile.h"
#include "WONFile.h"

using namespace WONAPI;

namespace
{
	struct BigFileStruct
	{
		WONBigFilePtr mBigFile;
		unsigned long mStartPos;
		unsigned long mLength;

		BigFileStruct() { } 
		BigFileStruct(WONBigFile *theFile, unsigned long theStartPos, unsigned long theLength) : 
			mBigFile(theFile), mStartPos(theStartPos), mLength(theLength) { }
	};

	typedef std::map<std::string,BigFileStruct,StringLessNoCase> BigFileMap;
	BigFileMap gBigFileMap;
	CriticalSection gBigFileCrit;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FileReader::AddBigFile(WONBigFile *theFile, const char *theBasePath)
{
	AutoCrit aCrit(gBigFileCrit);

	WONBigFilePtr aBigFileRef = theFile; // make sure file is reference counted

	std::string aBasePath;
	if(theBasePath!=NULL)
	{
		aBasePath = theBasePath;
		if(!aBasePath.empty())
		{
			WONFile aPath = aBasePath;
			aBasePath = aPath.GetFilePath();
			if(aBasePath[aBasePath.length()-1]!=WONFile::PATH_SEP)
				aBasePath += WONFile::PATH_SEP;
		}
	}
			
	const WONBigFile::FileMap &aMap = theFile->GetFileMap();
	if(aMap.empty())
		return false;
 
	WONBigFile::FileMap::const_iterator anItr = aMap.begin();
	while(anItr!=aMap.end())
	{
		WONFile aPath = aBasePath + anItr->first;
		gBigFileMap[aPath.GetFilePath()] = BigFileStruct(theFile,anItr->second.mStartPos,anItr->second.mLength);

		++anItr;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FileReader::RemoveBigFile(WONBigFile *theFile)
{
	AutoCrit aCrit(gBigFileCrit);

	BigFileMap::iterator anItr = gBigFileMap.begin();
	while(anItr!=gBigFileMap.end())
	{
		if(anItr->second.mBigFile.get()==theFile)
			gBigFileMap.erase(anItr++);
		else
			++anItr;			
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FileReader::Init()
{
	mFile = NULL;
	mMemDataP = NULL;
	mDataLen = 0;
	mDataPos = 0;
	mDataStartPos = 0;
	mOwnMemData = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FileReader::FileReader()
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FileReader::FileReader(const char *theFilePath)
{
	Init();
	Open(theFilePath);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FileReader::FileReader(const void *theMemDataP, unsigned long theMemDataLen)
{
	Init();
	Open(theMemDataP,theMemDataLen);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FileReader::~FileReader()
{
	Close();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FileReader::CalcFileLength()
{
	if(mFile!=NULL)
	{
		fseek(mFile,0,SEEK_END);
		mDataLen = ftell(mFile);
		rewind(mFile);
	}
	else
		mDataLen = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FileReader::Open(const char *theFilePath)
{
	Close();
	if(theFilePath[0]=='\0') // MS fopen.c asserts if theFilePath is empty
		return false;

	WONFile aFile(theFilePath);
	if(!aFile.Exists())
	{
		aFile.NormalizePath();

		AutoCrit aCrit(gBigFileCrit);
		BigFileMap::iterator anItr = gBigFileMap.find(aFile.GetFilePath());
		if(anItr==gBigFileMap.end())
			return false;

		BigFileStruct &aStruct = anItr->second; 
		WONBigFile *aBigFile = aStruct.mBigFile;
		const char *aMemData = aBigFile->GetMemData();
		if(aMemData!=NULL)
		{
			Open(aMemData + aStruct.mStartPos, aStruct.mLength);
			return true;
		}
		else
		{
			return Open(aBigFile->GetFilePath().c_str(),aStruct.mStartPos,aStruct.mLength);
		}

	}

	mFile = fopen(theFilePath,"rb");
	CalcFileLength();

	return mFile!=NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FileReader::Open(const char *theFilePath, unsigned long theStartPos, unsigned long theLength)
{
	Close();
	if(theFilePath[0]=='\0') // MS fopen.c asserts if theFilePath is empty
		return false;

	mFile = fopen(theFilePath,"rb");
	CalcFileLength();

	if(theStartPos+theLength <= mDataLen)
	{
		mDataStartPos = theStartPos;
		mDataLen = theLength;
		if(mFile!=NULL)
			fseek(mFile,mDataStartPos,SEEK_SET);
	}
	else
		Close();

	return mFile!=NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FileReader::Open(const void *theMemDataP, unsigned long theMemDataLen)
{
	Close();
	mMemDataP = (const char*)theMemDataP;
	mDataLen = theMemDataLen;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FileReader::Close()
{
	if(mMemDataP!=NULL)
	{
		if(mOwnMemData)
			delete [] (char*)mMemDataP;
		
		mMemDataP = NULL;
		mOwnMemData = false;
	}

	if(mFile!=NULL)
	{
		fclose(mFile);
		mFile = NULL;
	}

	mDataLen = 0;
	mDataPos = 0;
	mDataStartPos = 0;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
unsigned long FileReader::pos()
{
	return mDataPos;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
unsigned long FileReader::length()
{
	return mDataLen;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FileReader::SetPos(unsigned long thePos)
{
	if(thePos >= mDataLen)
		throw FileReaderException("Attempt to setpos past end of mem buffer.");

	if(mFile!=NULL)
		fseek(mFile,thePos + mDataStartPos,SEEK_SET);

	mDataPos = thePos;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int FileReader::ReadMaxBytes(void *theBuf, int theMaxBytes)
{
	int aNumBytes = mDataLen - mDataPos;
	if(aNumBytes > theMaxBytes)
		aNumBytes = theMaxBytes;

	if(aNumBytes<=0)
		return 0;

	if(mMemDataP!=NULL)
		memcpy(theBuf, mMemDataP + mDataPos, aNumBytes);
	else
		fread(theBuf,1,theMaxBytes,mFile);
 		
	mDataPos += aNumBytes;
	return aNumBytes;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FileReader::ReadBytes(void *theBuf, unsigned long theNumBytes)
{
	if(mDataPos + theNumBytes > mDataLen)
		throw FileReaderException("Attempt to read past end of file.");
	
	if(mMemDataP!=NULL)
		memcpy(theBuf, mMemDataP + mDataPos, theNumBytes);
	else
		fread(theBuf,1,theNumBytes,mFile);

	mDataPos += theNumBytes;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int FileReader::TryReadByte()
{
	if(mDataPos >= mDataLen)
		return -1;
	
	if(mMemDataP!=NULL)
		return (unsigned char)mMemDataP[mDataPos++];
	else
	{	
		mDataPos++;
		return fgetc(mFile);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FileReader::SkipBytes(unsigned long theNumBytes)
{
	if(mDataPos + theNumBytes > mDataLen)
		throw FileReaderException("Attempt to read past end of file.");

	mDataPos += theNumBytes;
	if(mFile!=NULL)
		fseek(mFile,theNumBytes,SEEK_CUR);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FileReader::ReadIntoMemory()
{
	if(mMemDataP!=NULL) // already in memory
		return;

	if(mFile==NULL) // no file
		return;

	DWORD aNumBytes = length();

	if(aNumBytes>0)
	{
		int aPos = pos();
		SetPos(0);

		char *aBytes = new char[aNumBytes];	
		aNumBytes = ReadMaxBytes(aBytes,aNumBytes);
		Open(aBytes,aNumBytes);
		mOwnMemData = true;
		SetPos(aPos);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
unsigned char FileReader::ReadByte()
{
	unsigned char aByte;
	ReadBytes(&aByte,1);
	return aByte;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
unsigned short FileReader::ReadShort()
{
	unsigned short aShort;
	ReadBytes(&aShort,2);
	return ShortFromLittleEndian(aShort);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
unsigned long FileReader::ReadLong()
{
	unsigned long aLong;
	ReadBytes(&aLong,4);
	return LongFromLittleEndian(aLong);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int FileReader::ReadLength(int theLengthFieldSize)
{
	switch(theLengthFieldSize)
	{
		case 1: return ReadByte(); break;
		case 2: return ReadShort(); break;
		case 4: return ReadLong(); break; 
		default: return 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FileReader::ReadRawString(std::string &theStr, int theNumChars)
{
	theStr.erase();
	theStr.reserve(theNumChars);
	for(int i=0; i<theNumChars; i++)
		theStr += (char)ReadByte();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FileReader::ReadRawWString(std::wstring &theWStr, int theNumChars)
{
	theWStr.erase();
	theWStr.reserve(theNumChars);
	for(int i=0; i<theNumChars; i++)
		theWStr += (wchar_t)ReadShort();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FileReader::ReadString(std::string &theStr, unsigned char theLengthFieldSize)
{
	ReadRawString(theStr,ReadLength(theLengthFieldSize));
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FileReader::ReadWString(std::wstring &theWStr, unsigned char theLengthFieldSize)
{
	ReadRawWString(theWStr,ReadLength(theLengthFieldSize));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FileReader::Rewind()
{
	SetPos(0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FileReader::EndOfFile()
{
	return mDataPos >= mDataLen;
}	
