#ifndef __WON_SPINNER_H__
#define __WON_SPINNER_H__

#include "Button.h"
#include "Container.h"
#include "InputBox.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class Spinner : public Container
{
protected:
	InputBoxPtr mInputBox; 
	ButtonPtr mUpArrow; 
	ButtonPtr mDownArrow; 
	int mMaxValue;   
	int mMinValue;   
	int	mValue;
	bool mValueIsValid;
	bool mWrapAround;

public:
	virtual void AddedToParent();
	virtual bool KeyDown(int theKey);
	virtual void GetDesiredSize(int &width, int &height);
	virtual void EnableHook(bool isEnabled);
	virtual void HandleComponentEvent(ComponentEvent* pEvent);

	void ForceValueCompliance();
	void SetTextToValue();
	void CalcValueFromText();
	void AddControls();

	void CheckValueChanged(bool oldValid, int oldValue);

public:
	Spinner();

	void IncrementValue(int theAmount = 1);
	void DecrementValue(int theAmount = 1);
	void SetValue(int theValue, bool sendEvent = true);

	void SetMax(int theMax);
	void SetMin(int theMin);
	void SetRange(int theMin, int theMax, bool sendEvent = true);
	void SetNoType(bool noType);
	void SetWrapAround(bool wrap);

	int GetMax(); 
	int GetMin(); 
	int GetValue();
	bool IsValueValid();
};

} // namespace WONAPI

#endif