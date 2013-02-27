#include "../jodocus.hpp"

jodocus::uncopyable::uncopyable()
{ }

jodocus::uncopyable::~uncopyable()
{ }

jodocus::system_information::system_information() {
	SYSTEM_INFO si = { 0 };
	GetSystemInfo(&si);
	num_cpu = si.dwNumberOfProcessors;
	page_size = si.dwPageSize;
	architecture = si.wProcessorArchitecture;
}

namespace jodocus {
	const system_information& system() {
		static system_information si;
		return si;
	}
}

std::string jodocus::internal::error_fmt(const std::string& message, unsigned code) {
	std::stringstream ss;
	ss << _T("A runtime exception occured.\nMessage: ") << message << _T("\nCode: ") << code;
	return ss.str();
}