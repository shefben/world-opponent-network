#include "version.hpp"

std::istream& operator >>(std::istream& is, version_request& request) {
	is.ignore(4);
	is >> static_cast<header&>(request);
	common::read(is, request.submsg_type);
	std::size_t count = 0;
	while(true) {
		std::uint16_t length;
		std::string string;
		common::read_string(is, string, length);
		if(is.eof() || count++ == common::maximum_num_entities) break;
		/*if(is.fail())
			return is;*/
		request.patch_info.push_back(string);
	}
	return is;
}

std::ostream& operator <<(std::ostream& os, const version_response& response) {
	std::stringstream fmt(std::ios_base::in | std::ios_base::out | std::ios_base::binary);
	fmt << static_cast<const header&>(response);
	common::write(fmt, response.submsg_type, response.status);
	common::write_string(fmt, response.error_message);
	common::write(fmt, response.sequence);
	for(std::size_t n = 0; n < response.patch_info.size(); ++n)
		common::write_string(fmt, response.patch_info[n]);

	common::write<std::uint16_t>(fmt, 0); // very mysterious

	std::uint32_t length = static_cast<std::uint32_t>(fmt.str().length() + sizeof length);
	common::write(os, length);
	common::write_bytes(os, reinterpret_cast<const std::uint8_t*>(fmt.str().c_str()), length - sizeof length);
	return os;
}