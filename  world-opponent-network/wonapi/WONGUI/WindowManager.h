#ifndef __WON_WINDOWMANAGER_H__
#define __WON_WINDOWMANAGER_H__

#include "WONCommon/CriticalSection.h"
#include "WONCommon/StringUtil.h"
#include "GUITypes.h"
#include "ColorScheme.h"
#include "ImageDecoder.h"
#include "SoundDecoder.h"
#include "WindowEvent.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class WindowManager : public RefCount
{
protected:
	CriticalSection mDataCrit;

	static DWORD mThreadId;
	ColorSchemePtr mColorScheme;
	MultiImageDecoder mImageDecoder;
	SoundDecoderPtr mSoundDecoder;
	typedef std::map<std::string,FontPtr,StringLessNoCase> NamedFontMap;
	FontPtr mDefaultFont;
	NativeIconPtr mDefaultWindowIcon;
	NamedFontMap mNamedFontMap;

	// Image Caching
	typedef std::map<std::string,NativeImagePtr,StringLessNoCase> NativeImageMap;
	typedef std::map<std::string,RawImagePtr,StringLessNoCase> RawImageMap;
	typedef std::map<std::string,ImagePtr,StringLessNoCase> DelayImageMap;

	NativeImageMap mNativeImageMap;
	RawImageMap mRawImageMap;
	DelayImageMap mDelayImageMap;
	int mCacheImageCount;

	DWORD mLastTimerTick;
	DWORD mLastUserInputTick;

	bool mEndPopup;
	bool mIsValid;
	bool mInDoPopup;
	bool mInEndPopup;
	bool mPopupIsUp;
	bool mPopupInOwnWindowDef;
	int mPopupFlags;
	int mPauseEvents;

	bool mDoMouseCapturePopupSupport;

	bool mIsFullScreen;
	int mFullScreenWidth, mFullScreenHeight;
	int mOriginalScreenWidth, mOriginalScreenHeight;
	DWORD mLastLoseFocusTick;
	void NotifyLostFocus();
	void NotifyGotFocus(Window *theWindows);

	int mCurMsgLoopDepth;
	int mTargetMsgLoopDepth;
	int mDialogResult;
	Window* mModalWindow;
	Component* mModalComponent;
	WindowPtr mPopupWindowSave;
	Window* mPopupOwnerWindow;
	ComponentPtr mPopupInputComponent;
	ComponentPtr mPopupOwnerComponent;
	typedef std::list<Component*> PopupList;
	PopupList mPopupList;

	Window* mFocusWindow;
	Window* mMouseDownWindow;

	Window* mDefaultWindow;
	DWORD mWindowCount;
	typedef std::set<WindowPtr,WindowPtr::Comp> WindowSet;
	WindowSet mWindowSet;

	typedef std::list<WindowEventBasePtr> WindowEventList;
	WindowEventList mWindowEventList;

	typedef std::set<WindowEventBasePtr> WindowEventSet;
	WindowEventSet mTimerEvents;
	WindowEventSet mRemoveTimerEvents;
	bool mCallingTimers;

	WindowEventBasePtr mCloseEvent;

	typedef std::set<ComponentPtr> BroadcastSet;
	BroadcastSet mBroadcastSet;

	static WindowManager* mDefaultWindowManager;

	virtual void GetNextEvent();
	WindowEventBase* PeekNextEvent();
	void DeliverEvent(WindowEventBase *theEvent);
	void AdjustPopupPosition(Window *theParent, Component *theComponent, bool inOwnWindow, const WONRectangle &theAvoidRect);

	virtual ~WindowManager();

	void CalcModality();

	void FindMouseWindow(MouseEvent *theEvent);
	bool CheckPopupClick(MouseEvent *theEvent);
	bool CheckModalValid();

	void InitColorScheme();

	virtual bool SetScreenResolutionHook(int theWidth, int theHeight);
	virtual void GetScreenResolutionHook(int &theWidth, int &theHeight);
	virtual void AddImageDecoders();

public:
	WindowManager();

	int MessageLoop();
	void PumpOnce();

	virtual void Signal() { }
	void PostEvent(WindowEventBase *theEvent, bool needSignal = true);
	void AddTimerEvent(WindowEventBase *theEvent);
	void RemoveTimerEvent(WindowEventBase *theEvent);
	void RemoveAllTimerEvents();
	void GetScreenResolution(int &theWidth, int &theHeight);
	void SetScreenResolution(int theWidth, int theHeight);
	void RestoreScreenResolution();


	Window* GetPopupOwnerWindow() { return mPopupOwnerWindow; }
	Window* GetModalWindow() { return mModalWindow; }
	Window* GetFocusWindow() { return mFocusWindow; }
	Component* GetModalComponent() { return mModalComponent; }
	bool NotModalWindow(Window *theWindow);
//	int DoDialog(const DialogParams &theParams);
	int DoDialog(Window *theWindow, Component *theComponent = NULL);
	void EndDialog(int theResult = -1, Window *theWindow = NULL, Component *theDialog = NULL);
	void ExitMessageLoop();

	void DoPopup(const PopupParams &theParams);
	void EndPopup(Component *thePopup = NULL);

	void ProcessEvents();

	static WindowManager* GetDefaultWindowManager() { return mDefaultWindowManager; }
	static FontPtr GetDefaultFont(const FontDescriptor &theDescriptor);
	static DisplayContext* GetDefaultDisplayContext();
	Window* GetDefaultWindow() { return mDefaultWindow; }

	virtual bool AddWindow(Window *theWindow);
	virtual bool RemoveWindow(Window *theWindow);
	DWORD GetWindowCount() const { return mWindowCount; }

	virtual void GetScreenRect(WONRectangle & /*theRect*/) { }
	bool IsValid() { return mIsValid; }

	void CloseAllWindows();

	ColorScheme* GetColorScheme() { return mColorScheme; }
	MultiImageDecoder* GetImageDecoder();
	NativeImagePtr DecodeImage(DisplayContext *theContext, const char *theFilePath);
	NativeImagePtr DecodeImage(const char *theFilePath);
	ImagePtr DecodeDelayImage(const char *theFilePath);
	RawImagePtr DecodeImageRaw(const char *theFilePath);
	void SetCacheImages(bool cache);

	void SetSoundDecoder(SoundDecoder *theDecoder);
	SoundPtr DecodeSound(const SoundDescriptor &theDesc);

	void InitSystemColorScheme();
	virtual void GetSystemColorScheme(ColorScheme * /*copyTo*/) { }

	Font* GetNamedFont(const std::string &theName);
	Font* GetDefaultFont();
	Font* GetDefaultFontMod(int theStyle, int theSize = 0);
	Font* GetNamedFontMod(const std::string &theName, int theStyle, int theSize = 0);
	NativeIcon* GetDefaultWindowIcon() { return mDefaultWindowIcon; }

	void SetDefaultFont(Font *theFont) { mDefaultFont = theFont; }
	void SetDefaultWindowIcon(NativeIcon *theIcon) { mDefaultWindowIcon = theIcon; }
	void SetNamedFont(const std::string &theName, Font *theFont);

	void SubscribeToBroadcast(Component *theComponent, bool subscribe);
	void BroadcastEvent(ComponentEvent *theEvent);
	void ClearSubscriptions();

	Window* GetPopupWindow();
	bool PopupFlagSet(int theFlag) { return mPopupFlags&theFlag?true:false; }
	void SetPopupInOwnWindowDef(bool inOwnWindow) { mPopupInOwnWindowDef = inOwnWindow; }
	bool GetPopupInOwnWindowDef() { return mPopupInOwnWindowDef; } 
	static DWORD GetThreadId() { return mThreadId; }
	static void SetThreadId(DWORD theId) { mThreadId = theId; }
	void SetIsFullScreen(bool isFullScreen, int theWidth = 0, int theHeight = 0);
	bool GetIsFullScreen() { return mIsFullScreen; }

	DWORD GetLastUserInputTick() { return mLastUserInputTick; }

	void CheckEndPopup(Component *theAncestor); // check if the Ancestor is an ancestor of the PopupOwner

	void PauseEvents();
	void UnPauseEvents();
};

class AutoWMPauseEvents
{
protected:
	int mPauseCount;

public:
	AutoWMPauseEvents();
	~AutoWMPauseEvents();

	void Pause();
	void UnPause();
};

typedef SmartPtr<WindowManager> WindowManagerPtr;

};

#endif