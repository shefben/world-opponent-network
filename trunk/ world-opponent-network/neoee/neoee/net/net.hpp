#pragma once

// library headers
#include "../jodocus.hpp"
#include "../event.hpp"
#include "../buffer.hpp"

// api headers
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <MSTcpIP.h>
#include <MSWSock.h>

// std headers
#include <algorithm>
#include <allocators>
#include <atomic>
#include <deque>
#include <functional>
#include <iosfwd>
#include <iterator>
#include <memory>
#include <mutex>
#include <thread>
#include <type_traits>
#include <utility>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")
#ifndef JODOCUS_SOCKETPOOL_SIZE
#define JODOCUS_SOCKETPOOL_SIZE 2048
#endif

JODOCUS_NS_BEGIN

namespace net {

	typedef SOCKET native_socket_type;
	typedef unsigned error_type;

	/**
	*	The listener coordinates all the network I/O notifications and dispatches tasks to the worker threads.
	*	It's creation is obligatory due to the fact that every network object has to be registered with an listener object in their constructors.
	*	As long as the listener object is valid, the application will receive network events, it has to be valid till any
	*	other network object is destroyed.<br />
	*	The listener is uncopyable but movable, though.
	*/
	class listener : jodocus::uncopyable {
	public:
		/**
		*	Creates a new listener object and starts the worker threads. The parameter specifies the amount of concurrent threads. It's amount should
		*	not exceed the amount of available CPUs of the system. (You can determine the number with a call to jodocus::system() using its num_cpu member.)
		
		*/
		listener(unsigned);
			/**
		*	Destroys the listener object and synchronizes with the worker threads. The calling thread will block until every worker thread has been
		*	shutdown.
		*/
		~listener();
			/** 
		*	The count() member returns the amount of running worker threads; the return value is identical to the contructors parameter. 
		*/
		std::size_t count() const;
		template <typename>
		friend class basic_socket;
		/**
		*	Dispatches all network events to the specified event handlers given to the I/O functions.
		*/
		struct dispatcher {
			dispatcher(const listener&);
			void operator() ();
				const listener& listener_;
		};
		private:
		HANDLE completion_port_;
		std::vector<std::thread*> threads_;
	};
	
	/**
	*	net_error is an exception class providing error handling within network applications. All parts of code using jodocus::net-objects should 
	*	catch net_error or its super class std::exception.
	*/
	class net_error : public std::exception {
	public:
		/**
		*	Creates a net_error object.<br />The <i>message</i> parameter specifies a special error message to determine the error's origin.<br />
		*	The <i>number</i> parameter can specify a special error number. However, the paramenter usually shouldn't be specified since its default value
		*	is the Winsock error code of the failed network operation.
		*/
		net_error(const std::string& message, error_type number = WSAGetLastError()) 
			: std::exception(), no_(number)
		{ 
			std::stringstream ss;
			ss << "jodocus network error\nmessage: " << message << "\nError-Number: " << no_ << '\n';
			message_ = ss.str();
		}

		/**
		* Returns a human-readable formatted error message containing the error number.
		*/
		const char* what() const {
			return message_.c_str();
		}

	private:
		error_type no_;
		std::string message_;	
	};

	/**
	* The network class occupies the ws2_32.dll access. Since an object of it is created in static memory space (and therefore created before main())
	* users shouldn't call it again, it won't have any effect, though.
	*/
	class network : jodocus::uncopyable {
	public:
		/**
		*	Loads the ws2_32.dll.
		*/
		network();

		/** 
		*	Unloads the ws2_32.dll
		*/
		~network();
	};

	namespace internal {

		void startup();

		native_socket_type invalid_socket();

		enum op_type {
			io_op, sock_op
		};

		class ip {
		public:
			typedef SOCKADDR_STORAGE address_type;
			enum {
				address_family = AF_INET6
			};
		};

		template <typename Protocol>
		class socket_interface_base {
		public:
			socket_interface_base();
			socket_interface_base(native_socket_type);
			~socket_interface_base();

		protected:
			native_socket_type socket_;
		};

		template <typename Protocol>
		class socket_interface;

		template <typename Protocol>
		std::deque<native_socket_type> create_sockets();

		template <typename Protocol>
		native_socket_type api_allocate_socket();


		void api_deallocate_socket(native_socket_type);
		void delete_sockets(std::deque<native_socket_type>&);

