#ifndef __WON_COMPONENTLISTENER_H__
#define __WON_COMPONENTLISTENER_H__

#include "WONCommon/SmartPtr.h"

namespace WONAPI
{

class Component;
class ComponentEvent;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ComponentListener : public WONAPI::RefCount
{
public:
	virtual void HandleComponentEvent(ComponentEvent *theEvent) = 0;
};
typedef WONAPI::SmartPtr<ComponentListener> ComponentListenerPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ComponentListenerFunc : public ComponentListener
{
protected:
	typedef void(*Callback)(ComponentEvent *theEvent);
	Callback mCallback;

public:
	ComponentListenerFunc(Callback theCallback) : mCallback(theCallback) {}
	virtual void HandleComponentEvent(ComponentEvent *theEvent) { mCallback(theEvent); }

};
typedef WONAPI::SmartPtr<ComponentListenerFunc> ComponentListenerFuncPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
template<class ParamType>
class ComponentListenerFuncParam : public ComponentListener
{
	typedef void(*Callback)(ComponentEvent *theEvent, ParamType theParam);
	Callback mCallback;
	ParamType mParam;

public:
	ComponentListenerFuncParam(Callback theCallback, ParamType theParam) : mCallback(theCallback), mParam(theParam) {}
	virtual void HandleComponentEvent(ComponentEvent *theEvent) { mCallback(theEvent,mParam); }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ComponentListenerComp : public ComponentListener
{
protected:
	Component *mListener;

public:
	ComponentListenerComp(Component *theListener) : mListener(theListener) { }
	virtual void HandleComponentEvent(ComponentEvent *theEvent);
};
typedef WONAPI::SmartPtr<ComponentListenerComp> ComponentListenerCompPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ComponentListenerFuncWithValue : public ComponentListener
{
protected:
	typedef void(*Callback)(ComponentEvent *theEvent, void* theValue);
	Callback	mCallback;
	void*		mValue;

public:
	ComponentListenerFuncWithValue(Callback theCallback, void* theValue) : mCallback(theCallback), mValue(theValue) {}
	virtual void HandleComponentEvent(ComponentEvent *theEvent) { mCallback(theEvent, mValue); }

};
typedef WONAPI::SmartPtr<ComponentListenerFuncWithValue> ComponentListenerFuncWithValuePtr;

}; // namespace WONAPI

#endif