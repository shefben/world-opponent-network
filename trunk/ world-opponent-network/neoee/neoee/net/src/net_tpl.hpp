// TCP-Definitions
#pragma once
#include <iostream>

namespace jodocus {
	namespace net {

		template <typename Protocol>
		native_socket_type internal::api_allocate_socket() {
			native_socket_type socket = INVALID_SOCKET;
			if((socket = ::WSASocket(typename Protocol::address_family,
									 typename Protocol::socket_type,
									 typename Protocol::protocol_family,
									 0, 0, WSA_FLAG_OVERLAPPED)) == SOCKET_ERROR)
				throw net_error("socket creation error");
			return socket;
		}

		template <typename Protocol>
		std::deque<native_socket_type> internal::create_sockets() {
			internal::startup();
			typedef std::deque<native_socket_type> svec;
			svec::size_type size = JODOCUS_SOCKETPOOL_SIZE;
			svec sockets(size);
			DWORD ipv6_mode = FALSE;
			for(svec::size_type n = 0; n < size; ++n) {
				sockets[n] = api_allocate_socket<Protocol>();
				// use dual stack sockets to hide IPv4/IPv6 implementations
				if(::setsockopt(sockets[n], IPPROTO_IPV6, IPV6_V6ONLY, reinterpret_cast<char*>(&ipv6_mode), sizeof ipv6_mode) == SOCKET_ERROR)
					throw net_error("dual stack socket mode not supported on this system");
			}
			return sockets;
		}
		
		template <typename Protocol>
		native_socket_type basic_protocol<Protocol>::allocate() {
			std::lock_guard<decltype(mutex_)> lck(mutex_);
			if(sockets().size() <= JODOCUS_SOCKETPOOL_SIZE / 10) { // the pool shrinked to 10% size --> refill it
				for(unsigned n = 0; n < JODOCUS_SOCKETPOOL_SIZE - (JODOCUS_SOCKETPOOL_SIZE / 10); ++n)
					sockets().push_back(internal::api_allocate_socket<Protocol>());
			}
			native_socket_type s = sockets().back();
			sockets().pop_back();
			return s;

			/*auto socket = internal::api_allocate_socket<Protocol>();
			DWORD ipv6_mode = FALSE;
			if(::setsockopt(socket, IPPROTO_IPV6, IPV6_V6ONLY, reinterpret_cast<char*>(&ipv6_mode), sizeof ipv6_mode) == SOCKET_ERROR)
				throw net_error("dual stack socket mode not supported on this system");
			return socket;*/
		}

		template <typename Protocol>
		void basic_protocol<Protocol>::deallocate(native_socket_type socket) {
			/*std::lock_guard<decltype(mutex_)> lck(mutex_);
			sockets().push_back(socket);*/

			internal::api_deallocate_socket(socket);
		}

		template <typename Protocol>
		std::deque<native_socket_type>& basic_protocol<Protocol>::sockets() {
			static wrapper socket_pool(internal::create_sockets<Protocol>()); // create socketpool in first call
			return socket_pool.storage;
		}


		template <typename Protocol>
		basic_address<Protocol>::basic_address()
			: address_(new address_type) {
			std::memset(address_, 0, sizeof address_type);
		}

		template <typename Protocol>
		basic_address<Protocol>::basic_address(const basic_address<Protocol>& address) {
			*this = address;
		}

		template <typename Protocol>
		basic_address<Protocol>::basic_address(const SOCKADDR* address, std::size_t size)
			: address_(new address_type)
		{
			std::memset(address_, 0, sizeof address_type);
			std::memcpy(address_, address, size);
		}

		template <typename Protocol>
		basic_address<Protocol>::basic_address(basic_address<Protocol>&& address)
			: address_(address.address_)
		{
			address.address_ = 0;
		}

		template <typename Protocol>
		basic_address<Protocol>& basic_address<Protocol>::operator =(const basic_address<Protocol>& address) {
			address_ = new address_type;
			std::memcpy(address_, address.address_, sizeof(address_type));
			return *this;
		}

		template <typename Protocol>
		basic_address<Protocol>& basic_address<Protocol>::operator =(basic_address<Protocol>&& address) {
			swap(address);
			return *this;
		}

		template <typename Protocol>
		basic_address<Protocol>::~basic_address() {
			delete address_;
		}

		template <typename Protocol>
		bool basic_address<Protocol>::operator ==(const basic_address<Protocol>& addr) const {
			return !std::memcmp(address_, addr.address_, sizeof(basic_address<Protocol>::address_type));
		}

