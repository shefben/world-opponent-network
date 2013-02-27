#include "../net.hpp"

namespace jodocus {
	namespace net {

		network::network() { 
			WSADATA data = { 0 };
			if(::WSAStartup(MAKEWORD(2, 2), &data)) 
				throw net_error("winsock startup error");
		}

		network::~network() {
			::WSACleanup(); // if it fails - who cares?
		}

		native_socket_type internal::invalid_socket() {
			return INVALID_SOCKET;
		}

		void internal::delete_sockets(std::deque<native_socket_type>& sockets) {
			while(!sockets.empty()) {
				api_deallocate_socket(sockets.back()); // may throw an exception --> abort, but so what?
				sockets.pop_back();
			}
		}

		void internal::startup() {
			static const network netw; // initialise winsock
		}

		void internal::api_deallocate_socket(native_socket_type socket) {
			if(socket != internal::invalid_socket() && ::closesocket(socket) == SOCKET_ERROR)
				throw net_error("socket closure failure");
		}

		listener::listener(unsigned threads)
			: completion_port_(INVALID_HANDLE_VALUE), threads_(threads)
		{
			internal::startup();
			if(!(completion_port_ = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0)))
				throw net_error("completion port creation error");
			for(unsigned i = 0u; i < threads; ++i)
				threads_[i] = new std::thread(listener::dispatcher(*this)); 
		}

		listener::~listener() {
			for(std::size_t n = 0; n < threads_.size(); ++n) {
				if(::PostQueuedCompletionStatus(completion_port_, 0, 0, 0) == FALSE)
					throw net_error("error enqueueing shutdown operation", GetLastError());
			}
			for(std::size_t n = 0; n < threads_.size(); ++n) {
				if(threads_[n]->joinable())
					threads_[n]->join();
				delete threads_[n];
			}
			::CloseHandle(completion_port_);
		}

		std::size_t listener::count() const {
			return threads_.size();
		}

		// Listener::Dispatcher

		listener::dispatcher::dispatcher(const listener& li)
			: listener_(li) { }

		void listener::dispatcher::operator ()() {
			DWORD bytes_transferred;
			ULONG_PTR key;
			WSAOVERLAPPED* ol;
			internal::operation* op_ptr;
			bool shutdown = false;
			for(;;) {
				bytes_transferred = 0;
				key = 0;
				ol = nullptr;
				op_ptr = nullptr;
				unsigned error = 0;
				// wait for network events
				if(::GetQueuedCompletionStatus(listener_.completion_port_, &bytes_transferred, reinterpret_cast<ULONG_PTR*>(&key), &ol, INFINITE) == FALSE)
					error = GetLastError();
				if(ol == 0) // listener is shutting down
					return;

				op_ptr = CONTAINING_RECORD(ol, internal::operation, overlapped); // get the pointer from the given

				switch(op_ptr->type) {
				case internal::io_op: // dispatched if WSARecv(), WSASend(), WSASendTo(), WSARecvFrom(), ConnectEx(), DisconnectEx() ... completed
					{
						std::unique_ptr<internal::io_operation> ptr(static_cast<internal::io_operation*>(op_ptr));

						if(error == ERROR_OPERATION_ABORTED) // operation aborted, no need to call handler
							break; // all resources gets destroyed

						ptr->handler(error, bytes_transferred); // call user function
						break;
					}
				case internal::sock_op: // AcceptEx() completed
					{	
						// gather information
						std::unique_ptr<internal::connection_operation> ptr(static_cast<internal::connection_operation*>(op_ptr));
						std::unique_ptr<ip::tcp::socket> socket_ptr(ptr->acceptor.sockets_[ptr->index].first);

						if(error == ERROR_OPERATION_ABORTED) // accept operation aborted, dont dispatch and destroy ressources
							break;

						++ptr->acceptor.burst_count_; // increase the burst size
						if(ptr->acceptor.max_measured_burst_ < ptr->acceptor.burst_count_) // check for maximum measured burst
							ptr->acceptor.max_measured_burst_ = ptr->acceptor.burst_count_;	

						if(!error) {
							auto tmp = socket_ptr->descriptor();
							// inherit all socket options to the socket associated with the new connection
							if(setsockopt(tmp, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, reinterpret_cast<char*>(&tmp), sizeof tmp) == SOCKET_ERROR) {
								if(WSAGetLastError() == WSAENOTCONN) {
									error = WSAENOTCONN;
									break;
								}
								throw net_error("setsockopt(SO_UPDATE_ACCEPT_CONTEXT) call failure");
							}
						}	
						
						// call user function
						ptr->handler(error, bytes_transferred, std::move(*socket_ptr), jodocus::buffer(ptr->buffer, bytes_transferred)); // synchronous call to the user-handler
						--ptr->acceptor.burst_count_; // decrease burst, accept-process is finished

						if(ptr->acceptor.running_)
							if(ptr->index == ptr->acceptor.pending_accepts_ - 1 && ptr->acceptor.max_measured_burst_ < ptr->acceptor.pending_accepts_) {
								--ptr->acceptor.pending_accepts_; // if there are more pending AcceptEx() calls then needed, decrease the needed size
							}
							else
								ptr->acceptor.do_accept(ptr->handler, ptr->index); // else post another AcceptEx() to compensate

						break;
					}
				}
			}
		}