		template <typename Iter>
		std::vector<WSABUF> make_api_buf_sequence(Iter, Iter); // RVO

	}
		
	/**
	*	The basic_protocol<Protocol> class defines the basic behavior of socket creation and destruction within it's special protocol
	*	specifications.
	*/
	template <typename Protocol>
	class basic_protocol {	
	public:
		/**
		*	This function semantically allocates a dual-stack socket descriptor and returns it to the caller. The caller of this function has the responsibility to
		*	destroy the returned object properly when it's not longer needed by calling the <i>deallocate()</i> member function.
		*/
		static native_socket_type allocate();

		/**
		*	The function takes a socket descriptor (which has to be returned by <i>allocate()</i>) and semantically destroys it.
		*/
		static void deallocate(native_socket_type);

	protected:
		basic_protocol();

		struct wrapper {
			wrapper(const std::deque<native_socket_type>& s)
				: storage(s) { }

			~wrapper() {
				internal::delete_sockets(storage);
			}

			std::deque<native_socket_type> storage;
		};

		static std::mutex mutex_; // synchronisation object for socket queue
		static std::deque<native_socket_type>& sockets();					
	};

	template <typename Protocol>
	std::mutex basic_protocol<Protocol>::mutex_;


	/**
	*	The tcp<NetworkLayer> class defines socket constants for stream-oriented UDP sockets or other network objects requiring UDP specifications
	*	like <i>basic_address</i>. The <i>NetworkLevel</i> template parameter specifies the addressing protocol. It is usually jodocus::net::internal::ip.
	*/
	template <typename NetworkLayer>
	class tcp : public basic_protocol<tcp<NetworkLayer>> {
	public:
		enum {
			address_family = NetworkLayer::address_family,
			protocol_family = IPPROTO_TCP,
			socket_type = SOCK_STREAM
		};

		typedef NetworkLayer network_layer;
		typedef typename network_layer::address_type address_type;
	};

	/**
	*	The udp<NetworkLayer> class defines socket constants for datagram-oriented UDP sockets or other network objects requiring UDP specifications
	*	like <i>basic_address</i>. The <i>NetworkLevel</i> template parameter specifies the addressing protocol. It is usually jodocus::net::internal::ip.
	*/
	template <typename NetworkLayer>
	class udp : public basic_protocol<udp<NetworkLayer>> {
	public:
		enum {
			address_family = NetworkLayer::address_family,
			protocol_family = IPPROTO_UDP,
			socket_type = SOCK_DGRAM
		};

		typedef NetworkLayer network_layer;
		typedef typename network_layer::address_type address_type;
	};

	/**
	*	The basic_address wraps both IPv4 and IPv6 addresses. It provides normal value/copy and move semantics, can be compared and
	*	converted to strings. The template parameter specifies the network protocol. For instance a socket<tcp> may only send data to a
	*	address<tcp>.
	*/
	template <typename Protocol>
	class basic_address {
	public:
		typedef typename Protocol::network_layer::address_type address_type;

		/**
		*	Creates an empty (thus invalid) address. Its value is 0.
		*/
		basic_address();

		/**
		*	Creates an address from another one by deep copying the address data.
		*/
		basic_address(const basic_address&);

		/**
		*	Creates an address from an rvalue-address. The old address-object (<b>not the address itself</b>) will be invalid afterwards. It should be destroyed.
		*/
		basic_address(basic_address&&);

		/**
		*	Creates an address from an API data type (POD), the size of the object has to be given as well.
		*/
		explicit basic_address(const SOCKADDR*, std::size_t);

		/**
		*	Assigns an address to another; the value will be copied.
		*/
		basic_address& operator =(const basic_address&);

		/**
		*	Assigns an address to another; the value will be moved, the source object will be invalid afterwards.
		*/
		basic_address& operator =(basic_address&&);

		/**
		*	Destroys an address object and frees its resources.
		*/
		~basic_address();

		/**
		*	This function compares two addresses <i>bitwise</i>.
		*/
		bool operator ==(const basic_address&) const;

		/**
		*	This function returns the inversed return value of operator ==().
		*/
		bool operator !=(const basic_address&) const;

		/**
		*	This function returns the <i>this</i>-Pointer of the calling object.
		*/
		const basic_address* operator ->() const;
			
		/**
		*	This function converts the address into string format and writes it into the given stream.
		*/
		template <typename>
		friend jodocus::ostream& operator <<(jodocus::ostream&, const basic_address&);

