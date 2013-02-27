#include "common.hpp"
#include <iostream>
#include <iomanip>

void common::print_binary(const jodocus::buffer& buffer) {
	for(std::size_t n = 0; n != buffer.size(); ++n)
		std::cout << static_cast<const char*>(buffer.storage())[n];
}
 
void common::dump_hex(const jodocus::buffer& buffer) {
	for(std::size_t n = 0; n != buffer.size(); ++n) {
		if(!(n % 23)) std::cout << std::dec << std::setw(4) << std::endl << std::setw(4) << std::ceil(n / 23) + 1 << ": ";
			unsigned char c = static_cast<const unsigned char*>(buffer.storage())[n];
			std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<std::uint16_t>(c) << " ";
	}
	std::cout << std::endl;
}

void common::print_information_(unsigned transferred, tcp::socket& socket, const jodocus::buffer& buffer) {
	std::wcout << L"\n==========================================================================\n"
			   << L"New connection opened on Server: " << socket.descriptor() << ".\n"
			   << L"Peer address: " << socket.peer() << "\n"
			   << L"Message length: " << transferred << " Bytes\n"
			   << L"Message content: ";
	common::print_binary(buffer);
	std::wcout << L"\nHex representation:\n";
	common::dump_hex(buffer);
	std::wcout << L"==========================================================================\n";	
}

void common::write_bytes(std::ostream& os, const std::uint8_t* bytes, std::size_t length) {
	os.write(reinterpret_cast<const char*>(bytes), length);
}

void common::read_bytes(std::istream& is, std::uint8_t* storage, std::streamsize length) {
	is.read(reinterpret_cast<char*>(storage), length);
}