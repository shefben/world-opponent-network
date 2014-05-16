#ifndef __WON_WONBIGFILE_H__
#define __WON_WONBIGFILE_H__
#include "WONShared.h"
#include "WONCommon/SmartPtr.h"
#include "WONCommon/StringUtil.h"

#include <map>

namespace WONAPI
{

class FileReader;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONBigFile : public RefCount
{
public:
	struct FilePiece
	{
		unsigned long mStartPos;
		unsigned long mLength;

		FilePiece() : mStartPos(0), mLength(0) { }
		FilePiece(unsigned long theStartPos, unsigned long theLength) : mStartPos(theStartPos), mLength(theLength) { }
	};

	typedef std::map<std::string,FilePiece,StringLessNoCase> FileMap;

protected:
	virtual ~WONBigFile();

	char *mMemDataP;
	bool mOwnMemData;
	std::string mFilePath;
	FileMap mFileMap;

	void Init();

	bool Read(FileReader &theReader);

public:
	WONBigFile(const char *theFilePath);
	WONBigFile(const void *theData, unsigned long theLength, bool ownData = false);

#ifdef WIN32_NOT_XBOX
	WONBigFile(const char *theResName, const char *theResType);
#endif

	const char* GetMemData() { return mMemDataP; }
	const std::string& GetFilePath() { return mFilePath; }
	const FileMap& GetFileMap() { return mFileMap; }
};
typedef SmartPtr<WONBigFile> WONBigFilePtr;


} // namespace WONAPI

#endif