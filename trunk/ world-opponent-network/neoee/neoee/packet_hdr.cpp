#include "packet_hdr.hpp"

std::istream& operator >>(std::istream& is, header& hdr) {
	common::read(is, hdr.packet_type, hdr.service_type, hdr.message_type);
	return is;
}

std::ostream& operator <<(std::ostream& os, const header& hdr) {
	common::write(os, hdr.packet_type, hdr.service_type, hdr.message_type);
	return os;
}