		/**
		*	This function swaps two addresses using move semantics.
		*/
		void swap(basic_address&&);

		/**
		*	This function returns the port number of the address in string format.
		*/
		jodocus::string port() const;

		/**
		*	This function returns the IP address of the address in string format.
		*/
		jodocus::string ip_address() const;

		/**
		*	This function returns a pointer to the API-POD type of the address.
		*/
		const address_type* api_address() const;

	private:
		jodocus::string to_string() const;
		address_type* address_; 
	};

	/**
	*	The basic_resolver class provides namespace resolving facilites, e.g. DNS services or others.
	*	The template parameters specifies the used protocol which is needed for the generated basic_address
	*	objects. Owing to the fact that it is uncopyable, you can only move basic_resolver objects.
	*/
	template <typename Protocol>
	class basic_resolver : jodocus::uncopyable {
	public:
		/**
		*	Creates an resolver object. An listener object has to be specified, even though the resolving
		*	process is not implemented asynchronously yet.
		*/
		explicit basic_resolver(const listener&);

		// address iterators

		/**
		*	For iterating through the host aliases the basic_resovler previously determined through a resolve() call,
		*	the <i>const_iterator</i> class provides <i>forward iterator</i> semantics; thus you can access every single
		*	basic_address by dereferencing an const_iterator object and increasing it by operator ++().
		*/
		class const_iterator : public std::iterator<std::forward_iterator_tag, const ADDRINFOT>  {
		public:
			/**
			*	The standard constructor creates an invalid const_iterator. The value of an iterator created by it is identical
			*	to the last plus one element of an alias list, therefore iterating loops should use this constructor for the loop condition.
			*/
			const_iterator();

			/**
			*	This function creates an const_iterator basing on a given pointer. The caller keeps the responsibility to free this resource.
			*/
			explicit const_iterator(pointer);

			/**
			*	The copy constructor creates a new const_iterator basing on the given one. Note that the resource won't be copied, so both
			*	the parameter and the newly created iterator will point to the same resource.
			*/
			const_iterator(const const_iterator&);

			/**
			*	The move constructor creates a new const_iterator basing on the given one. Note that the parameter iterator will be invalid after
			*	it has been given to this constructor, it's value has been moved to the new one.
			*/
			const_iterator(const_iterator&&);

			/**
			*	Assigns an const_iterator to another. After calling this function, both the iterators will point to the same resource.
			*/
			const_iterator& operator =(const const_iterator&);

			/**
			*	Assigns an const_iterator to another. After calling this function, the given iterator will be invalid and its value moved.
			*/
			const_iterator& operator =(const_iterator&&);

			/**
			*	The destructor destroys an iterator and frees its resources.
			*/
			~const_iterator();

			/**
			*	Increments the iterator by one and returns its new value.
			*/
			const_iterator& operator ++();

			/**
			*	Increments the iterator by one and returns the value before the increment.
			*/
			const_iterator operator ++(int);

			/**
			*	Compares two iterators; it will return true if the two iterators point to the very same address. The address values won't be compared.
			*/
			bool operator ==(const const_iterator&) const;

			/**
			*	Returns the inverted result of operator ==()
			*/
			bool operator !=(const const_iterator&) const;

			/**
			*	Dereferences the iterator returning a basic_address alias.
			*/
			const basic_address<Protocol> operator *() const;

			/**
			*	Dereferences the iterator returning a basic_address alias.
			*/
			const basic_address<Protocol> operator ->() const;
		private:
			void cleanup_() const;
			pointer ptr_, base_;
		};

		/**
		*	The iterator class behaves semantically almost like the const_iterator class. The access through the addresses is
		*	mutable, therefore addresses can be manipulated by the user.
		*/
		class iterator : public const_iterator {
		public:
			typedef ADDRINFOT value_type;
			typedef value_type* pointer;
			typedef value_type& reference;

			/**
			*	The standard constructor creates an invalid iterator object; it behaves similar to the const_iterators equivalent.
			*/
			iterator();

			/**
			*	This constructor creates an iterator object basing on a given pointer to the resource.
			*/
			explicit iterator(pointer);
		};

