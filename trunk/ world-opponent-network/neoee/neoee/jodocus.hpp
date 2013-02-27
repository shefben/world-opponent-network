#pragma once
#define WIN32_LEAN_AND_MEAN

#include <cstdint>
#include <string>
#include <sstream>
#include <tchar.h>
#include <Windows.h>
#include "buffer.hpp"
#undef max
#define JODOCUS_NS_BEGIN namespace jodocus {
#define JODOCUS_NS_END }

/**
*	The jodocus namespace contains almost the complete framework, only makros can't be positioned inside them. Since you can find global types
*	(e.g. jodocus::string), there may be conflicts with the std namespace if you use the using directitve (<i>using namespace XX;</i>) to resolve
*	the namespace jodocus and std. You should using-declarations instead.
*/
namespace jodocus {

	typedef std::basic_string<TCHAR> string;
	typedef std::basic_stringstream<TCHAR> stringstream;
	typedef std::basic_ifstream<TCHAR> ifstream;
	typedef std::basic_ofstream<TCHAR> ofstream;
	typedef std::basic_fstream<TCHAR> fstream;
	typedef std::basic_istream<TCHAR> istream;
	typedef std::basic_ostream<TCHAR> ostream;
	typedef std::basic_iostream<TCHAR> iostream;
	typedef TCHAR char_type;
	typedef std::uint8_t byte;
	typedef std::uint16_t word;
	typedef std::uint32_t dword;

	/**
	*	The uncopyable super class provides copy-protection for any class whose objects shall not be copyed by
	*	deriving <i>privately</i> from uncopyable. However, it does not disable any move semantics.
	*/
	class uncopyable {
	protected:
		uncopyable();
		~uncopyable();
	private:
		uncopyable(const uncopyable&);
		uncopyable& operator= (const uncopyable&);
	};

	/**
	*	The system_information class contains basic numbers describing the local machine. It is neither copyable nor
	*	can you directly instantiate an object of it. To use this class, see jodocus::system().
	*/
	struct system_information : private uncopyable {
		unsigned num_cpu;
		unsigned page_size;
		unsigned architecture;

	private:
		friend const system_information& system();
		system_information();
	};

	namespace internal {
		std::string error_fmt(const std::string&, unsigned);
	}

}
