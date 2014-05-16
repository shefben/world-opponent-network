#ifndef __WON_URL_H__
#define __WON_URL_H__

#include <string>

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
enum URLType
{
	URLType_None,
	URLType_File,
	URLType_HTTP,
	URLType_FTP
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class URL
{
protected:
	URLType mType;
	std::string mAddress;
	std::string mPath;
	bool mIsAbsolute;

public:
	URL();
	URL(const std::string &theURL);

	void CalcRelativeTo(const URL &theBaseURL);

	URLType GetType() const { return mType; }
	const std::string& GetAddress() const { return mAddress; }
	const std::string& GetPath() const { return mPath; }
	std::string GetPathAsLocalFilePath() const;

	void GetEntire(std::string &theStr);
};

}; // namespace WONAPI

#endif