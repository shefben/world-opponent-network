#ifndef __WON_RESOURCEHEADER_H__
#define __WON_RESOURCEHEADER_H__

#include "WONCommon/StringUtil.h"
#include "WONConfig/ConfigObject.h"

#include <set>
#include <vector>

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ResourceHeader : public ConfigObject
{
public:
	typedef std::set<std::string,StringLessNoCase> NameSet;
	typedef std::vector<std::string> NameVector;

protected:
	NameVector mStringNames;
	NameVector mImageNames;
	NameVector mSoundNames;
	NameVector mBackgroundNames;
	NameVector mIntNames;
	NameVector mColorNames;
	NameVector mFontNames;
	NameVector mControlIdNames;

	std::string mFileDir;
	std::string mFileName;
	std::string mHeaderFilePath;
	std::string mSourceFilePath;

	FILE *mHeaderFile;
	FILE *mSourceFile;

	virtual bool HandleKeyVal(ConfigParser &theParser, const std::string &theKey, StringParser &theVal);

	static ConfigObject* ResourceHeaderFactory() { return new ResourceHeader; }

	bool HaveResources();
	void WriteVariables();
	void WriteInitFunction();
	void WriteResourceIds();
	void WriteGlueFunction();
	void WriteUnloadFunction();

public:
	ResourceHeader();

	void SetFileName(const std::string &theName);
	bool IsFileReadOnly();
	bool WriteSourceFiles();

};
typedef SmartPtr<ResourceHeader> ResourceHeaderPtr;

}; //

#endif