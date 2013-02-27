#include "../event.hpp"
#include <iostream>
namespace jodocus {

	event::event(bool status) 
		: event_(CreateEvent(0, TRUE, status, 0))
	{ 
		if(!event_)
			throw std::runtime_error("error creating an event object");
	}

	event::~event() { 
		::CloseHandle(event_); 
	}

	void event::set() { 
		::SetEvent(event_); 
	}

	void event::reset() { 
		::ResetEvent(event_);
	}

	bool event::wait(unsigned duration)  { 
		DWORD ret = ::WaitForSingleObject(event_, duration);		
		switch(ret) {
		case WAIT_TIMEOUT:
			return false;
		case WAIT_OBJECT_0:
			return true;
		default:
			throw std::runtime_error("error synchronising with event object");
		}
	}

	HANDLE event::handle() const {
		return event_;
	}

	std::size_t multiple_wait(std::vector<event>&& events, bool wait_all, unsigned duration) {
		// das funktioniert, da nach C++11-Standard jodocus::event ein POD sein muss
		DWORD ret = ::WaitForMultipleObjects(events.size(), reinterpret_cast<event::native_handle_type*>(&events[0]), wait_all == true ? TRUE : FALSE, duration);
		switch(ret) {
		case WAIT_TIMEOUT:
			return -1;
		case WAIT_ABANDONED_0:
		case WAIT_FAILED:
			throw std::runtime_error("error synchronising with multiple event objects");
		case WAIT_OBJECT_0:
		default:
			if(wait_all) return events.size();
			else return ret - WAIT_OBJECT_0;
		}
	}

}