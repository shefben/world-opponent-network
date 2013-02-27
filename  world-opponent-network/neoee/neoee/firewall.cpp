#include "firewall.hpp"

std::istream& operator >>(std::istream& is, firewall_request& request) {
	is.ignore(2);
	is >> static_cast<header&>(request);
	common::read(is, request.port);
	if(request.header::message_type == 1)
		common::read(is, request.wait_connect, request.max_wait_time, request.send_reply);
	return is;
}

std::ostream& operator <<(std::ostream& os, const firewall_response& response) {
	std::stringstream fmt(std::ios_base::in | std::ios_base::out | std::ios_base::binary);

	fmt << static_cast<const header&>(response);
	common::write(fmt, response.status);
	common::write_bytes(fmt, reinterpret_cast<const std::uint8_t*>(response.ipv4), sizeof response.ipv4);

	std::uint16_t length = static_cast<std::uint16_t>(fmt.str().length() + sizeof length);
	common::write(os, length);
	common::write_bytes(os, reinterpret_cast<const std::uint8_t*>(fmt.str().c_str()), length - sizeof length);
	return os;
}