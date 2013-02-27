#pragma once
#include "packet_hdr.hpp"

struct version_request : header {
	std::uint16_t submsg_type;
	std::vector<std::string> patch_info;
};

struct version_response : header {
	std::uint16_t submsg_type, status;
	std::string error_message;
	std::uint8_t sequence;
	std::vector<std::string> patch_info;
};

std::istream& operator >>(std::istream&, version_request&);
std::ostream& operator <<(std::ostream&, const version_response&);