		template <typename Protocol>
		bool basic_address<Protocol>::operator !=(const basic_address<Protocol>& addr) const {
			return !(addr == *this);
		}

		template <typename Protocol>
		jodocus::ostream& operator <<(jodocus::ostream& os, const basic_address<Protocol>& address) {
			return os << address.to_string();
		}


		template <typename Protocol>
		void basic_address<Protocol>::swap(basic_address&& address) {
			address_type* tmp = address.address_;
			address.address_ = address_;
			address_ = tmp;
		}

		template <typename Protocol>
		jodocus::string basic_address<Protocol>::port() const {
			jodocus::string peer = to_string();
			auto position = peer.find_last_of(':') + 1;
			return jodocus::string(peer.begin() + position, peer.end());
		}

		template <typename Protocol>
		jodocus::string basic_address<Protocol>::ip_address() const {
			jodocus::string peer = to_string();
			auto position = peer.find_last_of(':');
			if(position == jodocus::string::npos)
				position = peer.size();
			return jodocus::string(peer.begin(), peer.begin() + position);
		}

		template <typename Protocol>
		jodocus::string basic_address<Protocol>::to_string() const {
			internal::startup();
			TCHAR buf[INET6_ADDRSTRLEN * 2] = { 0 }; // large enough? :O
			DWORD length = INET6_ADDRSTRLEN * 2;
			if(::WSAAddressToString(reinterpret_cast<SOCKADDR*>(address_), sizeof(SOCKADDR_STORAGE), 0, buf, &length) == SOCKET_ERROR)
				throw net_error("address to string conversion error");
			return jodocus::string(buf);
		}

		template <typename Protocol>
		const basic_address<Protocol>* basic_address<Protocol>::operator ->() const {
			return this;
		}

		template <typename Protocol>
		const typename basic_address<Protocol>::address_type* basic_address<Protocol>::api_address() const {
			return address_;
		}

		template <typename Protocol>
		basic_resolver<Protocol>::basic_resolver(const listener& li) { }

		template <typename Protocol>
		ADDRINFOT* basic_resolver<Protocol>::getaddrinfo(const TCHAR* host, const TCHAR* service, unsigned short tryings) {
			internal::startup();
			ADDRINFOT hints = { 0 }, *ai = 0;
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = Protocol::socket_type;
			hints.ai_protocol = Protocol::protocol_family;
			if(!host) hints.ai_flags = AI_PASSIVE;
			ICantBelieveThatIuseGoto:	
			switch(::GetAddrInfo(const_cast<TCHAR*>(host), 
								 const_cast<TCHAR*>(service),
								 &hints, 
								 &ai)) {
			case WSAHOST_NOT_FOUND:
			case EAI_AGAIN:
				if(tryings > 0) {
					--tryings;
					goto ICantBelieveThatIuseGoto;
				}
				else
					throw net_error("number of address resolve attempts exceeded");
				break;
			case NO_ERROR:
					break;
			default:
				throw net_error("address resolving error");
			}
			return ai;
		}

		template <typename Protocol>
		typename basic_resolver<Protocol>::const_iterator basic_resolver<Protocol>::resolve(const jodocus::string& host, const jodocus::string& service, unsigned short tryings) const {
			return static_cast<const_iterator>(const_cast<basic_resolver<Protocol>*>(this)->resolve(host, service, tryings));
		}
		
		template <typename Protocol>
		typename basic_resolver<Protocol>::iterator basic_resolver<Protocol>::resolve(const jodocus::string& host, const jodocus::string& service, unsigned short tryings) {
			return iterator(getaddrinfo(host.c_str(), service.c_str(), tryings));
		}

		template <typename Protocol>
		typename basic_resolver<Protocol>::const_iterator localhost(const jodocus::string& service) {
			return basic_resolver<Protocol>::const_iterator(basic_resolver<Protocol>::getaddrinfo(0, service.c_str(), 3));
		}

		template <typename Protocol>
		basic_resolver<Protocol>::const_iterator::const_iterator()
			: ptr_(0), base_(0)
		{ }

		template <typename Protocol>
		basic_resolver<Protocol>::const_iterator::const_iterator(const typename basic_resolver<Protocol>::const_iterator& iter) 
			: ptr_(iter.ptr_), base_(0)
		{ }

		template <typename Protocol>
		typename basic_resolver<Protocol>::const_iterator& basic_resolver<Protocol>::const_iterator::operator =(const typename basic_resolver<Protocol>::const_iterator& iter) {
			ptr_ = iter.ptr_;
			cleanup();
			base_ = 0;
		}

