#ifndef __WON_WONFILE_H__
#define __WON_WONFILE_H__
#include "WONShared.h"

#include <string>
#include <time.h>
namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WONFile
{
protected:
	std::string mFilePath;
	std::string mFileDirectory;
	std::string mFileName;
	std::string mFileNameWithoutExtension;
	std::string mFileExtension;
	std::string mFullPath;

	int mSize;
	time_t mModifyTime;
	time_t mCreateTime;
	bool mExists;
	bool mReadOnly;
	bool mIsDir;

	bool mGotFileDirectory;
	bool mGotFileName;
	bool mGotFileExtension;
	bool mGotFileInfo;
	bool mGotFullPath;

	void CalcFullPath();
	void CalcFileDirectory();
	void CalcFileExtension();
	void CalcFileInfo();

public:
	static const char PATH_SEP;

public:
	WONFile();
	WONFile(const std::string &thePath);
	void Reset();

	void SetFilePath(const std::string &thePath);
	void ClearCachedFileInfo();

	const std::string& GetFilePath() const;
	const std::string& GetFileDirectory() const;
	const std::string& GetFileName() const;
	const std::string& GetFileNameWithoutExtension() const;
	const std::string& GetFileExtension() const;
	const std::string& GetFullPath() const;
	std::string GetRelativePath(const WONFile &theSubFile);
	
	int GetSize() const;
	time_t GetModifyTime() const;
	time_t GetCreateTime() const;
	bool Exists() const;
	bool IsReadOnly() const;
	bool IsDir() const;


	bool Remove(bool evenIfReadOnly = true);
	bool SetReadOnly(bool readOnly);
	static WONFile GetWorkingDirectory();
	bool SetWorkingDirectory();

	bool CreateAsDirectory(bool createAllNecessaryDirectories = true);
	bool NormalizePath();
};


}; // namespace WONAPI

#endif
