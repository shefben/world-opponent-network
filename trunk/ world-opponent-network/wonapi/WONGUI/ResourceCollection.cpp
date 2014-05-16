#include "ResourceCollection.h"
#include "WONCommon/WONFile.h"
#include "WONCommon/WriteBuffer.h"
#include "WONCommon/StringParser.h"

using namespace WONAPI;
using namespace std;

static const int RESOURCE_VERSION = 1;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ResourceCollection::ResourceCollection()
{
//	mTrackByName = false;
	mTrackByName = true;
	mDoOutStringFilter = false;
	mFileVersion = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ResourceCollection::~ResourceCollection()
{
	Close();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceCollection::Open(const void *theData, unsigned long theDataLen)
{
	Close();
	mFileName = "";
	mFileReader.Open(theData,theDataLen);
	return ReadTableOfContents();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceCollection::Open(const GUIString &theFileName)
{
	Close();
	mFileName = theFileName;
	string aName = theFileName;
	if(!mFileReader.Open(aName.c_str()))
		return false;
	
	return ReadTableOfContents();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceCollection::Close()
{	
	mFileReader.Close();
	mResourceIdMap.clear();
	mResourceNameMap.clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceCollection::CloseFile()
{
	mFileReader.Close();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ResourceCollection::GetLatestVersion()
{
	return RESOURCE_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceCollection::DecodeAll(bool closeFile)
{
	for(ResourceIdMap::iterator anItr=mResourceIdMap.begin(); anItr!=mResourceIdMap.end(); ++anItr)
		DecodeResource(anItr->second);

	if(closeFile)
		CloseFile();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString ResourceCollection::ReadWideString()
{
	unsigned short aLength = mFileReader.ReadShort();
	GUIString aStr(aLength);
	for(int i=0; i<aLength; i++)
		aStr.append(mFileReader.ReadShort());

	return aStr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString ResourceCollection::ReadAsciiString()
{
	unsigned short aLength = mFileReader.ReadShort();
	GUIString aStr(aLength);
	for(int i=0; i<aLength; i++)
		aStr.append(mFileReader.ReadByte());

	return aStr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceCollection::AddEntry(ResourceEntry *theEntry)
{
	mResourceIdMap[theEntry->mId] = theEntry;

	if(mTrackByName)
		mResourceNameMap[theEntry->mName] = theEntry;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceCollection::RemoveEntry(const ResourceEntry *theEntry, bool doRemove)
{
	ResourceIdMap::iterator anItr = mResourceIdMap.find(theEntry->mId);
	if(anItr!=mResourceIdMap.end() && anItr->second.get()==theEntry)
	{
		if(doRemove)
		{
			if(mTrackByName)
				mResourceNameMap.erase(theEntry->mName);

			mResourceIdMap.erase(anItr);
		}
		else
		{
			ResourceEntry *anEntry = const_cast<ResourceEntry*>(theEntry);
			anEntry->mUpdateType = ResourceUpdateType_Remove;
		}
		
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceCollection::ReadTableOfContents()
{
	try
	{
		if(mFileReader.Available()>=7)
		{
			char aSig[4];
			aSig[3] = '\0';
			mFileReader.ReadBytes(aSig,3);
			if(strcmp(aSig,"WRS")==0)
				mFileVersion = mFileReader.ReadLong();
			else
				mFileReader.SetPos(0); // version 0
		}

		int aTableOffset = mFileReader.ReadLong();
		mFileReader.SetPos(aTableOffset);
		int aNumEntries = mFileReader.ReadLong();
		int *prevSize = NULL;
		int prevOffset = 0;
		for(int i=0; i<aNumEntries; i++)
		{
			ResourceEntryPtr anEntry = new ResourceEntry;
			anEntry->mId = mFileReader.ReadLong();
			anEntry->mType = (ResourceType)mFileReader.ReadShort();
			anEntry->mFileOffset = mFileReader.ReadLong();
			anEntry->mName = ReadAsciiString();
			anEntry->mDesc = ReadWideString();

			if(prevSize!=NULL)
				*prevSize = anEntry->mFileOffset - prevOffset;

			prevSize = &anEntry->mFileSize;
			prevOffset = anEntry->mFileOffset;

			AddEntry(anEntry);
		}
		if(prevSize!=NULL)
			*prevSize = aTableOffset - prevOffset;
	}
	catch(FileReaderException&)
	{
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceCollection::HaveResource(const ResourceEntry *theEntry)
{
	if(theEntry==NULL)
		return false;

	return GetResource(theEntry->mId,false)==theEntry;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FontPtr ResourceCollection::DecodeFont(const GUIString &theDesc)
{
	string aDesc = theDesc;
	StringParser aParser(aDesc.c_str());

	string aFace;
	aParser.ReadString(aFace,false);
	if(!aParser.CheckNextChar(','))
		return WindowManager::GetDefaultWindowManager()->GetDefaultFont();

	int aStyle = 0;
	while(true)
	{
		string aStyleStr;
		aParser.ReadString(aStyleStr,true);
		if(aStyleStr.empty())
			break;

		if(stricmp(aStyleStr.c_str(),"PLAIN")==0)
			aStyle |= FontStyle_Plain;
		else if(stricmp(aStyleStr.c_str(),"BOLD")==0)
			aStyle |= FontStyle_Bold;
		else if(stricmp(aStyleStr.c_str(),"ITALIC")==0)
			aStyle |= FontStyle_Italic;
		else if(stricmp(aStyleStr.c_str(),"UNDERLINE")==0)
			aStyle |= FontStyle_Underline;
	}

	if(!aParser.CheckNextChar(','))
		return WindowManager::GetDefaultWindowManager()->GetDefaultFont();

	int aSize = 0;
	aParser.ReadValue(aSize);

	if(aFace.empty() || aSize<=0)
		return WindowManager::GetDefaultWindowManager()->GetDefaultFont();

	return Window::GetDefaultWindow()->GetFont(FontDescriptor(aFace,aStyle,aSize));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static int ResourceCollectionGetHexDigit(char theChar)
{
	if(theChar>='0' && theChar<='9')
		return theChar - '0';
	
	theChar = toupper(theChar);
	if(theChar>='A' && theChar<='F')
		return theChar - 'A' + 10;

	return -1;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
DWORD ResourceCollection::DecodeColor(const GUIString &theDesc)
{
	const GUIString &aStr = theDesc;

	DWORD aColor = 0;
	int i;
	for(i=0; i<6; i++)
	{
		aColor<<=4;
		int aDigit = ResourceCollectionGetHexDigit(aStr.at(i));
		if(aDigit<0)
			return -1;

		aColor |= aDigit;
	
		if(aStr.at(i+1)=='\0')
			break;
	}

	if(i<6)
		aColor<<=(4*(5-i));

	return aColor;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RefCount* ResourceCollection::DecodeResource(const ResourceEntry *theEntry, ResourceType theExpectedType, bool altDecode)
{
	if(theExpectedType!=ResourceType_None && theEntry->mType!=theExpectedType)
		return NULL;

	if(altDecode)
	{
		if(theEntry->mAltData.get()!=NULL)
			return theEntry->mAltData;
	}
	else
	{
		if(theEntry->mData.get()!=NULL)
			return theEntry->mData;
	}

	ResourceEntry *anEntry = const_cast<ResourceEntry*>(theEntry);
	FileReader *aFileReader = &mFileReader;
	FileReader aSeperateFile;
	string aFilePath;
	if(anEntry->mFileSize > 0)
	{
		aFilePath = anEntry->mDesc;
		if(anEntry->NeedUpdate())
		{
			if(!aSeperateFile.Open(aFilePath.c_str()))
				return NULL;

			aFileReader = &aSeperateFile;
		}
		else
			mFileReader.SetPos(anEntry->mFileOffset);
	}

	switch(theEntry->mType)
	{
		case ResourceType_Image: 
		{
			if(mWindow.get()==NULL)
			{
				mWindow = Window::GetDefaultWindow();
				if(mWindow.get()==NULL)
					return NULL;
			}

			if(mWindow->GetDisplayContext()==NULL)
				return NULL;

			MultiImageDecoder *aDecoder = mWindow->GetWindowManager()->GetImageDecoder();
			aDecoder->SetFileName(aFilePath.c_str());
			if(altDecode)
				anEntry->mAltData = aDecoder->ImageDecoder::DecodeRaw(*aFileReader);
			else
				anEntry->mData = aDecoder->ImageDecoder::Decode(mWindow->GetDisplayContext(),*aFileReader);
		}
		break;

		case ResourceType_String: break;
		case ResourceType_Color: break;
		case ResourceType_Font: anEntry->mData = DecodeFont(anEntry->mDesc); break;
	
		case ResourceType_Binary: 
		{
			try
			{
				WriteBuffer aBuf;
				aBuf.Reserve(anEntry->mFileSize);
				aFileReader->ReadBytes(aBuf.data(),aBuf.capacity());
				aBuf.SkipBytes(aBuf.capacity());
				ByteBufferPtr aByteBuffer = aBuf.ToByteBuffer();
				anEntry->mData = (ByteBuffer*)aByteBuffer.get();
			}
			catch(FileReaderException&)
			{
			}
		}
		break;
	}

	if(altDecode)
		return anEntry->mAltData;
	else
		return anEntry->mData;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const ResourceEntry* ResourceCollection::GetResource(int theId, bool decode, ResourceType theExpectedType)
{
	ResourceIdMap::iterator anItr = mResourceIdMap.find(theId);
	if(anItr==mResourceIdMap.end())
		return NULL;
		
	if(decode)
		DecodeResource(anItr->second, theExpectedType);
	
	return anItr->second;	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const ResourceEntry* ResourceCollection::GetResource(const GUIString &theName, bool decode, ResourceType theExpectedType)
{
	ResourceNameMap::iterator anItr = mResourceNameMap.find(theName);
	if(anItr==mResourceNameMap.end())
		return NULL;

	if(decode)
		DecodeResource(anItr->second);

	return anItr->second;	
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RawImagePtr ResourceCollection::GetRawImage(int theId)
{
	const ResourceEntry *anEntry = (ResourceEntry*)GetResource(theId,false);
	RawImage *anImage = (RawImage*)DecodeResource(anEntry,ResourceType_Image,true);
	return anImage;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
NativeImagePtr ResourceCollection::GetImage(int theId)
{
	const ResourceEntry *anEntry = GetResource(theId,true,ResourceType_Image);
	if(anEntry==NULL)
		return NULL;
	else
		return (NativeImage*)anEntry->mData.get();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString ResourceCollection::OutStringFilter(const GUIString &theStr)
{
	GUIString aNewStr(theStr.length());
	for(int i=0; i<theStr.length(); i++)
	{
		int aChar = theStr.at(i);
		switch(aChar)
		{
			case '\n': aNewStr.append('\\'); aNewStr.append('n'); break;
			case '\t': aNewStr.append('\\'); aNewStr.append('t'); break;
			case '\\': aNewStr.append('\\'); aNewStr.append('\\'); break;
			default:
				aNewStr.append(aChar);
		}
	}

	return aNewStr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString ResourceCollection::GetString(int theId)
{
	const ResourceEntry *anEntry = GetResource(theId,true,ResourceType_String);
	if(anEntry==NULL)
		return "Invalid String";

	if(!mDoOutStringFilter)
		return anEntry->mDesc;
	else
		return OutStringFilter(anEntry->mDesc);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
DWORD ResourceCollection::GetColor(int theId)
{
	const ResourceEntry *anEntry = GetResource(theId,true,ResourceType_Color);
	if(anEntry==NULL)
		return 0;

	return DecodeColor(anEntry->mDesc);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FontPtr ResourceCollection::GetFont(int theId)
{
	const ResourceEntry *anEntry = GetResource(theId,true,ResourceType_Color);
	if(anEntry==NULL)
		return WindowManager::GetDefaultWindowManager()->GetDefaultFont();

	return (Font*)anEntry->mData.get();
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ResourceCollectionWriter::ResourceCollectionWriter()
{
	mTrackByName = true;
	mDoInStringFilter = false;
	mCurId = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ResourceCollectionWriter::~ResourceCollectionWriter()
{
	Close();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceCollectionWriter::OpenNew(const GUIString &theFileName)
{
	WONFile aFile(theFileName);
	aFile.Remove();
	return Open(theFileName);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceCollectionWriter::Open(const GUIString &theFileName)
{
	ResourceCollection::Open(theFileName);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceCollectionWriter::Close()
{
	ResourceCollection::Close();
	mFileWriter.Close();
	mCurId = 0;
	mHadDelete = false;
	mHadRename = false;
	mHadIdChange = false;
	mHadAdd = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceCollectionWriter::Save()
{
	SaveAs(mFileName);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceCollectionWriter::SaveAs(const GUIString &theName)
{
	try
	{
		string anOldName = mFileName;
		string aNewName = theName;
		mFileName = theName;

		bool overwrite = mFileReader.IsOpen() && StringCompareNoCase(anOldName,aNewName)==0;		
		bool writeHeader = true;
		if(overwrite)
		{
			if(!IsModified()) // no change
				return;

			if(!IsHeaderModified())
				writeHeader = false;

			aNewName += ".new";
		}

		if(!mFileWriter.Open(aNewName.c_str(),true))
		{
			mErrorString = "Couldn't open file: " + aNewName;
			return;
		}

		WriteResources();
		if(writeHeader)
			WriteHeaderFile();

		Close();
		if(overwrite)
		{
			WONFile anOldFile = anOldName;
			anOldFile.Remove();
			rename(aNewName.c_str(),anOldName.c_str());
		}
	}
	catch(FileWriterException &anEx)
	{
		mErrorString = anEx.what;
	}

	Close();
	Open(mFileName);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceCollectionWriter::ExportToCFG(const GUIString &theName)
{
	string aName = theName;
	WONFile aWONFile(aName);
	aWONFile.Remove(true);


	FILE *aFile = fopen(aName.c_str(),"w");
	if(aFile==NULL)
		return false;

	ResourceIdMap::iterator anItr = mResourceIdMap.begin();
	while(anItr!=mResourceIdMap.end())
	{
		const ResourceEntry *anEntry = anItr->second;
		const char *aTypeName = "Unknown";
		switch(anEntry->mType)
		{
			case ResourceType_Image: aTypeName = "Image:"; break;
			case ResourceType_String: aTypeName = "String:"; break;
			case ResourceType_Binary: aTypeName = "Binary:"; break;
			case ResourceType_Color: aTypeName = "Color:"; break;
			case ResourceType_Font: aTypeName = "Font:"; break;
		}

		string aName = anEntry->mName;
		string aDesc = anEntry->mDesc;
		fprintf(aFile,"%-10s %-8d %-50s %s\n",aTypeName,anEntry->mId,aName.c_str(),aDesc.c_str());
		
		++anItr;
	}

	fclose(aFile);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceCollectionWriter::IsHeaderModified()
{
	if(mHadDelete || mHadRename || mHadIdChange || mHadAdd)
		return true;
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceCollectionWriter::IsModified()
{
	if(IsHeaderModified())
		return true;

	for(ResourceIdMap::iterator anItr=mResourceIdMap.begin(); anItr!=mResourceIdMap.end(); ++anItr)
	{
		if(anItr->second->NeedUpdate())
			return true;
	}	

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GUIString ResourceCollectionWriter::GetHeaderFileName(const GUIString &theFilePath)
{
	WONFile aFileParse(mFileName);
	string aName = aFileParse.GetFileNameWithoutExtension() + "WRS";
	string aFileName = aFileParse.GetFileDirectory() + WONFile::PATH_SEP + aName + ".h";
	return aFileName;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceCollectionWriter::WriteResourceCollectionResource(ResourceEntry *theEntry)
{	
	try
	{
		mFileReader.SetPos(theEntry->mFileOffset);

		const int CHUNK_SIZE = 32768;
		char aBuf[CHUNK_SIZE];
		int aBytesLeft = theEntry->mFileSize;
		while(aBytesLeft>0)
		{
			int aNumToRead = CHUNK_SIZE<aBytesLeft ? CHUNK_SIZE : aBytesLeft;
			mFileReader.ReadBytes(aBuf,aNumToRead);
			mFileWriter.WriteBytes(aBuf,aNumToRead);
			aBytesLeft-=aNumToRead;
		}
	}
	catch(FileReaderException&)
	{
/*		char aBuf[512];
		string aName = theEntry->mDesc;
		sprintf(aBuf,"Error reading old resource: %s\n",aName.c_str());

		throw FileWriterException(aBuf);*/
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceCollectionWriter::WriteFileResource(const ResourceEntry *theEntry)
{
	if(theEntry->mRawData.get()!=NULL)
	{
		mFileWriter.WriteBytes(theEntry->mRawData->data(),theEntry->mRawData->length());
		return;
	}

	string aFileName = theEntry->mDesc;
	if(aFileName.empty()) // prevent assert in MS library for fopen of empty string
		return;

	FILE *aFile = fopen(aFileName.c_str(),"rb");
	if(aFile==NULL)
		return;
//		throw FileWriterException("Couldn't open file: " + aFileName);

	AutoFile aCloseFile(aFile);

	const int CHUNK_SIZE = 32768;
	char aBuf[CHUNK_SIZE];
	int aLen;

	while(!feof(aFile))
	{
		aLen = fread(aBuf,1,CHUNK_SIZE,aFile);
		if(aLen<=0)
			break;

		mFileWriter.WriteBytes(aBuf,aLen);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceCollectionWriter::WriteAsciiString(const GUIString &theString)
{
	string aStr = theString;
	mFileWriter.WriteShort(aStr.length());
	mFileWriter.WriteBytes(aStr.data(),aStr.length());
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceCollectionWriter::WriteWideString(const GUIString &theString)
{
	mFileWriter.WriteShort(theString.length());
	for(int i=0; i<theString.length(); i++)
		mFileWriter.WriteShort(theString.at(i));
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceCollectionWriter::WriteResources()
{
	mFileWriter.WriteBytes("WRS",3);
	mFileWriter.WriteLong(RESOURCE_VERSION);

	mFileWriter.WriteLong(0); // save place for table of contents position
	int anItemCount = 0;
	for(ResourceIdMap::iterator anItr=mResourceIdMap.begin(); anItr!=mResourceIdMap.end(); ++anItr)
	{
		ResourceEntry *anEntry = const_cast<ResourceEntry*>(anItr->second.get());
		if(anEntry->NeedRemove())
			continue;

		anItemCount++;
		int aFileOffset = mFileWriter.pos();
		if(anEntry->NeedUpdate())
		{
			switch(anEntry->mType)
			{
				case ResourceType_None: break;
				case ResourceType_String: break;
				case ResourceType_Color: break;
				default: 
					WriteFileResource(anEntry);
			}
		}
		else
			WriteResourceCollectionResource(anEntry);

		anEntry->mFileOffset = aFileOffset;
	}		
	WriteTableOfContents(anItemCount);
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceCollectionWriter::WriteTableOfContents(int numItems)
{
	int aPos = mFileWriter.pos();
	mFileWriter.SetPos(7);
	mFileWriter.WriteLong(aPos);
	mFileWriter.SetPos(aPos);
	mFileWriter.WriteLong(numItems);
	for(ResourceIdMap::iterator anItr=mResourceIdMap.begin(); anItr!=mResourceIdMap.end(); ++anItr)
	{
		const ResourceEntry *anEntry = anItr->second;
		if(!anEntry->NeedRemove())
		{
			mFileWriter.WriteLong(anEntry->mId);
			mFileWriter.WriteShort((unsigned short)anEntry->mType);
			mFileWriter.WriteLong(anEntry->mFileOffset);
			WriteAsciiString(anEntry->mName);
			WriteWideString(anEntry->mDesc);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceCollectionWriter::WriteHeaderFile()
{
	WONFile aFileParse(mFileName);
	string aName = aFileParse.GetFileNameWithoutExtension() + "WRS";
	string aFileName = aFileParse.GetFileDirectory() + WONFile::PATH_SEP + aName + ".h";

	FILE *aFile = fopen(aFileName.c_str(),"w");
	if(aFile==NULL)
	{
		WONFile aHeaderFile(aFileName);
		aHeaderFile.SetReadOnly(false);
		aFile = fopen(aFileName.c_str(),"w");
		if(aFile==NULL)
		{
			mErrorString = "Couldn't open file: " + aFileName;
			return;
		}
	}

	fprintf(aFile,"#ifndef __%s_H__\n",aName.c_str());
	fprintf(aFile,"#define __%s_H__\n",aName.c_str());
	fprintf(aFile,"\n\n");
	fprintf(aFile,"enum\n");
	fprintf(aFile,"{\n");

	for(ResourceIdMap::iterator anItr=mResourceIdMap.begin(); anItr!=mResourceIdMap.end(); ++anItr)
	{
		const ResourceEntry *anEntry = anItr->second;
		if(!anEntry->NeedRemove())
		{
			string anEnumName = anEntry->mName;
			string aComment = anEntry->mDesc;
//			fprintf(aFile, "\t%s\t\t\t=\t%d,\t//%s\n",anEnumName.c_str(),anEntry->mId,aComment.c_str());
			fprintf(aFile, "\t%s = %d,\n",anEnumName.c_str(),anEntry->mId);
		}
	}

	fprintf(aFile,"};\n");
	fprintf(aFile,"\n\n");
	fprintf(aFile,"#endif\n");
	fclose(aFile);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
int ResourceCollectionWriter::GetNextFreeId()
{
	mCurId++;
	ResourceIdMap::iterator anItr = mResourceIdMap.find(mCurId);
	while(anItr!=mResourceIdMap.end())
	{
		if(mCurId==anItr->first)
		{
			++mCurId;
			++anItr;
		}
		else
			break;
	}
	
	return mCurId;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ResourceCollectionWriter::InStringFilter(ResourceEntry *theEntry)
{
	if(theEntry->mType!=ResourceType_String)
		return;

	if(!mDoInStringFilter)
		return;

	GUIString &aStr = theEntry->mDesc;
	GUIString aNewStr;
	int aStartPos = 0;
	while(true)
	{
		int aSlashPos = aStr.find('\\',aStartPos);
		if(aSlashPos<0) 
		{
			aSlashPos = aStr.length();
			break;
		}
		aNewStr.append(aStr,aStartPos,aSlashPos-aStartPos);
		aStartPos = aSlashPos+1;
		int aNextChar = aStr.at(aStartPos);
		switch(aNextChar)
		{			
			case 'n': aNewStr.append('\n'); break;
			case 't': aNewStr.append('\t'); break;
			case '\\': aNewStr.append('\\'); break;
		}
		aStartPos++;
	}

	if(aStartPos!=0) // found some slashes
	{
		if(aStartPos<aStr.length()) // get last bit
			aNewStr.append(aStr,aStartPos);

		theEntry->mDesc = aNewStr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
const ResourceEntry* ResourceCollectionWriter::AddResource(ResourceType theType, const GUIString &theName, const GUIString &theDesc, const ByteBuffer *theRawData)
{
	ResourceEntryPtr anEntry = new ResourceEntry;
	ResourceNameMap::iterator anItr = mResourceNameMap.insert(ResourceNameMap::value_type(theName,anEntry)).first;
	if(anItr->second.get()!=anEntry.get())
	{
		mCollisionEntry = anItr->second;
		return NULL;
	}

	mHadAdd = true;
	anEntry->mType = theType;
	anEntry->mName = theName;
	anEntry->mDesc = theDesc;
	anEntry->mRawData = theRawData;
	anEntry->mId = GetNextFreeId();
	mResourceIdMap[anEntry->mId] = anEntry;

	InStringFilter(anEntry);
	UpdateResource(anEntry);
	return anEntry;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceCollectionWriter::ModifyResource(const ResourceEntry *theEntry, const GUIString &theNewDesc)
{
	ResourceEntry *anEntry = const_cast<ResourceEntry*>(theEntry);
	anEntry->mDesc = theNewDesc;
	InStringFilter(anEntry);
	return UpdateResource(theEntry);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceCollectionWriter::ModifyResourceId(const ResourceEntry *theEntry, int theNewId)
{
	if(theNewId<=0)
		return false;

	ResourceIdMap::iterator anItr = mResourceIdMap.find(theNewId);
	if(anItr!=mResourceIdMap.end())
	{
		if(anItr->second==theEntry)
			return true;
		else
			return false;
	}

	if(theNewId >= mCurId)
		mCurId++;

	ResourceEntry *anEntry = const_cast<ResourceEntry*>(theEntry);
	int anOldId = anEntry->mId;
	anEntry->mId = theNewId;

	mResourceIdMap[theNewId] = anEntry;
	mResourceIdMap.erase(anOldId);
	mHadIdChange = true;
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceCollectionWriter::UpdateResource(const ResourceEntry *theEntry)
{
	ResourceEntry *anEntry = const_cast<ResourceEntry*>(theEntry);
	anEntry->mUpdateType = ResourceUpdateType_Update;
	anEntry->mData = NULL;
	if(anEntry->mType!=ResourceType_String)
	{
		WONFile aFile = (const std::string&)anEntry->mDesc;
		anEntry->mFileSize = aFile.GetSize();
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceCollectionWriter::RenameResource(const ResourceEntry *theEntry, const GUIString &theNewName)
{
	const ResourceEntry* aNew = GetResource(theNewName,false);
	if(aNew!=NULL)
	{
		if(aNew==theEntry) // renaming to self
			return true;

		mCollisionEntry = aNew;
		return false;
	}

	ResourceEntryPtr anEntry = const_cast<ResourceEntry*>(theEntry);
	RemoveEntry(theEntry,true);

	mHadRename = true;

	anEntry->mName = theNewName;
	AddEntry(anEntry);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResourceCollectionWriter::DeleteResource(const ResourceEntry *theEntry)
{
	if(RemoveEntry(theEntry,false))
	{
		mHadDelete = true;
		return true;
	}
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/*
GUIString ResourceCollectionWriter::GetTempName(const GUIString &thePrefix)
{
	int anId = mCurId;
	GUIString aName;
	while(true)
	{
		anId++;

		char aBuf[50];
		sprintf(aBuf,"%d",anId);
		aName = thePrefix;
		aName.append(aBuf);
		if(mResourceNameMap.find(aName)==mResourceNameMap.end())
			break;
	}

	return aName;
}
*/