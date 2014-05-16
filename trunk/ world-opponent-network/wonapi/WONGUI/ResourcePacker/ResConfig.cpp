#include "ResConfig.h"

using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ResConfig::ResConfig(ResourceCollectionWriter &theWriter) : mResourceCollection(theWriter)
{
	mTypeMap["String"] = ResourceType_String;
	mTypeMap["Image"] = ResourceType_Image;
	mTypeMap["Binary"] = ResourceType_Binary;
	mTypeMap["Color"] = ResourceType_Color;
	mTypeMap["Font"] = ResourceType_Font;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ResConfig::HandleKeyVal(ConfigParser &theParser, const std::string& theKey, StringParser &theVal)
{
	if(ConfigObject::HandleKeyVal(theParser,theKey,theVal))
		return true;

	ResourceTypeMap::iterator anItr = mTypeMap.find(theKey);
	if(anItr==mTypeMap.end())
	{
		theParser.AbortRead("Unknown resource type: " + theKey);
		return false;
	}
	ResourceType aType = anItr->second;

	int anId = 0;
	string aName;
	wstring aDesc;
	bool wantId = theVal.ReadValue(anId);
	theVal.ReadValue(aName,true);
	theVal.ReadValue(aDesc,false);

	if(wantId && anId<=0)
	{
		theParser.AbortRead("Invalid resource id.");
		return false;
	}


	if(aName.empty())
	{
		theParser.AbortRead("Inavlid resource name.");
		return false;
	}

	const ResourceEntry *anEntry = mResourceCollection.AddResource(aType,aName,aDesc);
	if(anEntry==NULL)
	{
		theParser.AbortRead("Duplicate resource name.");
		return false;
	}

	if(wantId && !mResourceCollection.ModifyResourceId(anEntry,anId))
	{
		theParser.AbortRead("Duplicate resource id.");
		return false;
	}

	return true;
}