		internal::socket_interface<tcp<internal::ip>>::socket_interface() {
		}

		void internal::socket_interface<tcp<internal::ip>>::connect(const basic_address<tcp<internal::ip>>& host, std::function<void (unsigned, unsigned)> handler, const buffer& data) const
		{
			internal::startup();

			// Bind socket to local interface explicitely
			tcp<internal::ip>::address_type local_addr = { 0 };
			reinterpret_cast<SOCKADDR_STORAGE*>(&local_addr)->ss_family = tcp<internal::ip>::address_family;
			INETADDR_SETANY(reinterpret_cast<sockaddr*>(&local_addr));
			if(::bind(socket_, reinterpret_cast<sockaddr*>(&local_addr), static_cast<int>(INET_SOCKADDR_LENGTH(tcp<internal::ip>::address_family))) == SOCKET_ERROR)
				if(WSAGetLastError() != WSAEINVAL) // WSAEINVAL - socket already bound, so that's actually okay :)
					throw net_error("bind call error");

			static struct connectex_loader {
				connectex_loader(native_socket_type socket)
					: connectex_(0)
				{
					GUID connect_id = WSAID_CONNECTEX;
					DWORD bytes = 0;
					if(::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &connect_id, sizeof connect_id,
								  &connectex_, sizeof connectex_, &bytes, 0, 0) == SOCKET_ERROR)
						throw net_error("acceptex extension loading error");
				}

				LPFN_CONNECTEX connectex_;
			} loader(socket_);