		/**
		*	The resolve function <i>resolves</i> an address from string into binary format. The first parameter specifies the host name.
		*	You may pass an IPv4 ("127.0.0.1"), IPv6 ("[::1]") or a DNS name ("localhost") to the function - everything will be accepted and resolved.
		*	<br />The second parameter specifies the service name; you can give a standard service name (like "http" or "ftp") or use a port
		*	number like ("80" or "21"). The third parameter is the amount of retryings in case that a resolving attempt may fail. However, its default value is 3.
		*	<br />This function returns an const_iterator to the first alias name of the given address. You may increase it until it is invalid and
		*	identical to a const_iterator object created by standard constructor.
		*/
		const_iterator resolve(const jodocus::string&, const jodocus::string&, unsigned short = 3) const;

		/**
		*	This function returns a mutable iterator object pointing to an alias list. Its behavior is similar to its constant equivalent.
		*/
		iterator resolve(const jodocus::string&, const jodocus::string&, unsigned short = 3);
			
		/**
		*	The localhost() function returns an const_iterator object pointing to the alias list of addresses which are found by resolving "localhost".
		*/
		template <typename Protocol>
		friend typename basic_resolver<Protocol>::const_iterator localhost(const jodocus::string&);

	private:
		static ADDRINFOT* getaddrinfo(const TCHAR*, const TCHAR*, unsigned short);
	};

	/**
	*	The jodocus::internal namespace contains framework-internal controlling and implementing structures. The user of the library
	*	should not call or use any of this functions <i>directly</i> unless he has a serious reason to do it. Therefore, it's members
	*	are not documented completey. <b>Note: you shall not use the functions <i>directly</i>. However, the basic I/O functions are
	*	implemented inside it, so having a look at the socket_interface<> template specialisations may be usefull, though.
	*/
	namespace internal {

		template <typename Type>
		class pseudo_container {
		public:
			typedef Type value_type;
			typedef value_type* pointer;
			typedef const value_type* const_pointer;
			typedef value_type& reference;
			typedef const reference const_reference;
			typedef pointer iterator;
			typedef const_pointer const_iterator;
			
			pseudo_container(Type&);

			template <typename ConvertableType>
			pseudo_container(const pseudo_container<ConvertableType>&);
			iterator begin();
			const_iterator begin() const;

			iterator end();
			const_iterator end() const;
			std::size_t size() const;
			
		private:
			Type& rep_;
		};

		/**
		*	The tcp<ip> specialisation of socket_interface implements basic I/O operations for TCP sockets (except for accepting operations:
		*	due to their complexity they're handled in the acceptor class). Users of the library should not try to instantiate objects of this
		*	class because it provides its functionality as a super-class to basic_socket<> templates.
		*/
		template <>
		class socket_interface<tcp<ip>> : public socket_interface_base<tcp<ip>> {
		public:
			/**
			*	The connect()-function connects a TCP socket to the specified host given with the first parameter. The second parameter
			*	specifies the callback, it has to be a callable object compatible/convertable to the parameter type. The third, <i>optional</i>
			*	parameters specifies and initial message, it allows you to send a message immediately after the socket is connected within one call.
			*	The operation is asynchronous, therefore the function returns immediately. When the operation is completed, the callback will be called
			*	<i>in a several thread</i>, therefore any inter-thread data has to be synchronised.
			*/
			void connect(const basic_address<tcp<ip>>&, std::function<void (unsigned, unsigned)>, const buffer& = buffer()) const;

			/**
			*	The send()-function template allows the user to send a container of jodocus::buffer on the connected socket (Scatter/Gather-I/O). The container type has to
			*	be compatible to the STL containers. The second parameter specifies the callback, it has to be a callable object compatible/convertable
			*	to the parameter type. The operation is asynchronous, therefore the function returns immediately. When the operation is completed (the data has been received by the
			*	peer), the callback will be called <i>in a several thread</i>, therefore any thread-shared data has to be synchronised.
			*/
			template <typename BufferSequence> 
			void send(const BufferSequence&, std::function<void (unsigned, unsigned)>) const;

			/**
			*	This function is an overloaded form of the function template. It's major difference is that it takes only one buffer, it internally calls the
			*	general version and behaves exactly like it. Review the send()-function template for more information.
			*/
			void send(const buffer&, std::function<void (unsigned, unsigned)>) const;

