
#ifndef __GETPATCHSERVERMODULELISTOP_H__
#define __GETPATCHSERVERMODULELISTOP_H__


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "WONDB/DBProxyOp.h"					// Base Class


namespace WONAPI 
{


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// 
// GetPatchServerModuleListOp - GetPatchServerModuleListOp
// Request a list of available modules
// Reply:
//		unsigned short - Num Modules
//		{
//			unsigned long moduleID; 
//		}
//
///////////////////////////////////////////////////////////////////////////////
class GetPatchServerModuleListOp: public DBProxyOp
{
	// Attributes
protected:
	std::list<unsigned long>  mModuleList;	//HIWORD(aMsgType) LOWORD(aMsgSubType)	


	const unsigned short mMsgType;				// DO NOT MODIFY (MUST MATCH SERVER)

	// Operations
public:
	bool IsModuleAvailable(unsigned short theMsgType, unsigned short theSubMsgType);

protected:
	void Init();

	// Overrides
public:
	virtual WONStatus CheckResponse();			// Recv - For Extended Unpack
	virtual void	  RunHook();				// Send - For Extended Pack

	// Constructor
public:
	GetPatchServerModuleListOp(ServerContext* theContext);	// Requires gamename and list of patch servers
	GetPatchServerModuleListOp(const IPAddr &theAddr);		// or addr of a single server

	// Destructor
protected:
	virtual ~GetPatchServerModuleListOp() {}			// Safety Lock -- Enforces Smart Ptr Use

};
typedef SmartPtr<GetPatchServerModuleListOp> GetPatchServerModuleListOpPtr;		// Smart Ptr Template


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
} // namespace WONAPI


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __GETPATCHSERVERMODULELISTOP_H__
