#pragma once
#include "jodocus.hpp"
#include <objbase.h>

JODOCUS_NS_BEGIN

	class guid {
	public:
		guid();
		explicit guid(GUID*);
		std::wstring to_string() const;
		const GUID* handle() const;

		bool operator ==(const guid&) const;
		bool operator !=(const guid&) const;

	private:
		GUID guid_;
	};

	guid generate_guid();

JODOCUS_NS_END