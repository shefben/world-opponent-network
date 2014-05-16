#include "WONFile.h"
#include <sys/stat.h>
#include <vector>

#ifdef WIN32
#include <direct.h>
#include <io.h>
#endif

using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
const char WONFile::PATH_SEP = '\\';
#else
const char WONFile::PATH_SEP = '/';
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONFile::WONFile()
{
	SetFilePath("");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONFile::WONFile(const std::string &thePath)
{
	SetFilePath(thePath);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONFile::Reset()
{
	mFilePath.erase();
	mFileDirectory.erase();
	mFileName.erase();
	mFileExtension.erase();
	mFullPath.erase();

	mGotFileDirectory = false;
	mGotFileName = false;
	mGotFileExtension = false;
	mGotFileInfo = false;
	mGotFullPath = false;
	mIsDir = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONFile::SetFilePath(const std::string &thePath)
{
	Reset();
	mFilePath = thePath;
	int i=0;
//	if(thePath.length()>2 && thePath[0]=='\\' && thePath[1]=='\\')
//		i = 2; // skip network path stuff

	for(; i<mFilePath.length(); i++)
	{
		static const char INCORRECT_PATH_SEP = '/';
		if(mFilePath[i]==INCORRECT_PATH_SEP)
			mFilePath[i] = PATH_SEP;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONFile::NormalizePath()
{
	if(mFilePath.empty())
		return false;

	std::string aFilePath;
	bool firstIsSlash = false;
	if(mFilePath[0]!=PATH_SEP)
		aFilePath = PATH_SEP + mFilePath;
	else
	{
		firstIsSlash = true;
		aFilePath = mFilePath;
	}

	typedef std::vector<int> DirPosVec;
	DirPosVec aVec;
	
	int aPos = 0;
	while(aPos < aFilePath.length())
	{
		if(aFilePath[aPos]==PATH_SEP)
		{
			bool hadReplacement = false;
			if(aPos+2<aFilePath.length() && aFilePath[aPos+1]=='.' && aFilePath[aPos+2]=='.')
			{
				if(aPos+3>=aFilePath.length() || aFilePath[aPos+3]==PATH_SEP)
				{
					hadReplacement = true;

					// -----> /../
					if(!aVec.empty())
					{
						int aPrevDirPos = aVec.back();
						aFilePath = aFilePath.substr(0,aPrevDirPos) + aFilePath.substr(aPos+3);
						aPos = aPrevDirPos; 
						aVec.pop_back();
					}
					else
						aPos++;
				}
			}
			else if(aPos+1<aFilePath.length() && aFilePath[aPos+1]=='.')
			{
				// ----> /./
				if(aPos+2>=aFilePath.length() || aFilePath[aPos+2]==PATH_SEP)  
				{
					hadReplacement = true;
					aFilePath = aFilePath.substr(0,aPos) + aFilePath.substr(aPos+2);
				}
			}

			if(!hadReplacement)
			{
				aVec.push_back(aPos);
				aPos++;
			}
		}
		else						
			aPos++;
	}

	if(!firstIsSlash)
		mFilePath = aFilePath.substr(1);
	else
		mFilePath = aFilePath;

	mFileDirectory.erase();
	mGotFileDirectory = false;
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONFile::Remove(bool evenIfReadOnly)
{
	if(evenIfReadOnly)
	{
		if(!mGotFileInfo)
			CalcFileInfo();

		if(mReadOnly)
			SetReadOnly(false);
	}

	return remove(mFilePath.c_str())==0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONFile::ClearCachedFileInfo()
{
	mGotFileInfo = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONFile::SetReadOnly(bool readOnly)
{
	return chmod(mFilePath.c_str(),readOnly?S_IREAD:S_IREAD|S_IWRITE)==0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONFile WONFile::GetWorkingDirectory()
{
#ifdef _XBOX
	return WONFile();
#else
	char aLocalPath[MAX_PATH];
	_getcwd(aLocalPath, sizeof(aLocalPath));
	return WONFile(aLocalPath);
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONFile::SetWorkingDirectory()
{
#ifdef _XBOX
	return false;
#else
	GetFileDirectory();
	if(mFileDirectory.empty())
		return false;
	else
		return chdir(mFileDirectory.c_str())==0;
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
std::string WONFile::GetRelativePath(const WONFile &theSubFile)
{
	const std::string &aPath1 = GetFileDirectory();
	const std::string &aPath2 = theSubFile.GetFilePath();
	if(aPath2.length() > aPath1.length())
	{
		int i;
		for(i=0; i<aPath1.length(); i++)
		{
			unsigned char char1 = aPath1[i];
			unsigned char char2 = aPath2[i];

#ifdef WIN32
			char1 = toupper(char1);
			char2 = toupper(char2);
#endif
			if(char1!=char2)
				break;
		}

		if(i==aPath1.length() && aPath2[i]==PATH_SEP) // paths match
			return aPath2.substr(i+1);
	}

	// no match --> return absolute path
	return aPath2;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const std::string& WONFile::GetFilePath() const
{
	return mFilePath;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONFile::CalcFileInfo()
{
	mGotFileInfo = true;
	struct stat fileInfo;
	if(stat(mFilePath.c_str(), &fileInfo)==0)
	{
		mExists = true;
		int aMode = 0;
#ifdef WIN32
		aMode = 2;
#else
		aMode = 0200;
#endif

		mReadOnly = (fileInfo.st_mode & aMode)?false:true;
		mSize = fileInfo.st_size;
		mModifyTime = fileInfo.st_mtime;
		mCreateTime = fileInfo.st_ctime;
		mIsDir = (fileInfo.st_mode & _S_IFDIR)?true:false;

#if defined(macintosh) && (macintosh == 1)
	mModifyTime += 126230400; // # of seconds Mac time_t differs in stat and utime
#endif // mac
	}
	else
	{
		mSize = 0;
		mModifyTime = 0;
		mCreateTime = 0;
		mExists = false;
		mReadOnly = false;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONFile::CalcFileDirectory()
{
	int aSlashPos = mFilePath.find_last_of(PATH_SEP);
	if(aSlashPos!=string::npos)
		mFileDirectory = mFilePath.substr(0,aSlashPos);

	mFileName = mFilePath.substr(aSlashPos+1);
	mGotFileDirectory = true;
	mGotFileName = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONFile::CalcFullPath()
{
#ifdef WIN32_NOT_XBOX
	char aPath[_MAX_PATH];
	if(_fullpath(aPath,mFilePath.c_str(),_MAX_PATH)!=NULL)
		mFullPath = aPath;
#else
	mFullPath = mFilePath;
#endif

	mGotFullPath = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void WONFile::CalcFileExtension()
{
	GetFileName();

	int aDotPos = mFileName.find_last_of('.');
	if(aDotPos!=string::npos)
	{
		mFileExtension = mFileName.substr(aDotPos+1);
		mFileNameWithoutExtension = mFileName.substr(0,aDotPos);
	}
	else
		mFileNameWithoutExtension  = mFileName;

	mGotFileExtension = true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const std::string& WONFile::GetFileDirectory() const
{
	if(mGotFileDirectory)
		return mFileDirectory;

	(const_cast<WONFile*>(this))->CalcFileDirectory();
	return mFileDirectory;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const std::string& WONFile::GetFileName() const
{
	if(!mGotFileName)
		GetFileDirectory();

	return mFileName;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const std::string& WONFile::GetFileExtension() const
{
	if(!mGotFileExtension)
		(const_cast<WONFile*>(this))->CalcFileExtension();

	return mFileExtension;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const std::string& WONFile::GetFileNameWithoutExtension() const
{
	GetFileExtension();
	return mFileNameWithoutExtension;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const std::string& WONFile::GetFullPath() const
{
	if(!mGotFullPath)
		(const_cast<WONFile*>(this))->CalcFullPath();

	return mFullPath;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int WONFile::GetSize() const
{
	if(!mGotFileInfo)
		(const_cast<WONFile*>(this))->CalcFileInfo();

	return mSize;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
time_t WONFile::GetModifyTime() const
{
	if(!mGotFileInfo)
		(const_cast<WONFile*>(this))->CalcFileInfo();

	return mModifyTime;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
time_t WONFile::GetCreateTime() const
{
	if(!mGotFileInfo)
		(const_cast<WONFile*>(this))->CalcFileInfo();

	return mCreateTime;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONFile::Exists() const
{
	if(!mGotFileInfo)
		(const_cast<WONFile*>(this))->CalcFileInfo();

	return mExists;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONFile::IsReadOnly() const
{
	if(!mGotFileInfo)
		(const_cast<WONFile*>(this))->CalcFileInfo();

	return mReadOnly;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONFile::IsDir() const
{
	if(!mGotFileInfo)
		(const_cast<WONFile*>(this))->CalcFileInfo();

	return mIsDir;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool WONFile::CreateAsDirectory(bool createAllNecessaryDirectories)
{
	if(Exists())
		return IsDir();

	return CreateDirectory(mFilePath.c_str(),NULL)!=0;
}
