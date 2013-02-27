#include "common.hpp"

#include <condition_variable>
#include <cstdint>
#include <map>
#include <mutex>
#include <sstream>
#include <thread>

class server {
	std::mutex mtx_, cmtx_;
	std::map<net::native_socket_type, tcp::socket> clnts_;
	std::condition_variable cnd_;
	bool ntfy_;
	tcp::acceptor dir_acpt_, ver_acpt_, fire_acpt_, db_acpt_;
	std::thread thrd_;

public:
	server(const net::listener&,
		   const tcp::address& = *tcp::localhost(L"15106"),
		   const tcp::address& = *tcp::localhost(L"11337"),
		   const tcp::address& = *tcp::localhost(L"14000"),
		   const tcp::address& = *tcp::localhost(L"17000"));
	~server();

	void run_async();
	void stop();
	bool running() const;
 
private:
	void handle_dir_(tcp::socket&, jodocus::buffer&);
	void handle_ver_(tcp::socket&, jodocus::buffer&);
	void handle_fw_(tcp::socket&, jodocus::buffer&);

	void handle_key_(tcp::socket&, jodocus::buffer&);

	void receive_(tcp::socket&);
	void worker_thread_();
 
};