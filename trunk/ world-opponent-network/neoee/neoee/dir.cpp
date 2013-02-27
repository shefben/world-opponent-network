#include "dir.hpp"

std::istream& operator >>(std::istream& is, dir_request& request) {
	is.ignore(4);
	is >> static_cast<header&>(request);
	common::read(is, request.max_per_reply, request.num_entities);
	request.entities.clear();

	if(request.num_entities <= common::maximum_num_entities)
		request.entities.resize(request.num_entities);
	else {
		is.setstate(std::ios_base::failbit);
		return is;
	}
	
	for(std::size_t n = 0; n < request.num_entities; ++n) {
		dir_request::entity_t entity;
		std::uint16_t length;
		common::read(is, entity.flag, entity.type);
		common::read_string(is, entity.path, length);
		common::read(is, entity.num_types);
		request.entities[n] = std::move(entity);
	}
	return is;
}

std::ostream& operator <<(std::ostream& os, const dir_response_header& response) {
	os << static_cast<const header&>(response);
	common::write(os, response.status);
	return os;
}

std::ostream& operator <<(std::ostream& os, const multi_dir_response& response) {
	std::stringstream fmt(std::ios_base::in | std::ios_base::out | std::ios_base::binary);

	fmt << static_cast<const dir_response_header&>(response);
	std::uint16_t num_entities = static_cast<std::uint16_t>(response.entities.size());
	common::write(fmt, response.sequence, response.flags, num_entities);

	for(std::size_t n = 0; n < num_entities; ++n) {
		if(response.flags & 0x00000010)
			common::write(fmt, response.entities[n].type);
		if(response.flags & (0x01000000 | 0x00010000))
			common::write_string(fmt, response.entities[n].path);
		if(response.flags & (0x02000000 | 0x00020000))
			common::write(fmt, response.entities[n].name);
		if(response.flags & 0x04000000) {
			std::uint8_t length = static_cast<std::uint8_t>(common::addr_length);
			common::write(fmt, length);
			common::write_bytes(fmt, response.entities[n].address, length);
		}
		if(response.flags & 0x00040000) {
			std::uint8_t unique = response.entities[n].unique;
			common::write(fmt, unique);
		}
		if(response.flags & 0x00000020)
			common::write_string(fmt, response.entities[n].display_name);
		if(response.flags & 0x00000100)
			common::write(fmt, response.entities[n].lifespan);
		if(response.flags &	0x00000040)
			common::write(fmt, response.entities[n].created);
		if(response.flags & 0x00000080)
			common::write(fmt, response.entities[n].touched);
		if(response.flags & 0x00002000)
			common::write(fmt, response.entities[n].crc);
	}

	std::uint32_t length = static_cast<std::uint32_t>(fmt.str().length() + sizeof length);
	common::write(os, length);
	common::write_bytes(os, reinterpret_cast<const std::uint8_t*>(fmt.str().c_str()), length - sizeof length);
	return os;
}

std::ostream& operator <<(std::ostream& os, const single_dir_response& response) {
	std::stringstream fmt(std::ios_base::in | std::ios_base::out | std::ios_base::binary);

	fmt << static_cast<const dir_response_header&>(response);
	common::write(fmt, response.flags);
	if(response.flags & 0x00000010)
		common::write(fmt, response.entity.type);
	if(response.flags & (0x01000000 | 0x00010000))
		common::write_string(fmt, response.entity.path);
	if(response.flags & (0x02000000 | 0x00020000))
		common::write(fmt, response.entity.name);
	if(response.flags & 0x04000000) {
		std::uint8_t length = static_cast<std::uint8_t>(common::addr_length);
		common::write(fmt, length);
		common::write_bytes(fmt, response.entity.address, length);
	}
	if(response.flags & 0x00040000) {
		std::uint8_t unique = response.entity.unique;
		common::write(fmt, unique);
	}
	if(response.flags & 0x00000020)
		common::write_string(fmt, response.entity.display_name);
	if(response.flags & 0x00000100)
		common::write(fmt, response.entity.lifespan);
	if(response.flags &	0x00000040)
		common::write(fmt, response.entity.created);
	if(response.flags & 0x00000080)
		common::write(fmt, response.entity.touched);
	if(response.flags & 0x00002000)
		common::write(fmt, response.entity.crc);

	std::uint32_t length = static_cast<std::uint32_t>(fmt.str().length() + sizeof length);
	common::write(os, length);
	common::write_bytes(os, reinterpret_cast<const std::uint8_t*>(fmt.str().c_str()), length - sizeof length);
	return os;
}