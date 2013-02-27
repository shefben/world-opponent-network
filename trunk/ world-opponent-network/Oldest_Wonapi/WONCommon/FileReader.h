#ifndef __WON_FILEREADER_H__
#define __WON_FILEREADER_H__
#include "WONShared.h"

#include <stdio.h>
#include <exception>
#include <string>

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class FileReader
{
private:
	FILE *mFile;
	const char *mMemDataP;
	unsigned long mMemDataLen;
	unsigned long mMemDataPos;

public:
	FileReader();
	~FileReader();

	bool Open(const char *theFilePath);
	void Open(const void *theMemDataP, unsigned long theMemDataLen);
	void Close();

	void ReadBytes(void *theBuf, unsigned long theNumBytes);
	unsigned char ReadByte();
	unsigned short ReadShort();
	unsigned long ReadLong();

	void SkipBytes(unsigned long theNumBytes);
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
