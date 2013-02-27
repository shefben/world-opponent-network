#pragma once
#include "common.hpp"

struct header {
	std::uint8_t packet_type;
	std::uint16_t service_type, message_type;
};

std::istream& operator >>(std::istream&, header&);
std::ostream& operator <<(std::ostream&, const header&);