			/**
			*	This function sends a file on the socket to the target. The first parameter specifies the path where the file is located. 
			*	The second parameter specifies the callback, it has to be a callable object compatible/convertable to the parameter type.
			*	The operation is asynchronous, therefore the function returns immediately. When the operation is completed (the data has been received by the
			*	peer), the callback will be called <i>in a several thread</i>, therefore any inter-thread data has to be synchronised.
			*/
			bool send_file(const jodocus::string&, std::function<void (unsigned, unsigned)>) const;

			/**
			*	The receive()-function template allows the user to receive data to fill container of jodocus::buffer on the connected socket (Scatter/Gather-I/O). The container type has to
			*	be compatible to the STL containers. The second parameter specifies the callback, it has to be a callable object compatible/convertable
			*	to the parameter type. The operation is asynchronous, therefore the function returns immediately. When the operation is completed (the buffer is full or the peer
			*	send all the data), the callback will be called <i>in a several thread</i>, therefore any inter-thread data has to be synchronised.
			*/
			template <typename BufferSequence> 
			void receive(const BufferSequence&, std::function<void (unsigned, unsigned)>) const;

			/**
			*	This function is an overloaded form of the function template. It's major difference is that it takes only one buffer to fill, it internally calls the
			*	general version and behaves exactly like it. Review the receive()-function template for more information.
			*/
			void receive(const buffer&, std::function<void (unsigned, unsigned)>) const;

			/**
			*	Disconnects the open connection gracefully and recycles the socket resources. The socket can be used for another connect() after this function completed.
			*	The parameter specifies the callback, it has to be a callable object compatible/convertable
			*	to the parameter type. The operation is asynchronous, therefore the function returns immediately. When the operation is completed (after the TIME_WAIT state
			*	has been left, which usually is at least twice the maximum segment lifetime), the callback will be called
			*	<i>in a several thread</i>, therefore any inter-thread data has to be synchronised.	
			*/
			void disconnect(std::function<void (unsigned, unsigned)>) const;

			/**
			*	The peer()-function returns the address of the connected host on the socket. If the socket is not connected, this function throws a net_error.
			*/
			basic_address<tcp<ip>> peer() const;

			/**
			*	The connect_time()-function returns the time in seconds the socket is in ESTABLISHED mode (and therefore connected). If the socket is not connected,
			*	the function returns -1.
			*/
			int connect_time() const;

			/**
			*	The is_connect()-function effectively calls the connect_time()-function and returns true if the returned value is greater than zero, otherwise false.
			*/
			bool is_connected() const;

		protected:
			socket_interface();
			mutable std::mutex mutex_;
		};

		/**
		*	The tcp<ip> specialisation of socket_interface implements basic I/O operations for UDP sockets.
		*	Users of the library should not try to instantiate objects of this class because it provides its
		*	functionality as a super-class to basic_socket<> templates.
		*/
		template <>
		class socket_interface<udp<ip>> : public socket_interface_base<udp<ip>> {
		public:
			/**
			*	The send()-function template allows the user to send a container of jodocus::buffer on the socket (Scatter/Gather-I/O) to the address specified by the
			*	second parameter. The container type has to be compatible to the STL containers. The third parameter specifies the callback, it has to be a callable object compatible/convertable
			*	to the parameter type. The operation is asynchronous, therefore the function returns immediately. When the operation is completed (all data has been send, which <b>not</b>
			*	means that the peer received it), the callback will be called <i>in a several thread</i>, therefore any inter-thread data has to be synchronised.
			*/
			template <typename BufferSequence> 
			void send(const BufferSequence&, const basic_address<udp<ip>>&, std::function<void (unsigned, unsigned)>) const;
			
			/**
			*	This function is an overloaded form of the function template. It's major difference is that it takes only one buffer, it internally calls the
			*	general version and behaves exactly like it. Review the send()-function template for more information.
			*/
			void send(const buffer&, const basic_address<udp<ip>>&, std::function<void (unsigned, unsigned)>) const;

			/**
			*	The receive()-function template allows the user to receive data to fill container of jodocus::buffer on the socket (Scatter/Gather-I/O) from the address specified by
			*	the second parameter. The container type has to be compatible to the STL containers. The third parameter specifies the callback, it has to be a callable object compatible/convertable
			*	to the parameter type. The operation is asynchronous, therefore the function returns immediately. When the operation is completed (a chunk of data has been received),
			*	the callback will be called <i>in a several thread</i>, therefore any inter-thread data has to be synchronised.
			*/
			template <typename BufferSequence> 
			void receive(const BufferSequence&, basic_address<udp<ip>>&, std::function<void (unsigned, unsigned)>) const;

