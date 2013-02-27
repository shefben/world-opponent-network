#pragma once
#include "packet_hdr.hpp"

struct firewall_request : header {
	std::uint16_t port;
	std::uint8_t wait_connect, send_reply; // bools
	std::uint32_t max_wait_time;
};

struct firewall_response : header {
	std::uint16_t status;
	char ipv4[4];
};

std::istream& operator >>(std::istream&, firewall_request&);
std::ostream& operator <<(std::ostream&, const firewall_response&);