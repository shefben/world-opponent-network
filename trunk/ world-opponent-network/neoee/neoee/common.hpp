#pragma once
#include "net/net.hpp"

typedef jodocus::net::ip::tcp tcp;
namespace net = jodocus::net;

namespace common {
	void print_binary(const jodocus::buffer&);
	void dump_hex(const jodocus::buffer&);

	void print_information_(unsigned, tcp::socket&, const jodocus::buffer&);

	void write_bytes(std::ostream&, const std::uint8_t*, std::size_t);
	void read_bytes(std::istream&, std::uint8_t*, std::streamsize);
 
	enum {
		maximum_string_length = 256,
		maximum_num_entities = 20,
		addr_length = 14
	};


	template <typename T, typename ...Args>
	void write(std::ostream& os, T t, Args&&... args) {
		write(os, t);
		write(os, std::forward<Args>(args)...);
	}

	template <typename T>
	void write(std::ostream& os, T t) {
		os.write(reinterpret_cast<const char*>(&t), sizeof t);
	}

	template <typename CharType>
	void write_string(std::ostream& os, const std::basic_string<CharType>& str, std::size_t field = 2) {
		std::uint32_t length = static_cast<std::uint32_t>(str.length());
		switch(field) {
		case 1:
			write(os, static_cast<std::uint8_t>(length));
			break;
		case 2:
			write(os, static_cast<std::uint16_t>(length));
			break;
		case 4:
			write(os, static_cast<std::uint32_t>(length));
			break;
		}
		write_bytes(os, reinterpret_cast<const std::uint8_t*>(str.c_str()), str.length() * sizeof(CharType));
	}

	template <typename T, typename ...Args>
	void read(std::istream& is, T& t, Args&... args) {
		read(is, t);
		read(is, args...);
	}

	template <typename T>
	void read(std::istream& is, T& t) {
		is.read(reinterpret_cast<char*>(&t), sizeof t);
	}

	template <typename CharType, typename LengthType>
	void read_string(std::istream& is, std::basic_string<CharType>& string, LengthType& length) {
		read(is, length);
		string.clear();
		if(length > maximum_string_length) {
			is.setstate(std::ios_base::failbit);
			return;
		}
		std::vector<std::uint8_t> str_bytes(length * sizeof(CharType));
		read_bytes(is, &str_bytes[0], str_bytes.size());
		string.assign(reinterpret_cast<const CharType*>(&str_bytes[0]), str_bytes.size() / sizeof(CharType));
	}

}