			SOCKADDR_STORAGE remote_addr = { 0 };
			if(host.api_address()->ss_family == AF_INET) {
				sockaddr_in6 addrv6 = { 0 };
				SCOPE_ID scope = INETADDR_SCOPE_ID(reinterpret_cast<const SOCKADDR*>(host.api_address()));
				IN6ADDR_SETV4MAPPED(&addrv6, reinterpret_cast<const IN_ADDR*>(INETADDR_ADDRESS(reinterpret_cast<const SOCKADDR*>(host.api_address()))), scope, INETADDR_PORT(reinterpret_cast<const SOCKADDR*>(host.api_address())));
				std::memcpy(reinterpret_cast<SOCKADDR*>(&remote_addr), &addrv6, sizeof addrv6);
			}
			else
				std::memcpy(&remote_addr, host.api_address(), INET_SOCKADDR_LENGTH(AF_INET6));
			internal::io_operation* co = new internal::io_operation(handler);
			if(loader.connectex_(socket_, 
								 reinterpret_cast<SOCKADDR*>(&remote_addr),
								 sizeof remote_addr,
								 const_cast<void*>(data.storage()), // in case of standard parameter this is 0
								 static_cast<DWORD>(data.size() * sizeof(TCHAR)),       // equivalent
								 0,
								 &co->overlapped) == FALSE)
				if(WSAGetLastError() != WSA_IO_PENDING)
					handler(WSAGetLastError(), 0);
		}

		void internal::socket_interface<tcp<internal::ip>>::send(const buffer& single_buffer, std::function<void (unsigned, unsigned)> handler) const
		{
			std::vector<const buffer> v(1, single_buffer);
			send(v, handler);
		}


		bool internal::socket_interface<tcp<internal::ip>>::send_file(const jodocus::string& path, std::function<void (unsigned, unsigned)> handler) const
		{
			internal::startup();
			static struct transmitfile_loader {
				transmitfile_loader(native_socket_type socket)
					: transmit_file_(0)
				{
					GUID transmit_file_id = WSAID_TRANSMITFILE;
					DWORD bytes = 0;
					if(::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &transmit_file_id, sizeof transmit_file_id,
								  &transmit_file_, sizeof transmit_file_, &bytes, 0, 0) == SOCKET_ERROR)
						throw net_error("acceptex extension loading error");
				}

				LPFN_TRANSMITFILE transmit_file_;
			} loader(socket_);

			HANDLE file = INVALID_HANDLE_VALUE;
			if((file = ::CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0)) == INVALID_HANDLE_VALUE) {
				if(GetLastError() == ERROR_FILE_NOT_FOUND)
					return false;
				else
					throw net_error("file opening failure");
			}

			auto operation = new internal::io_operation(handler);
			if(loader.transmit_file_(socket_,
									 file,
									 0,
									 0,
									 &operation->overlapped,
									 0,
									 TF_USE_DEFAULT_WORKER) == FALSE)
			{
				switch(WSAGetLastError()) {
				case ERROR_IO_PENDING:
					break;
				case WSAEDISCON:
				case WSAECONNABORTED:
				case WSAECONNRESET:
				case WSAENETRESET:
					{
						std::auto_ptr<internal::io_operation> ptr(operation); // exception safety due to handler-call
						handler(WSAGetLastError(), 0);
						CloseHandle(file);
						return false;
					}
				default:
					::CloseHandle(file);
					throw net_error("sending file failure");
				}
			}
			CloseHandle(file);
			return true;
		}

		void internal::socket_interface<tcp<internal::ip>>::receive(const buffer& single_buffer, std::function<void (unsigned, unsigned)> handler) const
		{
			receive(internal::pseudo_container<const buffer>(single_buffer), handler);
		}

		void internal::socket_interface<tcp<internal::ip>>::disconnect(std::function<void (unsigned, unsigned)> handler) const {
			internal::startup();
			static struct disconnectex_loader {
				disconnectex_loader(native_socket_type socket)
					: disconnectex_(0)
				{
					GUID disconnect_id = WSAID_DISCONNECTEX;
					DWORD bytes = 0;
					if(::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &disconnect_id, sizeof disconnect_id,
								  &disconnectex_, sizeof disconnectex_, &bytes, 0, 0) == SOCKET_ERROR)
						throw net_error("disconnectex extension loading error");
				}

				LPFN_DISCONNECTEX disconnectex_;
			} loader(socket_);

			auto operation = new internal::io_operation(handler);
			if(loader.disconnectex_(socket_, &operation->overlapped, TF_REUSE_SOCKET, 0) == FALSE)
				if(WSAGetLastError() != WSA_IO_PENDING)
					handler(WSAGetLastError(), 0);
		}

		basic_address<tcp<internal::ip>> internal::socket_interface<tcp<internal::ip>>::peer() const {
			SOCKADDR_STORAGE saddr = { 0 };
			int length = sizeof saddr;
			if(::getpeername(socket_, reinterpret_cast<SOCKADDR*>(&saddr), &length) == SOCKET_ERROR)
				throw net_error("could not determine remote peer");
			return basic_address<tcp<internal::ip>>(reinterpret_cast<SOCKADDR*>(&saddr), sizeof saddr);
		}

		int internal::socket_interface<tcp<internal::ip>>::connect_time() const {
			int connect_time = 0, len = sizeof connect_time;
			if(::getsockopt(socket_, SOL_SOCKET, SO_CONNECT_TIME, reinterpret_cast<char*>(&connect_time), &len) == SOCKET_ERROR)
				throw net_error("getsockopt(SO_CONNECT_TIME) call failure");
			return connect_time;
		}

		bool internal::socket_interface<tcp<internal::ip>>::is_connected() const {
			return connect_time() == -1 ? false : true;
		}

		/* ************* UDP/IP ******************** */

		internal::socket_interface<udp<internal::ip>>::socket_interface() 
		{ }

		void internal::socket_interface<udp<internal::ip>>::send(const buffer& single_buffer, const basic_address<udp<internal::ip>>& address, std::function<void (unsigned, unsigned)> handler) const
		{
			std::vector<const buffer> v(1, single_buffer);
			send(v, address, handler);
		}


		void internal::socket_interface<udp<internal::ip>>::receive(const buffer& single_buffer, basic_address<udp<ip>>& address, std::function<void (unsigned, unsigned)> handler) const
		{
			receive(internal::pseudo_container<const buffer>(single_buffer), address, handler);
		}

		ip::tcp::acceptor::acceptor(const jodocus::net::listener& listener, const jodocus::net::ip::tcp::address& local, std::size_t max_burst)
			: socket_(listener, local), sockets_(max_burst), handler_(), running_(true), watch_thread_(),
			  burst_count_(), pending_accepts_(), max_measured_burst_(), mutex_()
		{
			pending_accepts_ = 1;
			max_measured_burst_ = 0;
			burst_count_ = 0;
			if(max_burst == 0)
				throw net_error("maximum burst can't be zero");
			if(::WSAEventSelect(socket_.descriptor(), ev_.handle(), FD_ACCEPT) == SOCKET_ERROR)
				throw net_error("accept event association error");
			if(::listen(socket_.descriptor(), SOMAXCONN) == SOCKET_ERROR)
				throw net_error("port listening error");	
			watch_thread_.swap(std::thread(std::bind(&acceptor::watchdog_, this)));
		}

		ip::tcp::acceptor::~acceptor() {
			stop();
			ev_.set();
			watch_thread_.join();
		}

		void ip::tcp::acceptor::start(std::function<void (unsigned, unsigned, socket::ref, jodocus::buffer)> handler) {
			running_ = true;
			handler_ = handler;
			do_accept(handler_, 0);
		}

		void ip::tcp::acceptor::do_accept(std::function<void (unsigned, unsigned, socket::ref, jodocus::buffer)> handler, std::size_t index) {
			static struct acceptex_loader {
				acceptex_loader(native_socket_type socket)
					: acceptex_(0)
				{
					GUID accept_id = WSAID_ACCEPTEX;
					DWORD bytes = 0;
					if(::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &accept_id, sizeof accept_id,
								  &acceptex_, sizeof acceptex_, &bytes, 0, 0) == SOCKET_ERROR)
						throw net_error("acceptex extension loading error");
				}

				LPFN_ACCEPTEX acceptex_;
			} loader(socket_.descriptor());

			std::lock_guard<decltype(mutex_)> lck(mutex_);

			sockets_[index].first = new socket(socket_.listener());
			std::size_t buffer_size = 2048;

			sockets_[index].second = new internal::connection_operation(handler, *this, index, buffer_size);
			if(loader.acceptex_(socket_.descriptor(), 
								sockets_[index].first->descriptor(),
								sockets_[index].second->buffer, 
								static_cast<DWORD>(buffer_size - (2 * (sizeof(address::address_type) + 16))),
								sizeof(address::address_type) + 16, 
								sizeof(address::address_type) + 16,
								0,
								&sockets_[index].second->overlapped) == FALSE)
				if(::WSAGetLastError() != WSA_IO_PENDING) {
					handler(WSAGetLastError(), 0, ip::tcp::socket::ref(sockets_[index].first), jodocus::buffer(sockets_[index].second->buffer, buffer_size));
					std::lock_guard<std::mutex> lck(mutex_);
					stop(); // if an exception occurs, a new start()-call must be made
				}
		}

		void ip::tcp::acceptor::watchdog_() {
			for(;;) {
				DWORD ret = 0;
				HANDLE ev = ev_.handle();
				if((ret = ::WSAWaitForMultipleEvents(1, &ev, TRUE, INFINITE, FALSE)) == WSA_WAIT_FAILED)
					throw net_error("acceptex waiting check failure");
				if(!running_) break;
				{	
					// check for stale clients
					for(std::size_t index = 0; index < pending_accepts_; ++index) {
						// determine whether AcceptEx-operation completed or not							 
						DWORD result = ::WaitForSingleObject(sockets_[index].second->overlapped.hEvent, 20); // 20ms because of possible context swap delays // ist geschummelt, ich weiß ;-)
						switch(result) {																	 // leading to wrong WAIT_TIMEOUT
						case WAIT_TIMEOUT:
							if(sockets_[index].first->connect_time() > 2) {	// 2 seconds connected, but nothing sent --> disconnect from bad client
								if(!::CancelIoEx(reinterpret_cast<HANDLE>(socket_.descriptor()), &sockets_[index].second->overlapped)) // abort outstanding blocked AcceptEx
									if(GetLastError() != ERROR_NOT_FOUND)															   // and will cause 995 error in dispatcher
										throw net_error("acceptex canceling failure in check thread");									   // dispatcher will call a new AcceptEx
							}
							break;
						case WAIT_OBJECT_0: // overlapped-event is set which means that AcceptEx completed while the watchdog checks --> don't disconnect!
							break;
						default:
							throw net_error("unexpected event failure in check thread");
						}
					}

					// burst detection and compensation
					if(pending_accepts_ * 2 < sockets_.size()) { // maximum burst limitation 
						for(std::size_t n = pending_accepts_; n < pending_accepts_ * 2; ++n)
							do_accept(handler_, n);
						pending_accepts_.exchange(pending_accepts_ * 2);
					}
					ev_.reset();
				}
			}
		}

		// stop the acceptor-process
		void ip::tcp::acceptor::stop() const {
			if(!running_) return;
			socket_.cancel();
			running_ = false;
		}

		// returns the number of currently incomming connections
		std::size_t ip::tcp::acceptor::burst() const {
			return burst_count_;
		}

		ip::tcp::resolver::const_iterator ip::tcp::localhost(const jodocus::string& service) {
			return jodocus::net::localhost<transport_type>(service);
		}

		ip::udp::resolver::const_iterator ip::udp::localhost(const jodocus::string& service) {
			return jodocus::net::localhost<transport_type>(service);
		}

	}
}