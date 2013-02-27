#pragma once
#include "jodocus.hpp"

namespace jodocus {

	/**
	*	The event class provides basic signal facilities for multithreading applications. A signal
	*	can be at two states: either signaled or not signaled. With this functionality, threads have the
	*	possibility to communicate and to synchronise their work.
	*/
	class event {
	public:
		typedef HANDLE native_handle_type;
		/**
		*	The constructor creates an event instance. The parameter specifies its initial status. true indicates that
		*	the event object shall be signaled, false means not singaled. If the creation fails, the constructor throws
		*	and std::runtime_error.
		*/
		event(bool = false);

		/**
		*	The destructor destroys an event object.
		*/
		~event();

		/**
		*	The set()-function sets the event to singaled thread-safely. If the thread is already signaled, this
		*	function won't have any effect. It guarantees not to throw any exceptions.
		*/
		void set() throw();

		/**
		*	The reset()-function sets the event to <i>not</i> singaled thread-safely. If the thread is already not signaled, this
		*	function won't have any effect. It guarantees not to throw any exceptions.
		*/
		void reset() throw();

		/**
		*	The wait()-function blocks the calling thread for a certain time specified by the parameter in <i>milliseconds</i> as long as
		*	the event object is <b>not signaled</b> and returns <i>false</i> afterwards when the time is over. However, if the event object
		*	is set to signaled by an other thread the calling thread which is blocked by a call to wait() and not finished yet will return true.
		*	The default parameter indicates that the calling thread will be blocked for an indefinitely amount of time; thus the thread can only
		*	continue when the event object is set to signaled by another thread. If this function fails it will throw an std::runtime_error.
		*/
		bool wait(unsigned = INFINITE) ;

		/**
		*	The handle()-function returns an API event handle of the object. It guarantees not to throw any exceptions.
		*/
		native_handle_type handle() const throw();

	private:
		native_handle_type event_;
	};

	std::size_t multiple_wait(std::vector<event>&&, bool = false, unsigned = INFINITE);

}