#ifndef __WON_FILEWRITER_H__
#define __WON_FILEWRITER_H__
#include "WONShared.h"

#include <stdio.h>
#include <exception>
#include <string>

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class FileWriter
{
private:
	FILE *mFile;

public:
	FileWriter();
	~FileWriter();

	bool Open(const char *theFilePath, bool openIfReadOnly = false);
	void Close();

	void WriteBytes(const void *theBuf, unsigned long theNumBytes);
	void WriteByte(unsigned char theByte);
	void WriteShort(unsigned short theShort);
	void WriteLong(unsigned long theLong);

	void WriteLength(unsigned long theLen, unsigned char theLengthFieldSize);
	void WriteRawString(const std::string &theStr);
	void WriteRawWString(const std::wstring &theWStr);
	void WriteString(const std::string &theStr, unsigned char theLengthFieldSize = 2);
	void WriteWString(const std::wstring &theWStr, unsigned char theLengthFieldSize = 2);

	void SetPos(unsigned long thePos);

	unsigned long pos();
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class FileWriterException : public std::exception
{
public:
	std::string what;
	FileWriterException() : exception() { }
	FileWriterException(const std::string &theWhat) { what = theWhat; }
};


}; // namespace WONAPI

#endif