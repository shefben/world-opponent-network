#include "../guid.hpp"

JODOCUS_NS_BEGIN

guid::guid() {
	std::memset(&guid_, 0, sizeof guid_);
}

guid::guid(GUID* guid) {
	std::memmove(&guid_, guid, sizeof guid);
}

std::wstring guid::to_string() const {
	const auto buffer_size = 39;
	wchar_t buffer[buffer_size] = { 0 };
	StringFromGUID2(guid_, buffer, buffer_size);
	return std::wstring(buffer);
}

const GUID* guid::handle() const {
	return &guid_;
}

bool guid::operator ==(const guid& g) const {
	for(std::size_t n = 0; n < sizeof g; ++n)
		if((reinterpret_cast<const byte*>(&g))[n] != (reinterpret_cast<const byte*>(&guid_))[n])
			return false;
	return true;
}

bool guid::operator !=(const guid& g) const {
	return !(*this == g);
}

guid generate_guid() {
	GUID g;
	::CoCreateGuid(&g);
	return guid(&g);
}

JODOCUS_NS_END