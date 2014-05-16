#ifndef __WON_FILEREADER_H__
#define __WON_FILEREADER_H__
#include "WONShared.h"

#include <stdio.h>
#include <exception>
#include <string>

namespace WONAPI
{

class WONBigFile;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class FileReader
{
private:
	FILE *mFile;
	const char *mMemDataP;
	bool mOwnMemData;
	
	unsigned long mDataLen;
	unsigned long mDataPos;
	unsigned long mDataStartPos;

	void CalcFileLength();
	void Init();

public:
	static bool AddBigFile(WONBigFile *theFile, const char *theBasePath = NULL);
	static void RemoveBigFile(WONBigFile *theFile);

public:
	FileReader();
	FileReader(const char *theFilePath);
	FileReader(const void *theMemDataP, unsigned long theMemDataLen);

	~FileReader();

	bool Open(const char *theFilePath);
	bool Open(const char *theFilePath, unsigned long theStartPos, unsigned long theLength);
	void Open(const void *theMemDataP, unsigned long theMemDataLen);
	void Close();

	int ReadMaxBytes(void *theBuf, int theMaxBytes);
	void ReadBytes(void *theBuf, unsigned long theNumBytes);
	unsigned char ReadByte();
	unsigned short ReadShort();
	unsigned long ReadLong();
	int TryReadByte(); // tries to read a byte but returns -1 if at end of file (just like fgetc.)

	int ReadLength(int theLengthFieldSize);
	void ReadRawString(std::string &theStr, int theNumChars);
	void ReadRawWString(std::wstring &theWStr, int theNumChars);
	void ReadString(std::string &theStr, unsigned char theLengthFieldSize = 2);
	void ReadWString(std::wstring &theWStr, unsigned char theLengthFieldSize = 2);

	unsigned long pos();
	unsigned long length();
	unsigned long Available() { return length() - pos(); }

	void SkipBytes(unsigned long theNumBytes);
	void SetPos(unsigned long thePos);
	void Rewind();
	void ReadIntoMemory();
	bool EndOfFile();

	const char* GetMemData() { return mMemDataP; }
	void SetOwnMemData(bool own) { mOwnMemData = own; }

	bool IsOpen() { return mFile!=NULL || mMemDataP!=NULL; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class FileReaderException : public std::exception
{
public:
	std::string what;
	FileReaderException() : exception() { }
	FileReaderException(const std::string &theWhat) { what = theWhat; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct AutoFile 
{
	FILE *mFile;
	AutoFile(FILE *theFile) : mFile(theFile) {}
	~AutoFile() { fclose(mFile); }
};

}; // namespace WONAPI

#endif