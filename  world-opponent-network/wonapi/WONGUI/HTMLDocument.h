#ifndef __WON_HTMLIMAGEFETCHER_H__
#define __WON_HTMLIMAGEFETCHER_H__

#include "HTMLTag.h"
#include "Table.h"
#include "RawImage.h"
#include "NativeImage.h"
#include "Animation.h"
#include "Window.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class HTMLDocument : public RefCount
{
protected:
	virtual ~HTMLDocument();

public:
	///////////////////////////////////////////
	// Images
	struct LoadedImageStruct
	{
		RawImagePtr mRawImage; // for scaling
		NativeImagePtr mImage;
		AnimationPtr mAnimation;
		bool mHaveUsedAnimation;

		LoadedImageStruct(NativeImage *theImage = NULL, Animation *theAnimation = NULL) :
			mImage(theImage), mAnimation(theAnimation), mHaveUsedAnimation(false) { }
	};
	typedef std::map<GUIString,LoadedImageStruct,GUIStringLessNoCase> LoadedImageMap;

protected:
	GUIString mBasePath;
	Window* mWindow;
	bool mDelayImages;

	///////////////////////////////////////////
	// Document
	HTMLTagList mTagList;
	HTMLTagList::iterator mTagItr;
	HTMLTagList::iterator mBodyStart;
	HTMLTagList::iterator mBodyEnd;

	///////////////////////////////////////////
	// Images
	LoadedImageMap mLoadedImageMap;
	LoadedImageMap mScaledImageMap;
	bool mDecodingImages;

	HTMLTagList mImageTagList;



	LoadedImageStruct* GetImage(const GUIString &thePath, HTMLTag *theTag);
	NativeImage* CheckScaleImage(LoadedImageStruct *theStruct, const GUIString &thePath, int theWidth, int theHeight);

	///////////////////////////////////////////
	// Image Maps
	typedef std::map<GUIString,ImageMapPtr,GUIStringLessNoCase> ImageMapMap;
	ImageMapPtr mCurImageMap;
	ImageMapMap mImageMapMap;

	///////////////////////////////////////////
	// Lists
	struct ListInfo
	{
		bool mOrdered;
		HTMLListType mType;
		int mCount;
	};
	typedef std::vector<ListInfo> ListStack;
	ListStack mListStack;

	
	void HandleList(HTMLListTag *theTag);
	void HandleListItem(HTMLListItemTag *theTag);

	void HandleImage(HTMLImageTag *theTag);
	void HandleImageMap(HTMLImageMapTag *theTag);
	void HandleArea(HTMLAreaTag *theTag);

	void HandleInput(HTMLInputTag* aTag); 
	void HandleBody(HTMLBodyTag *theTag);

	typedef std::multimap<GUIString,HTMLTagPtr,GUIStringLessNoCase> ImageTagMap;
	ImageTagMap mImageTagMap;

public:
	HTMLDocument(const GUIString &theBasePath = GUIString::EMPTY_STR);
	void SetBasePath(const GUIString &theBasePath);
	void AddTag(HTMLTag *theTag);
	void AddDocument(HTMLDocument *theDocument);
	void DecodeImages(bool continueDelay);
	void SetDelayImages(bool delay) { mDelayImages = delay; }

	bool DoDecodeImage(LoadedImageStruct *theStruct, Window *theWindow, const std::string &theImagePath);
	LoadedImageMap& GetLoadedImageMap() { return mLoadedImageMap; }

	HTMLTagList& GetTagList() { return mTagList; }
	HTMLTagList::iterator GetBodyStart();
	HTMLTagList::iterator GetBodyEnd();

	void Rewind(bool beginAtBodyStart = true);
	HTMLTag* GetNextTag(bool stopAtBodyEnd = true);

	void Clear();
};
typedef SmartPtr<HTMLDocument> HTMLDocumentPtr;

}; // namespace WONAPI

#endif