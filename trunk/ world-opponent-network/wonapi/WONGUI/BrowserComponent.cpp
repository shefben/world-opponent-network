#include "BrowserComponent.h"

#include "WONGUI/MSControls.h"
#include "WONGUI/ChildLayouts.h"
#include "WONGUI/GUICompat.h"
#include "WONGUI/GUICompletion.h"
#include "WONCommon/MiscUtil.h"
#include "WONCommon/Thread.h"


using namespace WONAPI;
using namespace std;


namespace WONAPI
{
/*
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class BrowserCompletion : public OpCompletionBase
{
private:
	virtual void Complete(AsyncOpPtr theOp);
	OpCompletionBasePtr mCompletion;
	
public:
	BrowserCompletion(OpCompletionBase *theCompletion) : mCompletion(theCompletion) { }
	void DoBrowserComplete(AsyncOp *theOp);
};
typedef SmartPtr<BrowserCompletion> BrowserCompletionPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class BrowserCompletionEvent : public WindowEventBase
{
protected:
	BrowserCompletionPtr mCompletion;
	AsyncOpPtr mOp;

public:
	BrowserCompletionEvent(AsyncOp *theOp, BrowserCompletion *theCompletion) :
	  mCompletion(theCompletion), mOp(theOp) { }

	virtual void Deliver()
	{
		mCompletion->DoBrowserComplete(mOp);
	}		
};
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BrowserCompletion::Complete(AsyncOpPtr theOp)
{
	if(Thread::GetCurThreadId()==WindowManager::GetThreadId())
		DoBrowserComplete(theOp);
	else
	{
		WindowManager *aMgr = WindowManager::GetDefaultWindowManager();
		if(aMgr!=NULL)
			aMgr->PostEvent(new BrowserCompletionEvent(theOp,this));
	}	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BrowserCompletion::DoBrowserComplete(AsyncOp *theOp)
{
	mCompletion->Complete(theOp);
}*/


} // namespace WONAPI

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
BrowserComponent::BrowserComponent()
{
	mLaunchBrowserOnLink = false;

	mFontSize = HTMLFontSize_Medium;
	mHTTPSession = new HTTPSession;
	mHTTPCache = HTTPCache::GetGlobalCache();

	mCurPage = 0;
	mNumPendingOps = 0;
	mVertOffset = -1;

	mHTMLView = new Table;
	mHaveBackground = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
BrowserComponent::~BrowserComponent()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BrowserComponent::AddedToParent()
{	
	if(mScroller.get()==NULL) // set default scroller
	{
		MSScrollContainerPtr aContainer = new MSScrollContainer(mHTMLView);
		aContainer->SetScrollbarConditions(ScrollbarCondition_Conditional,ScrollbarCondition_Conditional);
		SetScroller(aContainer);
	}

	Container::AddedToParent();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BrowserComponent::SetScroller(Scroller *theScroller)
{
	mScroller = theScroller;
	if(theScroller->GetScrollArea()!=mHTMLView)
		theScroller->SetScrollArea(mHTMLView);

	AddChildLayout(theScroller,CLI_SameSize,this);
	AddChild(theScroller);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool BrowserComponent::StopButtonEnabled()
{
	if(mHTTPSession.get()==NULL)
		return false;
	else
		return mNumPendingOps>0; //mHTTPSession->GetNumOps()>0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool BrowserComponent::BackButtonEnabled()
{
	return !mPageVector.empty() && mCurPage>0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool BrowserComponent::ForwardButtonEnabled()
{
	return !mPageVector.empty() && mCurPage<mPageVector.size()-1;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BrowserComponent::SetStatus(const GUIString &theStatus)
{
	mCurStatus = theStatus;
	FireEvent(ComponentEvent_BrowserStatusChange);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BrowserComponent::DownloadDocumentCompletion(AsyncOpPtr theOp, RefCountPtr that)
{
	BrowserComponent *aComponent = (BrowserComponent*)that.get();
	HTTPGetOp *anOp = (HTTPGetOp*)theOp.get();

	if(anOp->Killed())
		return;

	char aBuf[512];
	sprintf(aBuf,"Got %s%s - %s %d",anOp->GetHost().c_str(),anOp->GetRemotePath().c_str(),WONStatusToString(anOp->GetStatus()),anOp->GetHTTPStatus());
	aComponent->SetStatus(aBuf);
//	OutputDebugString(aBuf); OutputDebugString("\n");
	aComponent->mNumPendingOps--;


	if(!anOp->Succeeded())
	{
		aComponent->FireEvent(ComponentEvent_BrowserButtonChange);
		aComponent->SetStatus(aBuf);
		return;
	}


	aComponent->mHTMLDocumentPath = anOp->GetLocalPath();
	aComponent->mCurPath = "http://" + anOp->GetEndingHost() + anOp->GetEndingPath();
	aComponent->ParseDocument();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BrowserComponent::FinishImages()
{
	mHTMLDocument->DecodeImages(false);

	MultiLineElementPtr anElement = new MultiLineElement;
	HTMLDisplayGen aGen;
	aGen.SetFontSize(mFontSize);
	aGen.SetNoBackground(mHaveBackground);
	aGen.Generate(mHTMLDocument,anElement,mHTMLView);
	mHTMLView->SetContents(anElement);

	SetStatus("");
	mHTTPSession->Kill();
	mNumPendingOps = 0;
	FireEvent(ComponentEvent_BrowserButtonChange);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BrowserComponent::DownloadImageCompletion(AsyncOpPtr theOp, ImageParamPtr theParam)
{
	HTTPGetOp *anOp = (HTTPGetOp*)theOp.get();
	char aBuf[1024];
	if(anOp->Killed())
	{
		sprintf(aBuf,"Killed %s%s.",anOp->GetHost().c_str(),anOp->GetRemotePath().c_str());
//		OutputDebugString(aBuf); OutputDebugString("\n");
		theParam->mBrowser->SetStatus(aBuf);
		return;
	}

	int anHTTPStatus = anOp->GetHTTPStatus();
	HTMLDocument::LoadedImageStruct *aStruct = theParam->mImageStruct;
	if(anOp->Succeeded())
	{
		if(aStruct->mImage.get()!=NULL)
		{
			if(anHTTPStatus>=200 && anHTTPStatus<300) // not just a cache hit
				aStruct->mImage = NULL;
		}

		if(aStruct->mImage.get()==NULL)
			theParam->mBrowser->mHTMLDocument->DoDecodeImage(theParam->mImageStruct,theParam->mBrowser->GetWindow(),anOp->GetLocalPath());
	}


	sprintf(aBuf,"Got %s%s - %s %d",anOp->GetHost().c_str(),anOp->GetRemotePath().c_str(),WONStatusToString(anOp->GetStatus()),anHTTPStatus);
//	OutputDebugString(aBuf); OutputDebugString("\n");
	theParam->mBrowser->SetStatus(aBuf);

//	int aNumOps = theParam->mBrowser->mHTTPSession->GetNumOps();
	int aNumOps = --theParam->mBrowser->mNumPendingOps;
	if(aNumOps<=0)
	{
		// done downloading
		theParam->mBrowser->FinishImages();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool BrowserComponent::DownloadImages()
{
	Window *aWindow = GetWindow();

	HTMLDocument::LoadedImageMap &aMap = mHTMLDocument->GetLoadedImageMap();

	HTMLDocument::LoadedImageMap::iterator anItr = aMap.begin();
	while(anItr!=aMap.end())
	{
		HTMLDocument::LoadedImageStruct &aStruct = anItr->second;

		URL aPathURL(anItr->first);
		URL aBasePathURL(mCurPath);

		aPathURL.CalcRelativeTo(aBasePathURL);

		if(aPathURL.GetType()==URLType_File)
			mHTMLDocument->DoDecodeImage(&aStruct,aWindow,aPathURL.GetPathAsLocalFilePath());
		else if(aPathURL.GetType()==URLType_HTTP)
		{
			HTTPGetOpPtr anOp = new HTTPGetOp(aPathURL.GetAddress(),aPathURL.GetPath());
			HTTPCacheEntryPtr anEntry = mHTTPCache->GetEntry(aPathURL.GetAddress() + aPathURL.GetPath());
			if(anEntry->IsValid())
				mHTMLDocument->DoDecodeImage(&aStruct,aWindow,anEntry->GetLocalPath());

			anOp->SetHTTPCache(mHTTPCache);
			anOp->SetCompletion(new OpGUICompletion(new ParamCompletion<AsyncOpPtr,ImageParamPtr>(DownloadImageCompletion,new ImageParam(this,&aStruct))));
			mHTTPSession->AddOp(anOp);
			mNumPendingOps++;
		}

		++anItr;
	}

	//if(mHTTPSession->GetNumOps()==0)
	if(mNumPendingOps<=0) 
	{
		FinishImages();
		return true;
	}
	else
		return false;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BrowserComponent::ParseDocument()
{
	if(mHTMLDocumentPath.empty())
	{
		Stop();
		return;
	}

	HTMLParser aParser;
	aParser.SetDocument(new HTMLDocument(mCurPath));
	aParser.ParseFromFile(mHTMLDocumentPath.c_str());
	mHTMLDocument = aParser.GetDocument();

	MultiLineElementPtr anElement = new MultiLineElement;
	if(!DownloadImages()) // generate without images for now
	{
		HTMLDisplayGen aGen;
		aGen.SetFontSize(mFontSize);
		mHTMLDocument->DecodeImages(true);
		aGen.SetNoBackground(mHaveBackground);
		aGen.Generate(mHTMLDocument,anElement,mHTMLView);
		mHTMLView->SetContents(anElement);
	}

	GotoAnchor();

	mCurLocation = mCurPath;
	FireEvent(ComponentEvent_BrowserLocationChange);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BrowserComponent::Reset()
{
	mPageVector.clear();
	mCurPage = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BrowserComponent::Refresh()
{
	if(!mPageVector.empty())
		Go(mCurLocation,true,true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BrowserComponent::Stop()
{
	mHTTPSession->Kill();
	mNumPendingOps = 0;
	FireEvent(ComponentEvent_BrowserButtonChange);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BrowserComponent::GotoAnchor()
{
	if(mVertOffset>=0) // this was a back or forward traversal and we were somewhere else on the page
	{
		mHTMLView->SetOffsets(0,mVertOffset);
		return;
	}

	mHTMLView->SetOffsets(0,0);
	if(mCurAnchor.empty())
		return;

	mHTMLView->GotoAnchor(mCurAnchor);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BrowserComponent::Go(HTMLDocument *theDocument)
{
	if(theDocument==NULL)
		return;

	Stop();

	mHTMLDocument = theDocument;
	MultiLineElementPtr anElement = new MultiLineElement;
	if(!DownloadImages()) // generate without images for now
	{
		HTMLDisplayGen aGen;
		aGen.SetFontSize(mFontSize);
		aGen.SetNoBackground(mHaveBackground);
		aGen.Generate(mHTMLDocument,anElement,mHTMLView);
		mHTMLView->SetContents(anElement);
		mHTMLView->SetOffsets(0,0);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static bool PathsAreEqual(const GUIString &p1, const GUIString &p2)
{
	if(p1.length()!=p2.length())
		return false;

	for(int i=0; i<p1.length(); i++)
	{
		if(p1.at(i)!=p2.at(i))
		{
			int p1c = p1.at(i);
			int p2c = p2.at(i);
			bool isSlash1 = p1c=='\\' || p1c=='/';
			bool isSlash2 = p2c=='\\' || p2c=='/';
			if(!isSlash1 || !isSlash2)
				return false;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BrowserComponent::RememberVertOffset()
{
	if(mCurPage>=0 && mCurPage<mPageVector.size()) // remember the current vert offset in this page
		mPageVector[mCurPage].mVertOffset = mHTMLView->GetVertOffset();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BrowserComponent::Go(const GUIString &theLocation, bool force, bool wasBackTraversal, int theVertOffset)
{
	Stop();

	int aPoundPos = theLocation.find('#');	
	GUIString aPath, anAnchor;
	if(aPoundPos>=0)
	{
		aPath = theLocation.substr(0,aPoundPos);
		anAnchor = theLocation.substr(aPoundPos+1);
	}
	else
		aPath = theLocation;

	mCurAnchor = anAnchor;


	if(!wasBackTraversal)
	{
		RememberVertOffset();
		if(!mPageVector.empty() && mCurPage<mPageVector.size()+1)
			mPageVector.erase(mPageVector.begin() + mCurPage + 1, mPageVector.end());

		if(mPageVector.empty() || theLocation!=mPageVector.back().mLocation)
		{
			mPageVector.push_back(PageLocation(theLocation,0));

			const int aMaxHistory = 100;
			if(mPageVector.size() > aMaxHistory)
				mPageVector.erase(mPageVector.begin(), mPageVector.begin() + mPageVector.size() - aMaxHistory);
		}

		mCurPage = mPageVector.size()-1;
	}
	mCurLocation = theLocation;
	mVertOffset = theVertOffset;

	FireEvent(ComponentEvent_BrowserLocationChange);
	FireEvent(ComponentEvent_BrowserButtonChange);

	if(PathsAreEqual(aPath,mCurPath) && !force)
	{
		GotoAnchor();
		return;
	}

	if(aPath.empty())
		return;

	if(mLaunchBrowserOnLink)
	{
		Browse(theLocation);
		return;
	}

	URL aPathURL(aPath);
	mCurPath = aPath;

	if(aPathURL.GetType()==URLType_File)
	{
		mHTMLDocumentPath = aPathURL.GetPathAsLocalFilePath();
		ParseDocument();
	}
	else if(aPathURL.GetType()==URLType_HTTP)
	{
		HTTPGetOpPtr anOp = new HTTPGetOp(aPathURL.GetAddress(),aPathURL.GetPath());
		anOp->SetHTTPCache(mHTTPCache);
		anOp->SetCompletion(new OpGUICompletion(new OpRefCompletion(DownloadDocumentCompletion,this)));
		mHTTPSession->AddOp(anOp);
		mNumPendingOps++;
		mHTMLView->SetContents(new MultiLineElement);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BrowserComponent::Back()
{
	if(!mPageVector.empty() && mCurPage>0)
	{
		RememberVertOffset();

		mCurPage--;
		PageLocation &aLoc = mPageVector[mCurPage];
		Go(aLoc.mLocation,false,true,aLoc.mVertOffset);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BrowserComponent::Forward()
{
	if(!mPageVector.empty() && mCurPage<mPageVector.size()-1)
	{
		RememberVertOffset();

		mCurPage++;
		PageLocation &aLoc = mPageVector[mCurPage];
		Go(aLoc.mLocation,false,true,aLoc.mVertOffset);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BrowserComponent::GetLinkPath(const std::string &theLink, std::string &thePath)
{
	URL aLink(theLink);
	string aStr = mCurPath;
	aLink.CalcRelativeTo(aStr);
	aLink.GetEntire(thePath);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool BrowserComponent::FindStr(const GUIString &theStr, bool next, bool caseSensitive)
{
	return mHTMLView->FindStr(theStr,next,caseSensitive);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BrowserComponent::SetBackground(const Background &theBackground)
{
	mHaveBackground = true;
	mHTMLView->SetBackground(theBackground);
}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BrowserComponent::RemoveBackground()
{
	mHaveBackground = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
/*
void BrowserComponent::SetTransparent(bool isTransparent)
{
	mIsTransparent = isTransparent;
	if(mIsTransparent)
		mHTMLView->SetBackground(-1);

	mHTMLView->SetComponentFlags(ComponentFlag_GrabBG,mIsTransparent);
}*/

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BrowserComponent::HandleComponentEvent(ComponentEvent *theEvent)
{
	switch(theEvent->mType)
	{
		case ComponentEvent_HyperLinkActivated:
		{
			HyperLinkEvent *anEvent = (HyperLinkEvent*)theEvent;
			const std::string &aLink = anEvent->mLinkParam->mLinkParam;
			string aPath; 
			GetLinkPath(aLink,aPath);
	
			int aKeyMod = GetKeyMod();
			if((aKeyMod & KEYMOD_SHIFT) || (anEvent->mLinkParam->mLaunchBrowser && !(aKeyMod & KEYMOD_ALT)))
				Browse(aPath);
			else
				Go(aPath);
		}
		return;

		case ComponentEvent_HyperLinkOver:
		{
			HyperLinkEvent *anEvent = (HyperLinkEvent*)theEvent;
			if(anEvent->mLinkParam.get()!=NULL)
			{
				const std::string &aLink = anEvent->mLinkParam->mLinkParam;
				string aPath; 
				GetLinkPath(aLink,aPath);
				SetStatus(aPath);
			}
			else
				SetStatus("");
		}
		return;
	}
}