			/**
			*	This function is an overloaded form of the function template. It's major difference is that it takes only one buffer to fill, it internally calls the
			*	general version and behaves exactly like it. Review the receive()-function template for more information.
			*/
			void receive(const buffer&, basic_address<udp<ip>>&, std::function<void (unsigned, unsigned)>) const;
		protected:
			socket_interface();
		};
	}

	/**
	*	The basic_socket class template represents a socket descriptor in an object oriented manner. The template parameter
	*	represents the used protocol type and has to match with the basic_protocol<> interface. A basic_socket template instance
	*	cannot be copyed but still moved. By default, the basic_socket<> is on IPv6 mode. However, due to dual stack techniques, it can
	*	communicate with IPv4 peers as well.
	*/
	template <typename Protocol>
	class basic_socket : jodocus::uncopyable, public internal::socket_interface<Protocol>  {
	public:
		/**
		*	Type definition for rvalue reference semantics. However, since VS11 is bugged with RV-references, this type is defined as std::shared_ptr.
		*/
		//typedef std::shared_ptr<basic_socket<Protocol>> ref;		
			
		typedef basic_socket<Protocol>&& ref;

		/**
		*	The constructor creates a new socket object which will be associated with the given listener. Every operation on this socket
		*	will be handled by this listener, therefore the listener must not be destroyed before this sockets destructor is called. Afterwards,
		*	the socket will be bound to a local <i>IPv6</i> interface, if possible. Otherwise, an exception will be thrown.
		*/
		explicit basic_socket(const jodocus::net::listener&);

		/**
		*	The constructor creates a new socket object which will be associated with the given listener. Every operation on this socket
		*	will be handled by this listener, therefore the listener must not be destroyed before this sockets destructor is called. Afterwards,
		*	the socket will be bound to the specified address, if possible. Otherwise, an exception will be thrown.
		*/
		explicit basic_socket(const jodocus::net::listener&, const basic_address<Protocol>&);

		/**
		*	The move constructor creates a new socket basing on the given one. All resources will be transferred to the new socket, therefore the old
		*	one is invalid and should be destroyed if it is not done automatically.
		*/
		explicit basic_socket(basic_socket&&);

		/**
		*	The destructor destroys a socket. Note: if the socket is TCP and connected, the connection won't be gracefully shutdown. Thus
		*	the disconnect()-member function should be called before the socket gets destroyed.
		*/
		~basic_socket();

		/**
		*	The bind() functions <i>binds</i> a socket to a chosen local interface explicitely. The interface address can be obtained by using 
		*	the localhost() function. <i>Note</i> that this function will throw a net_error if the specified socket has been created with a special address
		*	within its constructor. Then it will be allready bound. You only can use this function when the socket has been created without a special
		*	address. <i>Note</i> that you must not bind a socket to do a connect()-call. This will be handled by the library.
		*/
		void bind(const basic_address<Protocol>&) const;

		/**
		*	The listener()-function returns a constant reference to the listener given to the basic_socket in its constructor.
		*/
		const jodocus::net::listener& listener() const;

		/**
		*	The descriptor()-function returns the API socket descriptor to the caller. However, the basic_socket still has the responsibility
		*	to cleanup the socket, therefore it should not be freed or manipulated unless there is a founded reason.
		*/
		native_socket_type descriptor() const;

		/**
		*	This function stops every I/O-operation currently pending on the socket. The operation will fail and the specified callback will be called
		*	with an special error code in a worker thread.
		*/
		bool cancel() const;

	private:
		const jodocus::net::listener& listener_;
	};

	namespace internal {
		struct connection_operation;
	}

	/**
	*	The ip namespace contains type definitions of basic types specialised to internet protocol, like basic_socket or basic_resolver.
	*/
	namespace ip {

		/**
		*	The tcp class contains type definitions of basic types specialised to transmission control protocol, like basic_socket or basic_resolver.
		*	Since it has no members, creating an instance of it is obsolete, but not forbidden, though.
		*/
		class tcp {
		public:
			typedef jodocus::net::tcp<internal::ip> transport_type;
			typedef basic_socket<transport_type> socket;
			typedef basic_address<transport_type> address;
			typedef basic_resolver<transport_type> resolver;

