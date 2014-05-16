#include "ResourceHeader.h"

#include "WONCommon/FileReader.h"
#include "WONCommon/WONFile.h"

#include <stdio.h>
#include <assert.h>

using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ResourceHeader::ResourceHeader()
{
	RegisterKeyGroup("Resource",ResourceHeaderFactory);
	mHeaderFile = mSourceFile = NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceHeader::HandleKeyVal(ConfigParser &theParser, const std::string &theKey, StringParser &theVal)
{
	NameVector *aVector = NULL;
	if(theKey=="STRING" || theKey=="FONT" || theKey=="COLOR" || theKey=="IMAGE" || theKey=="SOUND" || theKey=="BACKGROUND" || theKey=="INT")
		return true;
	else if(theKey=="CODESTRING")
		aVector = &mStringNames;
	else if(theKey=="CODEIMAGE")
		aVector = &mImageNames;
	else if(theKey=="CODESOUND")
		aVector = &mSoundNames;
	else if(theKey=="CODEBACKGROUND")
		aVector = &mBackgroundNames;
	else if(theKey=="CODEINT")
		aVector = &mIntNames;
	else if(theKey=="CODECOLOR")
		aVector = &mColorNames;
	else if(theKey=="CODEFONT")
		aVector = &mFontNames;
	else if(theKey=="CONTROLID")
		aVector = &mControlIdNames;
	else
		return ConfigObject::HandleKeyVal(theParser,theKey,theVal);

	if(aVector!=NULL)
	{
		std::string aName;
		theVal.ReadString(aName,true);
		if(aName.empty())
			throw ConfigObjectException("Invalid Resource Key.\n");

		aVector->push_back(aName);
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceHeader::SetFileName(const std::string &theName)
{
	WONFile aFile(theName);
	mFileDir = aFile.GetFileDirectory();
	if(!mFileDir.empty())
		mFileDir += WONFile::PATH_SEP;

	mFileName = aFile.GetFileNameWithoutExtension();
	mHeaderFilePath = mFileDir + mFileName + ".h";
	mSourceFilePath = mFileDir + mFileName + ".cpp";
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceHeader::IsFileReadOnly()
{
	WONFile aFile(mSourceFilePath);
	if(aFile.IsReadOnly())
		return true;

	aFile = mHeaderFilePath;
	if(aFile.IsReadOnly())
		return true;

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void HeaderPrintf(FILE *theFile, const char *theStr, const ResourceHeader::NameVector &theNames)
{
	for(ResourceHeader::NameVector::const_iterator anItr = theNames.begin(); anItr!=theNames.end(); ++anItr)
		fprintf(theFile,theStr,anItr->c_str(),anItr->c_str());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void HeaderPrintf(FILE *theFile, const char *theStr, const char *param1, const ResourceHeader::NameVector &theNames)
{
	for(ResourceHeader::NameVector::const_iterator anItr = theNames.begin(); anItr!=theNames.end(); ++anItr)
		fprintf(theFile,theStr,param1,anItr->c_str(),anItr->c_str());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceHeader::HaveResources()
{
	return !(mStringNames.empty() && mImageNames.empty());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceHeader::WriteVariables()
{
	if(GetParent()==NULL)
	{
		Rewind();
		while(HasMoreChildren())
			((ResourceHeader*)NextChild())->WriteVariables();

		return;
	}

	if(!HaveResources())
		return;

	const char *aGroupName = GetName().c_str();
	HeaderPrintf(mHeaderFile,"extern WONAPI::GUIString %s_%s_String;\n",aGroupName,mStringNames);
	HeaderPrintf(mHeaderFile,"extern WONAPI::ImagePtr %s_%s_Image;\n",aGroupName,mImageNames);
	HeaderPrintf(mHeaderFile,"extern WONAPI::SoundPtr %s_%s_Sound;\n",aGroupName,mSoundNames);
	HeaderPrintf(mHeaderFile,"extern WONAPI::FontPtr %s_%s_Font;\n",aGroupName,mFontNames);
	HeaderPrintf(mHeaderFile,"extern WONAPI::Background %s_%s_Background;\n",aGroupName,mBackgroundNames);
	HeaderPrintf(mHeaderFile,"extern DWORD %s_%s_Color;\n",aGroupName,mColorNames);
	HeaderPrintf(mHeaderFile,"extern int %s_%s_Int;\n",aGroupName,mIntNames);

	HeaderPrintf(mSourceFile,"GUIString %s_%s_String;\n",aGroupName,mStringNames);
	HeaderPrintf(mSourceFile,"ImagePtr %s_%s_Image;\n",aGroupName,mImageNames);
	HeaderPrintf(mSourceFile,"SoundPtr %s_%s_Sound;\n",aGroupName,mSoundNames);
	HeaderPrintf(mSourceFile,"FontPtr %s_%s_Font;\n",aGroupName,mFontNames);
	HeaderPrintf(mSourceFile,"Background %s_%s_Background;\n",aGroupName,mBackgroundNames);
	HeaderPrintf(mSourceFile,"DWORD %s_%s_Color;\n",aGroupName,mColorNames);
	HeaderPrintf(mSourceFile,"int %s_%s_Int;\n",aGroupName,mIntNames);

	fprintf(mHeaderFile,"\n");
	fprintf(mSourceFile,"\n");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceHeader::WriteInitFunction()
{
	if(GetParent()==NULL)
	{
		fprintf(mHeaderFile,"WONAPI::ResourceConfigTablePtr %s_Init(WONAPI::ResourceConfigTable *theTable = NULL);\n",mFileName.c_str());
		fprintf(mSourceFile,"ResourceConfigTablePtr %s_Init(WONAPI::ResourceConfigTable *theTable)\n{\n",mFileName.c_str());
		fprintf(mSourceFile,"\tResourceConfigTablePtr aTable = theTable;\n");
		fprintf(mSourceFile,"\tif(theTable==NULL)\n\t\taTable = theTable = new ResourceConfigTable;\n\n");
		fprintf(mSourceFile,"\ttheTable->RegisterGlueResourceFunc(%s_SafeGlueResources);\n\n",mFileName.c_str());

		fprintf(mHeaderFile,"enum %sCtrlId\n{\n",mFileName.c_str());
		fprintf(mHeaderFile,"\t%sCtrlId_Min = WONAPI::ControlId_UserMin,\n",mFileName.c_str());

		Rewind();
		while(HasMoreChildren())
			((ResourceHeader*)NextChild())->WriteInitFunction();

		fprintf(mHeaderFile,"};\n");
		fprintf(mSourceFile,"\treturn aTable;\n");
		fprintf(mSourceFile,"}\n\n");
		return;
	}

	if(mControlIdNames.empty())
		return;

	HeaderPrintf(mHeaderFile,"\tID_%s,\n",mControlIdNames);
	HeaderPrintf(mSourceFile,"\ttheTable->RegisterControlId(\"%s\",ID_%s);\n",mControlIdNames);
	fprintf(mHeaderFile,"\n");
	fprintf(mSourceFile,"\n");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceHeader::WriteResourceIds()
{
	if(GetParent()==NULL)
	{
		fprintf(mHeaderFile,"enum %sId\n{\n",mFileName.c_str());
		fprintf(mSourceFile,"static void *gResources[] =\n{\n");

		Rewind();
		while(HasMoreChildren())
			((ResourceHeader*)NextChild())->WriteResourceIds();

		fprintf(mHeaderFile,"};\n");
		fprintf(mSourceFile,"\n\tNULL\n};\n");

		fprintf(mHeaderFile,"const WONAPI::GUIString& %s_GetString(%sId theId);\n",mFileName.c_str(),mFileName.c_str());
		fprintf(mSourceFile,"const GUIString& %s_GetString(%sId theId) { return *(GUIString*)gResources[theId]; }\n",mFileName.c_str(),mFileName.c_str());

		fprintf(mHeaderFile,"WONAPI::Image* %s_GetImage(%sId theId);\n",mFileName.c_str(),mFileName.c_str());
		fprintf(mSourceFile,"Image* %s_GetImage(%sId theId) { return *(ImagePtr*)gResources[theId]; }\n",mFileName.c_str(),mFileName.c_str());

		fprintf(mHeaderFile,"WONAPI::Sound* %s_GetSound(%sId theId);\n",mFileName.c_str(),mFileName.c_str());
		fprintf(mSourceFile,"Sound* %s_GetSound(%sId theId) { return *(SoundPtr*)gResources[theId]; }\n",mFileName.c_str(),mFileName.c_str());

		fprintf(mHeaderFile,"WONAPI::Font* %s_GetFont(%sId theId);\n",mFileName.c_str(),mFileName.c_str());
		fprintf(mSourceFile,"Font* %s_GetFont(%sId theId) { return *(FontPtr*)gResources[theId]; }\n",mFileName.c_str(),mFileName.c_str());

		fprintf(mHeaderFile,"const WONAPI::Background& %s_GetBackground(%sId theId);\n",mFileName.c_str(),mFileName.c_str());
		fprintf(mSourceFile,"const Background& %s_GetBackground(%sId theId) { return *(Background*)gResources[theId]; }\n",mFileName.c_str(),mFileName.c_str());

		fprintf(mHeaderFile,"DWORD %s_GetColor(%sId theId);\n",mFileName.c_str(),mFileName.c_str());
		fprintf(mSourceFile,"DWORD %s_GetColor(%sId theId) { return *(DWORD*)gResources[theId]; }\n",mFileName.c_str(),mFileName.c_str());

		fprintf(mHeaderFile,"int %s_GetInt(%sId theId);\n",mFileName.c_str(),mFileName.c_str());
		fprintf(mSourceFile,"int %s_GetInt(%sId theId) { return *(int*)gResources[theId]; }\n",mFileName.c_str(),mFileName.c_str());


		return;
	}


	if(!HaveResources())
		return;

	const char *aGroupName = GetName().c_str();

	HeaderPrintf(mHeaderFile,"\t%s_%s_String_Id,\n",aGroupName,mStringNames);
	HeaderPrintf(mSourceFile,"\t&%s_%s_String,\n",aGroupName,mStringNames);

	HeaderPrintf(mHeaderFile,"\t%s_%s_Image_Id,\n",aGroupName,mImageNames);
	HeaderPrintf(mSourceFile,"\t&%s_%s_Image,\n",aGroupName,mImageNames);

	HeaderPrintf(mHeaderFile,"\t%s_%s_Sound_Id,\n",aGroupName,mSoundNames);
	HeaderPrintf(mSourceFile,"\t&%s_%s_Sound,\n",aGroupName,mSoundNames);

	HeaderPrintf(mHeaderFile,"\t%s_%s_Font_Id,\n",aGroupName,mFontNames);
	HeaderPrintf(mSourceFile,"\t&%s_%s_Font,\n",aGroupName,mFontNames);

	HeaderPrintf(mHeaderFile,"\t%s_%s_Background_Id,\n",aGroupName,mBackgroundNames);
	HeaderPrintf(mSourceFile,"\t&%s_%s_Background,\n",aGroupName,mBackgroundNames);

	HeaderPrintf(mHeaderFile,"\t%s_%s_Int_Id,\n",aGroupName,mIntNames);
	HeaderPrintf(mSourceFile,"\t&%s_%s_Int,\n",aGroupName,mIntNames);

	HeaderPrintf(mHeaderFile,"\t%s_%s_Color_Id,\n",aGroupName,mColorNames);
	HeaderPrintf(mSourceFile,"\t&%s_%s_Color,\n",aGroupName,mColorNames);

	fprintf(mHeaderFile,"\n");
	fprintf(mSourceFile,"\n");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceHeader::WriteGlueFunction()
{
	if(GetParent()==NULL)
	{
		fprintf(mHeaderFile,"void %s_SafeGlueResources(WONAPI::ResourceConfigTable *theTable);\n",mFileName.c_str());
		fprintf(mSourceFile,"void %s_SafeGlueResources(ResourceConfigTable *theTable)\n{\n",mFileName.c_str());

		fprintf(mSourceFile,"\tResourceConfig *aConfig;\n");
		Rewind();
		while(HasMoreChildren())
			((ResourceHeader*)NextChild())->WriteGlueFunction();

		fprintf(mSourceFile,"}\n\n");
		return;
	}

	if(!HaveResources() && mControlIdNames.empty())
		return;

	const char *aGroupName = GetName().c_str();

	fprintf(mSourceFile,"\taConfig=theTable->SafeGetConfig(\"%s\");\n",aGroupName);
	
	HeaderPrintf(mSourceFile,"\t%s_%s_String = aConfig->SafeGetString(\"%s\");\n",aGroupName,mStringNames);
	HeaderPrintf(mSourceFile,"\t%s_%s_Image = aConfig->SafeGetImage(\"%s\");\n",aGroupName,mImageNames);
	HeaderPrintf(mSourceFile,"\t%s_%s_Sound = aConfig->SafeGetSound(\"%s\");\n",aGroupName,mSoundNames);
	HeaderPrintf(mSourceFile,"\t%s_%s_Font = aConfig->SafeGetFont(\"%s\");\n",aGroupName,mFontNames);
	HeaderPrintf(mSourceFile,"\t%s_%s_Background = aConfig->SafeGetBackground(\"%s\");\n",aGroupName,mBackgroundNames);
	HeaderPrintf(mSourceFile,"\t%s_%s_Color = aConfig->SafeGetColor(\"%s\");\n",aGroupName,mColorNames);
	HeaderPrintf(mSourceFile,"\t%s_%s_Int = aConfig->SafeGetInt(\"%s\");\n",aGroupName,mIntNames);

	fprintf(mSourceFile,"\n");

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceHeader::WriteUnloadFunction()
{
	if(GetParent()==NULL)
	{
		fprintf(mHeaderFile,"void %s_UnloadResources();\n",mFileName.c_str());
		fprintf(mSourceFile,"void %s_UnloadResources()\n{\n",mFileName.c_str());

		Rewind();
		while(HasMoreChildren())
			((ResourceHeader*)NextChild())->WriteUnloadFunction();
	
		fprintf(mSourceFile,"}\n\n");

		return;
	}

	if(!HaveResources())
		return;


	const char *aGroupName = GetName().c_str();
	
	HeaderPrintf(mSourceFile,"\t%s_%s_String.erase();\n",aGroupName,mStringNames);
	HeaderPrintf(mSourceFile,"\t%s_%s_Image = NULL;\n",aGroupName,mImageNames);
	HeaderPrintf(mSourceFile,"\t%s_%s_Sound = NULL;\n",aGroupName,mSoundNames);
	HeaderPrintf(mSourceFile,"\t%s_%s_Font = NULL;\n",aGroupName,mFontNames);
	HeaderPrintf(mSourceFile,"\t%s_%s_Background = Background();\n",aGroupName,mBackgroundNames);
//	HeaderPrintf(mSourceFile,"\t%s_%s_Color = NULL;\n",aGroupName,mColorNames);
//	HeaderPrintf(mSourceFile,"\t%s_%s_Int = NULL;\n",aGroupName,mIntNames);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceHeader::WriteSourceFiles()
{
	WONFile aFileCheck(mHeaderFilePath);
	aFileCheck.Remove(true);

	aFileCheck = mFileDir + mSourceFilePath;
	aFileCheck.Remove(true);

	mHeaderFile = fopen(mHeaderFilePath.c_str(),"w");
	mSourceFile = fopen(mSourceFilePath.c_str(),"w");
	AutoFile aCloseHeader(mHeaderFile);
	AutoFile aCloseSource(mSourceFile);

	if(mHeaderFile==NULL || mSourceFile==NULL)
	{
		mHeaderFile = mSourceFile = NULL;
		return false;
	}
	
	fprintf(mHeaderFile,"#ifndef __%s_H__\n",mFileName.c_str());
	fprintf(mHeaderFile,"#define __%s_H__\n",mFileName.c_str());
	fprintf(mHeaderFile,"#include \"WONGUI/WONGUIConfig/ResourceConfig.h\"\n");
	fprintf(mHeaderFile,"#include \"WONGUI/StringLocalize.h\"\n");
	fprintf(mHeaderFile,"#include \"WONGUI/EventTypes.h\"\n\n");

	fprintf(mSourceFile,"#include \"WONGUI/WONGUIConfig/ComponentConfig.h\"\n");
	fprintf(mSourceFile,"#include \"%s\"\n\nusing namespace WONAPI;\n\n",(mFileName + ".h").c_str());

	Rewind();
	while(HasMoreChildren())
	{
		ResourceHeader *aRes = (ResourceHeader*)NextChild();
		aRes->mHeaderFile = mHeaderFile;
		aRes->mSourceFile = mSourceFile;
		aRes->mFileName = mFileName;
	}


	WriteVariables();
	WriteInitFunction();
	WriteResourceIds();
	WriteGlueFunction();
	WriteUnloadFunction();

	fprintf(mHeaderFile,"\n#endif\n");

	mHeaderFile = mSourceFile = NULL;
	return true;
}


