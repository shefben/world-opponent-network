#ifndef __WON_LOBBY_H__
#define __WON_LOBBY_H__

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
//#include <ddraw.h>
#include <list>

//#include "WonCommon/CriticalSection.h"

//#ifdef _USRDLL // DLL
//#ifdef WONLOBBY_EXPORTS
//#define WONLOBBY_API __declspec(dllexport)
//#else
//#define WONLOBBY_API __declspec(dllimport)
//#endif
//#else // LIB
#define WONLOBBY_API
//#endif


namespace WONAPI
{

class Window;
class Component;
class WONResourceCollection;
	
// Types
// LaunchGameCallback();
// LobbyDoneCallback(LobbyResult);

// Interface to the WON Generic Lobby
class WONLOBBY_API WONLobby
{
public:
	// Enums 
	enum LobbyResult
	{
		LobbyResult_Success = 0,	// Lobby ran successfully 
		LobbyResult_StartGame = 1,	// User clicked start game
		LobbyResult_Failure = 100
	};

	enum ValueType
	{
		ValueType_Invalid	= 0,
		ValueType_Int		= 1,
		ValueTyep_String	= 2
	};

public:
	typedef void (*LobbyDoneCallback)(LobbyResult theLobbyResult, void* theUserData);
public:

	// struct that holds a user input that will be returned after lobby is done
	// Id of control in resource, type,
	class UserInput
	{
	public:
		UserInput() :
			ControlId(0),
			InputValueType(ValueType_Invalid),
			InputResultP(NULL)
		{}

		unsigned int	ControlId;
		ValueType		InputValueType;
		// Need value range like 1-32 etc.
		// **
		// Pointer to the memory location that is going to receive the data
		void*			InputResultP;
	};

	typedef std::list<UserInput> UserInputList;

	// Class used to define lobby configuration
	class LobbyDefinition
	{
	public:
		LobbyDefinition() :
		  	mLobbyName(),
			mUserInputList()
		{

		}
		~LobbyDefinition() {}

		void SetLobbyName(const std::string& theName)	{ mLobbyName = theName;	}
		//AddUserInputField(unsigned int theControlId, ValueType theInputValueType, void* theInputResultP);

		const std::string GetLobbyName() const { return mLobbyName; }
	private:

		// user input list
		std::string		mLobbyName;
		UserInputList	mUserInputList;
	};

	
// Public Methods
public:
	WONLobby();
	virtual ~WONLobby();

	void LobbyWindowClosed();

// Public Interface
public:

	virtual LobbyResult Init(const LobbyDefinition& theLobbyDefinitionR /*LobbyDoneCallback& theLobbyDoneCallback*/);

	virtual LobbyResult LaunchLobby(const RECT& theWindowRectR); // Blocking call
	virtual LobbyResult LaunchLobby(const RECT& theWindowRectR, LobbyDoneCallback theLobbyDoneCallback, void* theUserData = NULL);  // Non blocking call

	// Not sure this is needed since Launch is blocking
	LobbyResult Shutdown();

protected:
	virtual void DefineLobby() {};
	void SetResizeInfo(Component *theComponent, int dx, int dy, int dw, int dh);

private:

public:
	static void LobbyWindowClosedStatic(void* theData);

// Member data
protected:
	Window*					mWindowP;
	WONResourceCollection*	mResourceCollectionP;
	std::string				mLobbyName;

	LobbyDoneCallback		mLobbyDoneCallback;
	void*					mLobbyDoneCallbackData;

	//CriticalSection mCrit;
};

}

#endif // __WON_LOBBY_H__

