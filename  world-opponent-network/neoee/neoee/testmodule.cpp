#include "server.hpp"
#include <conio.h>
#define NEOEE_VERSION "0.0.0.3"
 
int main() {
	std::cout << "Neo-EE Titan Server v" << NEOEE_VERSION << " startup...\n";
	std::ostream os(std::cout.rdbuf(), std::ios_base::binary);
	net::listener listener(jodocus::system().num_cpu);
	try {
		server server(listener);
		server.run_async();
		std::cout << "\tINFO: Press [ESC] to shut down the server.\n";
		while(_getch() != 27);
	}
	catch(const net::net_error& error) {
		std::cerr << '\a' << error.what() << '\n';
	}
}