		template <typename Protocol>
		typename basic_resolver<Protocol>::const_iterator& basic_resolver<Protocol>::const_iterator::operator =(typename basic_resolver<Protocol>::const_iterator&& iter) {
			ptr_ = iter.ptr_;
			base_ = iter.base_;
			iter.base_ = 0;
			return *this;
		}

		template <typename Protocol>
		basic_resolver<Protocol>::const_iterator::const_iterator(typename basic_resolver<Protocol>::const_iterator&& iter)
			: ptr_(iter.ptr_), base_(iter.base_)
		{
			iter.base_ = 0;
		}

		template <typename Protocol>
		basic_resolver<Protocol>::const_iterator::const_iterator(pointer ptr)
			: ptr_(ptr), base_(ptr)
		{ }

		template <typename Protocol>
		typename basic_resolver<Protocol>::const_iterator& basic_resolver<Protocol>::const_iterator::operator ++() {
			ptr_ = ptr_->ai_next;
			return *this;
		}

		template <typename Protocol>
		typename basic_resolver<Protocol>::const_iterator basic_resolver<Protocol>::const_iterator::operator ++(int) {
			auto tmp = *this;
			ptr_ = ptr_->ai_next;
			return tmp;
		}

		template <typename Protocol>
		bool basic_resolver<Protocol>::const_iterator::operator ==(const const_iterator& iter) const {
			return ptr_ == iter.ptr_;
		}

		template <typename Protocol>
		bool basic_resolver<Protocol>::const_iterator::operator !=(const const_iterator& iter) const {
			return !(*this == iter);
		}

		template <typename Protocol>
		const basic_address<Protocol> basic_resolver<Protocol>::const_iterator::operator *() const {
			return basic_address<Protocol>(ptr_->ai_addr, ptr_->ai_addrlen);
		}

		template <typename Protocol>
		const basic_address<Protocol> basic_resolver<Protocol>::const_iterator::operator ->() const {
			return basic_address<Protocol>(ptr_->ai_addr);
		}

		template <typename Protocol>
		basic_resolver<Protocol>::const_iterator::~const_iterator() {
			cleanup_();
		}

		template <typename Protocol>
		void basic_resolver<Protocol>::const_iterator::cleanup_() const {
			::FreeAddrInfo(const_cast<ADDRINFOT*>(base_));
		}

		template <typename Protocol>
		basic_resolver<Protocol>::iterator::iterator()
			: basic_resolver<Protocol>::const_iterator()
		{ }

		template <typename Protocol>
		basic_resolver<Protocol>::iterator::iterator(pointer address)
			: basic_resolver<Protocol>::const_iterator(address)
		{ }

		// Pseudo_Container

		template <typename Type>
		internal::pseudo_container<Type>::pseudo_container(Type& value)
			: rep_(value)
		{ }

		template <typename Type>
		template <typename ConvertableType>
		internal::pseudo_container<Type>::pseudo_container(const pseudo_container<ConvertableType>& container)
			: rep_(container.rep_)
		{ }

		template <typename Type>
		typename internal::pseudo_container<Type>::iterator internal::pseudo_container<Type>::begin() {
			return &rep_;
		}

		template <typename Type>
		typename internal::pseudo_container<Type>::const_iterator internal::pseudo_container<Type>::begin() const {
			return const_cast<pseudo_container<Type>*>(this)->begin();
		}

		template <typename Type>
		typename internal::pseudo_container<Type>::iterator internal::pseudo_container<Type>::end() {
			return &rep_ + 1;
		}

		template <typename Type>
		typename internal::pseudo_container<Type>::const_iterator internal::pseudo_container<Type>::end() const {
			return const_cast<pseudo_container<Type>*>(this)->end();
		}

		template <typename Type>
		std::size_t internal::pseudo_container<Type>::size() const {
			return 1;
		}


		// Socket Interface Base

		template <typename Protocol>
		internal::socket_interface_base<Protocol>::socket_interface_base() 
			: socket_(Protocol::allocate())
		{ }

		template <typename Protocol>
		internal::socket_interface_base<Protocol>::socket_interface_base(native_socket_type api_socket)
		{ }


		template <typename Protocol>
		internal::socket_interface_base<Protocol>::~socket_interface_base() {
			if(socket_ != internal::invalid_socket())
				Protocol::deallocate(socket_);
		}

		// Socket Interfaces


		/* ********* TCP/IP ***************** */

