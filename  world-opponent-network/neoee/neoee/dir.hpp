#pragma once
#include "packet_hdr.hpp"

struct dir_request : header {
	std::uint16_t max_per_reply;
	std::uint8_t num_entities;

	struct entity_t {
		std::uint32_t flag;
		std::uint8_t type;
		std::wstring path;
		std::uint16_t num_types;
	};

	std::vector<entity_t> entities;
};

struct dir_response_header : header {
	std::uint16_t status;

	struct entity_t {
		std::uint8_t type;
		std::wstring path;
		std::wstring name;
		std::uint8_t address[common::addr_length];
		std::uint8_t unique; // bool
		std::wstring display_name;
		std::uint32_t lifespan, created, touched, crc;
	};

	// To Do: Add Data Objects and ACLs here
};

struct single_dir_response : dir_response_header {
	std::uint32_t flags;
	entity_t entity;
};

struct multi_dir_response : dir_response_header {
	std::uint8_t sequence;
	std::uint32_t flags;

	std::vector<entity_t> entities;	
};

std::istream& operator >>(std::istream&, dir_request&);
std::ostream& operator <<(std::ostream&, const dir_response_header&);
std::ostream& operator <<(std::ostream&, const multi_dir_response&);
std::ostream& operator <<(std::ostream&, const single_dir_response&);