			/**
			*	The localhost()-function returns an iterator to TCP/IPv6 format addresses resolved by "localhost".
			*/
			static resolver::const_iterator localhost(const jodocus::string&);


			/**
			*	The acceptor class coordinates the accept-operations. It provides a simple burst detection and compensates 
			*	bottlenecks dynamically. However, acceptor objects contain unique resources and therefore can't be copyed.
			*/
			class acceptor : jodocus::uncopyable {
			public:
				/**
				*	Creates the acceptor object and the accepting socket which will be associated with the specified listener object and
				*	bound to the given address. The default parameter represents the maximum burst (concurrent incomming connections) the server
				*	can handle. Additionally, an internal observing thread will be started to initiate burst detection.
				*/
				acceptor(const listener&, const address&, std::size_t = 300);

				/**
				*	The destructor destroys the acceptor resources and synchronises with the observing thread.
				*/
				~acceptor();

				/**
				*	The start()-function runs the accepting processes. It's parameter is the callback function which will be called when a new connection has
				*	been established and sent at least one byte of data. The callback function gets the amount of transferred bytes, the error code, the buffer
				*	and the new socket by the framework. The accepting mode can be stoped by calling the stop()-function.
				*/
				void start(std::function<void (unsigned, unsigned, socket::ref, jodocus::buffer)>);

				/**
				*	The stop()-function stops the accepting mode of the acceptor and cancels the accept-operation on the socket. The acceptor recycles its
				*	resources and is ready afterwards for a new call to start()-function. If the acceptor is already stopped, this function has no effect.
				*/
				void stop() const;

				/**
				*	The burst function returns the amount of currently connecting clients right when the function is called. However, in most cases this function
				*	returns 0, but users may safe e.g. the highest messured burst.
				*/
				std::size_t burst() const;

				friend struct listener::dispatcher;

			private:
				typedef std::function<void (unsigned, unsigned, socket::ref, jodocus::buffer)> handler_func;

				void watchdog_();
				void do_accept(std::function<void (unsigned, unsigned, socket::ref, jodocus::buffer)>, std::size_t);

				socket socket_;
				std::vector<std::pair<socket*, internal::connection_operation*>> sockets_;
				handler_func handler_;
				mutable bool running_;
				std::thread watch_thread_;
				event ev_;
				std::atomic_uint burst_count_, pending_accepts_, max_measured_burst_;
				std::mutex mutex_;
			};
		};

		/**
		*	The udp class contains type definitions of basic types specialised to udp, like basic_socket or basic_resolver.
		*	Since it has no members, creating an instance of it is obsolete, but not forbidden, though.
		*/
		class udp {
		public:
			typedef jodocus::net::udp<internal::ip> transport_type;
			typedef basic_socket<transport_type> socket;
			typedef basic_address<transport_type> address;
			typedef basic_resolver<transport_type> resolver;

			/**
			*	The localhost()-function returns an iterator to UDP/IPv6 format addresses resolved by "localhost".
			*/
			static resolver::const_iterator localhost(const jodocus::string&);
		};
	}


	namespace internal {

		struct operation {
			WSAOVERLAPPED overlapped;
			op_type type;

		protected:
			operation(op_type t) 
				: type(t) {
				std::memset(&overlapped, 0, sizeof overlapped);
				overlapped.hEvent = ::WSACreateEvent();
			}
			
			virtual ~operation() { 
				WSACloseEvent(overlapped.hEvent);
			}
		};

		struct io_operation : operation {
			typedef std::function<void (unsigned, unsigned)> handler_func;
			io_operation(handler_func hf)
				: operation(io_op), handler(hf) {				
			}
			handler_func handler;
		};

		struct connection_operation : operation {
			typedef std::function<void (unsigned, unsigned, jodocus::net::ip::tcp::socket::ref, jodocus::buffer)> handler_func;
			connection_operation(handler_func hf, jodocus::net::ip::tcp::acceptor& a, std::size_t i, std::size_t size)
				: operation(sock_op), handler(hf), acceptor(a), buffer(new char[size]), index(i), buffer_size(size) {
				std::memset(buffer, 0, buffer_size);
			}

			~connection_operation() {
				delete [] buffer;
			}
				
			handler_func handler;
			jodocus::net::ip::tcp::acceptor& acceptor;
			char* buffer;
			std::size_t index, buffer_size;
		};

	}

}

JODOCUS_NS_END

#include "src/net_tpl.hpp"