		template <typename Iter>
		std::vector<WSABUF> internal::make_api_buf_sequence(Iter first, Iter last) {
			std::vector<WSABUF> buffers(std::distance(first, last));
			unsigned int counter = 0;
			for(; first != last; ++first) {
				buffers[counter].buf = reinterpret_cast<char*>(const_cast<void*>(first->storage()));
				buffers[counter].len = static_cast<DWORD>(first->size());
				++counter;
			}
			return buffers;
		}

		template <typename BufferSequence>
		void internal::socket_interface<tcp<internal::ip>>::send(const BufferSequence& buffer_seq, std::function<void (unsigned, unsigned)> handler) const
		{
			internal::startup();
			auto operation = new internal::io_operation(handler);
			std::vector<WSABUF> buffers(make_api_buf_sequence(buffer_seq.begin(), buffer_seq.end()));
			if(::WSASend(socket_, &buffers[0], static_cast<DWORD>(buffers.size()), 0, 0, &operation->overlapped, 0) == SOCKET_ERROR) 
				switch(WSAGetLastError()) {
				case WSA_IO_PENDING:
					break;
				case WSAEDISCON:
				case WSAENOTCONN:
				case WSAECONNRESET:
					{
						std::auto_ptr<internal::io_operation> ptr(operation); // exception safety due to handler-call
						handler(WSAGetLastError(), 0);
						break;
					}
				default:
					delete operation;
					throw net_error("socket sending data failure");
				}
		}

		template <typename BufferSequence>
		void internal::socket_interface<tcp<internal::ip>>::receive(const BufferSequence& buffer_seq, std::function<void (unsigned, unsigned)> handler) const
		{
			internal::startup();
			internal::io_operation* operation = new internal::io_operation(handler);
			std::vector<WSABUF> buffers(make_api_buf_sequence(buffer_seq.begin(), buffer_seq.end()));
			DWORD flags = 0;
			if(::WSARecv(socket_, &buffers[0], static_cast<DWORD>(buffers.size()), 0, &flags, &operation->overlapped, 0) == SOCKET_ERROR)
				switch(WSAGetLastError()) {
				case WSA_IO_PENDING:
				break;
				case WSAEDISCON:
				case WSAECONNRESET:
				case WSAECONNABORTED:
					{
						std::auto_ptr<internal::io_operation> ptr(operation); // exception safety due to handler-call
						handler(WSAGetLastError(), 0);
						break;
					}
				default:
					delete operation;
					throw net_error("socket receiving data failure");
				}
		}

		template <typename BufferSequence>
		void internal::socket_interface<udp<internal::ip>>::send(const BufferSequence& buffer_seq, const basic_address<udp<internal::ip>>& address, std::function<void (unsigned, unsigned)> handler) const
		{
			internal::startup();
			internal::io_operation* operation = new internal::io_operation(handler);
			std::vector<WSABUF> buffers(make_api_buf_sequence(buffer_seq.begin(), buffer_seq.end()));
			SOCKADDR_STORAGE remote_addr = { 0 };
			if(address.api_address()->ss_family == AF_INET) {
				sockaddr_in6 addrv6 = { 0 };
				SCOPE_ID scope = INETADDR_SCOPE_ID(reinterpret_cast<const SOCKADDR*>(address.api_address()));
				IN6ADDR_SETV4MAPPED(&addrv6, reinterpret_cast<const IN_ADDR*>(INETADDR_ADDRESS(reinterpret_cast<const SOCKADDR*>(address.api_address()))), scope, INETADDR_PORT(reinterpret_cast<const SOCKADDR*>(address.api_address())));
				std::memcpy(reinterpret_cast<SOCKADDR*>(&remote_addr), &addrv6, sizeof addrv6);
			}
			else {
				std::memcpy(&remote_addr, address.api_address(), INET_SOCKADDR_LENGTH(AF_INET6));
			}

			if(::WSASendTo(socket_, &buffers[0], static_cast<DWORD>(buffers.size()), 0, 0, reinterpret_cast<sockaddr*>(&remote_addr), sizeof(remote_addr), &operation->overlapped, 0) == SOCKET_ERROR) 
				switch(WSAGetLastError()) {
				case WSA_IO_PENDING:
					break;
				case WSAECONNRESET:
				case WSAECONNABORTED:
					{
						std::auto_ptr<internal::io_operation> ptr(operation); // exception safety due to handler
						handler(WSAGetLastError(), 0);
						break;
					}
				default:
					delete operation;
					throw net_error("socket sending datagram failure");
				}
		}

