#ifndef __WON_GUISTRING_H__
#define __WON_GUISTRING_H__

#include "WONCommon/SmartPtr.h"
#include <wchar.h>


namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GUIStringData : public RefCount
{
public:
#ifdef _LINUX
#define GUISTRING_ASCII
#endif

#ifdef GUISTRING_ASCII
	typedef unsigned char CharData;
#else
	typedef wchar_t CharData;
#endif

	CharData *mData;
	unsigned short mLength;
	unsigned short mCapacity;

	~GUIStringData();

	void Init();
	void AssignStr(const char *theStr, int theLen);
	void AssignWStr(const wchar_t *theStr, int theLen);

	void Fill(int theChar, unsigned short theOffset, unsigned short theLength);

public:

	GUIStringData();
	explicit GUIStringData(unsigned short theCapacity);
	GUIStringData(GUIStringData *theCopy);
	GUIStringData(GUIStringData *theCopy, unsigned short theOffset, unsigned short theLength);
	GUIStringData(const char *theStr);
	GUIStringData(const wchar_t *theStr);
	GUIStringData(const char *theStr, unsigned short theLen);
	GUIStringData(const wchar_t *theStr, unsigned short theLen);
	GUIStringData(const std::string &theStr);
	GUIStringData(const std::wstring &theStr);

	CharData* data() { return mData; }
	const CharData* data() const { return mData; }
	unsigned short length() const { return mLength; }
	bool empty() const { return mLength==0; }
	int compare(const GUIStringData *theStr) const;
	int compareNoCase(const GUIStringData *theStr) const;

	int compare(const char *theStr, int theOffset) const;
	int compareNoCase(const char *theStr, int theOffset) const;

	int atoi(unsigned short theOffset) const;
	int find(int theChar, int theStartPos) const;
	int find(const GUIStringData *theStr, int theStartPos) const;
	int findNoCase(const GUIStringData *theStr, int theStartPos) const;
	int findOneOf(const GUIStringData *theStr, int theStartPos) const;
	int rFind(int theChar, int theStartPos) const;
	int rFind(const GUIStringData* theStr, int theStartPos) const;

	std::string GetAsString() const;
	std::wstring GetAsWString() const;

	int at(int thePos) const { return (unsigned short)mData[thePos]; }
	int setAt(int thePos, int theChar);

	// inline for efficiency
	void reserve(int theNewCapacity, bool chunkAlloc = false)
	{
		theNewCapacity++;
		if(mCapacity < theNewCapacity)
		{
			CharData *anOldData = mData;
			if(chunkAlloc)
				theNewCapacity *= 2;

			mData = new CharData[theNewCapacity];
			if(anOldData!=NULL)
			{
				memcpy(mData,anOldData,sizeof(CharData)*(mLength+1));
				delete [] anOldData;
			}
			mCapacity = (unsigned short)theNewCapacity;
		}
	}

	void resize(int theNewLength, bool chunkAlloc = false)
	{
		reserve(theNewLength,chunkAlloc);
		mLength = (unsigned short)theNewLength;
		mData[mLength] = 0;
	}

	void append(int theChar) 
	{ 	
		resize(mLength+1,true); 
		mData[mLength-1] = (CharData)theChar; 
	}
	
	void append(const GUIStringData *theStr, unsigned short theOffset, unsigned short theLength)
	{
		int anOldLength = mLength;
		resize(mLength+theLength,true);
		memcpy(mData+anOldLength, theStr->data()+theOffset,sizeof(CharData)*theLength);
	}

	void erase(int theStartChar, int theLength);

	void insert(int theStartChar, const GUIStringData *theStr);

	void assign(const GUIStringData *theStr, unsigned short theOffset, unsigned short theLength);
	void toUpperCase();
	void toLowerCase();
};
typedef SmartPtr<GUIStringData> GUIStringDataPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GUIString
{
protected:
	GUIStringDataPtr mRep;

	static GUIStringData* GetNullStr();
	void CopyOnWrite() { if(mRep->GetRefCount()>1) mRep = new GUIStringData(mRep); }
public:
	const void* GetInternalData() const { return mRep->data(); }
	static const GUIString EMPTY_STR;

public:
	GUIString() : mRep(GetNullStr()) { }
	GUIString(GUIStringData *theData) : mRep(theData) { }
	GUIString(const char *theStr) : mRep(new GUIStringData(theStr)) { }
	GUIString(const wchar_t *theStr) : mRep(new GUIStringData(theStr)) { }
	GUIString(const char *theStr, unsigned short theLen) : mRep(new GUIStringData(theStr,theLen)) { }
	GUIString(const wchar_t *theStr, unsigned short theLen) : mRep(new GUIStringData(theStr,theLen)) { }
	GUIString(const std::string &theStr) : mRep(new GUIStringData(theStr)) { }
	GUIString(const std::wstring &theStr) : mRep(new GUIStringData(theStr)) { }
	explicit GUIString(unsigned short theCapacity) : mRep(new GUIStringData(theCapacity)) { }

	bool empty() const { return mRep->empty(); }
	unsigned short length() const { return mRep->length(); }
	unsigned short size() const { return mRep->length(); }
	int at(int thePos) const { return mRep->at(thePos); }
	int setAt(int thePos, int theChar) { CopyOnWrite(); return mRep->setAt(thePos, theChar); }

	operator std::string() const { return mRep->GetAsString(); }
	operator std::wstring() const { return mRep->GetAsWString(); }
	
	int compare(const GUIString &theStr) const { return mRep->compare(theStr.mRep); }
	int compare(const char *theStr, int theOffset=0) const { return mRep->compare(theStr,theOffset); }
	int compareNoCase(const GUIString &theStr) const { return mRep->compareNoCase(theStr.mRep); }
	int compareNoCase(const char *theStr, int theOffset=0) const { return mRep->compareNoCase(theStr,theOffset); }

	bool operator ==(const GUIString &theStr) const { return mRep->compare(theStr.mRep)==0; }
	bool operator !=(const GUIString &theStr) const { return mRep->compare(theStr.mRep)!=0; }
	bool operator <(const GUIString &theStr)  const { return mRep->compare(theStr.mRep)<0;  }
	bool operator <=(const GUIString &theStr) const { return mRep->compare(theStr.mRep)<=0; }
	bool operator >(const GUIString &theStr)  const { return mRep->compare(theStr.mRep)>0;  }
	bool operator >=(const GUIString &theStr) const { return mRep->compare(theStr.mRep)>=0; }

	int atoi(unsigned short theOffset=0) const { return mRep->atoi(theOffset); }
	int find(int theChar, int theStartPos = 0) const { return mRep->find(theChar,theStartPos); }
	int find(const GUIString &theStr, int theStartPos = 0) const { return mRep->find(theStr.mRep,theStartPos); }
	int findNoCase(const GUIString &theStr, int theStartPos = 0) const { return mRep->findNoCase(theStr.mRep,theStartPos); }
	int findOneOf(const GUIString &theStr, int theStartPos = 0) const { return mRep->findOneOf(theStr.mRep,theStartPos); }
	int rFind(const GUIString &theStr, int theStartPos = -1) const { return mRep->rFind(theStr.mRep,theStartPos); }
	int rFind(const int theChar, int theStartPos = -1) const { return mRep->rFind(theChar,theStartPos); }

	// Copy on write
	void Fill(int theChar, unsigned short theOffset, unsigned short theLength) 
	{ CopyOnWrite(); mRep->Fill(theChar,theOffset,theLength); }

	void resize(int theNewLength) { CopyOnWrite(); mRep->resize(theNewLength); }
	void reserve(int theLength) { mRep->reserve(theLength); }

	void append(int theChar) { CopyOnWrite(); mRep->append(theChar); }
	void append(const GUIString& theStr) { append(theStr,0,theStr.length()); }
	void append(const GUIString& theStr, unsigned short theOffset) { append(theStr,theOffset,theStr.length()-theOffset); }
	void append(const GUIString& theStr, unsigned short theOffset, unsigned short theLength) 
	{ CopyOnWrite(); mRep->append(theStr.mRep,theOffset,theLength); }

	void assign(const GUIString& theStr) { assign(theStr,0,theStr.length()); }
	void assign(const GUIString& theStr, unsigned short theOffset) { assign(theStr,theOffset,theStr.length()-theOffset); }
	void assign(const GUIString& theStr, unsigned short theOffset, unsigned short theLength) 
	{ CopyOnWrite(); mRep->assign(theStr.mRep,theOffset,theLength); }

	void toUpperCase() { CopyOnWrite(); mRep->toUpperCase(); }
	void toLowerCase() { CopyOnWrite(); mRep->toLowerCase(); }

	GUIString substr(unsigned short theOffset, unsigned short theLength) const { return GUIString(new GUIStringData(mRep,theOffset,theLength)); }
	GUIString substr(unsigned short theOffset) const { return GUIString(new GUIStringData(mRep,theOffset,length()-theOffset)); }

	void erase() { mRep = GetNullStr(); } // Don't need CopyOnWrite since changing mRepPtr and not data in mRep
	void erase(int theStartChar, int theLength = 0) { CopyOnWrite(); mRep->erase(theStartChar, theLength); }

	void insert(int theStartChar, const GUIString &theStr) { CopyOnWrite(); mRep->insert(theStartChar, theStr.mRep); }

};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GUIStringLessNoCase
{
public:
	bool operator()(const GUIString &s1, const GUIString &s2) const { return s1.compareNoCase(s2)<0; }
};

}; // namespace WONAPI;

#endif