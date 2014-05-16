#ifndef __WON_BrowserComponent_H__
#define __WON_BrowserComponent_H__

#include "WONGUI/Button.h"
#include "WONGUI/InputBox.h"
#include "WONGUI/Table.h"
#include "WONGUI/Label.h"
#include "WONGUI/Dialog.h"

#include "WONGUI/HTMLParser.h"
#include "WONGUI/HTMLDisplayGen.h"
#include "WONGUI/HTMLDocument.h"

#include "WONMisc/HTTPEngine.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class BrowserComponent;
typedef SmartPtr<BrowserComponent> BrowserComponentPtr;
	
class BrowserComponent : public Container
{
protected:
	TablePtr mHTMLView;
	ScrollerPtr mScroller;
	HTMLFontSize mFontSize;

	HTTPCachePtr		mHTTPCache;
	HTTPSessionPtr		mHTTPSession;
	HTMLDocumentPtr		mHTMLDocument;
	bool mHaveBackground;

	GUIString mCurStatus;
	GUIString mCurPath;
	GUIString mCurAnchor;
	std::string mHTMLDocumentPath;
	bool mLaunchBrowserOnLink;
	int mNumPendingOps;
	int mVertOffset;

	void SetStatus(const GUIString &theStatus);
	void GotoAnchor();
	void RememberVertOffset();

	void FinishImages();
	bool DownloadImages();
	void ParseDocument();
	void GetLinkPath(const std::string &theLink, std::string &thePath);

	struct ImageParam : public RefCount
	{
		BrowserComponentPtr mBrowser;
		HTMLDocument::LoadedImageStruct *mImageStruct;

		ImageParam(BrowserComponent *theComp, HTMLDocument::LoadedImageStruct *theStruct) :
			mBrowser(theComp), mImageStruct(theStruct) { }
	};
	typedef SmartPtr<ImageParam> ImageParamPtr;


	static void DownloadImageCompletion(AsyncOpPtr theOp, ImageParamPtr theParam);
	static void DownloadDocumentCompletion(AsyncOpPtr theOp, RefCountPtr that);

	struct PageLocation
	{
		GUIString mLocation;
		int mVertOffset;

		PageLocation() : mVertOffset(0) { }
		PageLocation(const GUIString &theLocation, int theOffset) : mLocation(theLocation), mVertOffset(theOffset) { }
	};
	typedef std::vector<PageLocation> PageVector;
	PageVector mPageVector;
	int mCurPage;
	GUIString mCurLocation;

	virtual ~BrowserComponent();

public:
	virtual void HandleComponentEvent(ComponentEvent *theEvent);
	virtual void AddedToParent();

public:
	BrowserComponent();
	Table* GetHTMLView() { return mHTMLView; }
	void SetScroller(Scroller *theScroller);
	void SetFontSize(HTMLFontSize theSize) { mFontSize = theSize; }

	void Refresh();
	void Stop();
	void Go(HTMLDocument *theDocument);
	void Go(const GUIString &theLocation, bool force = false, bool wasBackTraversal = false, int theVertOffset = -1);
	void Back();
	void Forward();
	bool FindStr(const GUIString &theStr, bool next = true, bool caseSensitive = false);
	void Reset();

	void SetLaunchBrowserOnLink(bool launch) { mLaunchBrowserOnLink = launch; }
	void SetBackground(const Background &theBackground);
	void RemoveBackground();

	bool StopButtonEnabled();
	bool BackButtonEnabled();
	bool ForwardButtonEnabled();
	const GUIString& GetCurStatus() { return mCurStatus; }
	const GUIString& GetCurLocation() { return mCurLocation; }
};

}; // namespace WONAPI


#endif