		template <typename BufferSequence>
		void internal::socket_interface<udp<internal::ip>>::receive(const BufferSequence& buffer_seq, basic_address<udp<ip>>& address, std::function<void (unsigned, unsigned)> handler) const
		{
			internal::startup();
			internal::io_operation* operation = new internal::io_operation(handler);
			std::vector<WSABUF> buffers(make_api_buf_sequence(buffer_seq.begin(), buffer_seq.end()));
			DWORD flags = 0;
			int size = sizeof (basic_address<udp<internal::ip>>::address_type);
			if(::WSARecvFrom(socket_, &buffers[0], static_cast<DWORD>(buffers.size()), 0, &flags, reinterpret_cast<sockaddr*>(const_cast<basic_address<udp<internal::ip>>::address_type*>(address.api_address())), &size, &operation->overlapped, 0) == SOCKET_ERROR)
				switch(WSAGetLastError()) {
				case WSA_IO_PENDING:
				break;
				case WSAEDISCON:
				case WSAECONNRESET:
				case WSAECONNABORTED:
					{
						std::auto_ptr<internal::io_operation> ptr(operation); // exception safety due to handler
						handler(::WSAGetLastError(), 0);
						break;
					}
				default:
					delete operation;
					throw net_error("socket receiving datagram failure");
				}
		}

		// Basic Socket

		template <typename Protocol>
		basic_socket<Protocol>::basic_socket(const jodocus::net::listener& li)
			: listener_(li)
		{
			internal::startup();
			// Associate Socket with completion port
			if(::CreateIoCompletionPort(reinterpret_cast<HANDLE>(socket_), li.completion_port_, reinterpret_cast<ULONG_PTR>(this), 0) == NULL)
				if(GetLastError() != ERROR_INVALID_PARAMETER)
					throw net_error("completion port association error");
		}

		template <typename Protocol>
		basic_socket<Protocol>::basic_socket(const jodocus::net::listener& li, const basic_address<Protocol>& address)
			: listener_(li)
		{
			internal::startup();
			// Associate Socket with completion port
			if(::CreateIoCompletionPort(reinterpret_cast<HANDLE>(socket_), li.completion_port_, reinterpret_cast<ULONG_PTR>(this), 0) == NULL)
				if(GetLastError() != ERROR_INVALID_PARAMETER)
					throw net_error("completion port association error");
			// Bind socket to local interface explicitely
			Protocol::address_type local_addr = { 0 };
			reinterpret_cast<SOCKADDR_STORAGE*>(&local_addr)->ss_family = Protocol::address_family;
			if(address.api_address()->ss_family == AF_INET) {
				sockaddr_in6 addrv6 = { 0 };
				SCOPE_ID scope = INETADDR_SCOPE_ID(reinterpret_cast<const SOCKADDR*>(address.api_address()));
				IN6ADDR_SETV4MAPPED(&addrv6, reinterpret_cast<const IN_ADDR*>(INETADDR_ADDRESS(reinterpret_cast<const SOCKADDR*>(address.api_address()))), scope, INETADDR_PORT(reinterpret_cast<const SOCKADDR*>(address.api_address())));
				std::memcpy(reinterpret_cast<SOCKADDR*>(&local_addr), &addrv6, sizeof addrv6);
			}
			else
				std::memcpy(&local_addr, address.api_address(), sizeof local_addr);
			if(::bind(socket_, reinterpret_cast<const sockaddr*>(&local_addr), sizeof(basic_address<Protocol>::address_type)) == SOCKET_ERROR)
				throw net_error("bind call error");
		}

		template <typename Protocol>
		basic_socket<Protocol>::basic_socket(basic_socket<Protocol>&& socket) 
			: socket_interface<Protocol>(), listener_(socket.listener_)
		{
			std::lock_guard<decltype(socket.mutex_)> lck(socket.mutex_);
			socket_ = socket.socket_;
			socket.socket_ = internal::invalid_socket();	
		}

		template <typename Protocol>
		basic_socket<Protocol>::~basic_socket() {
			std::lock_guard<decltype(mutex_)> lck(mutex_);
			if(socket_ != internal::invalid_socket()) // <--- CAREFUL HERE
				cancel();
		}

		template <typename Protocol>
		const listener& basic_socket<Protocol>::listener() const {
			return listener_;
		}

		template <typename Protocol>
		native_socket_type basic_socket<Protocol>::descriptor() const {
			std::lock_guard<decltype(mutex_)> lck(mutex_);
			return socket_;
		}

		template <typename Protocol>
		bool basic_socket<Protocol>::cancel() const {
			if(!::CancelIoEx(reinterpret_cast<HANDLE>(socket_), 0)) {
				if(::GetLastError() != ERROR_NOT_FOUND)
					throw net_error("operation canceling failure");
				else 
					return false;
			}
			return true;
		}

	}
}