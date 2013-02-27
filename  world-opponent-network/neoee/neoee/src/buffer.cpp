#include "../buffer.hpp"

namespace jodocus {

	buffer::buffer()
		: storage_(0), length_(0)
	{ }

	buffer::buffer(const void* data, std::size_t length)
		: storage_(data), length_(length)
	{ }

	buffer::buffer(const buffer& buffer)
		: storage_(buffer.storage_), length_(buffer.length_)
	{ }

	buffer& buffer::operator =(const buffer& buf) { 
		storage_ = buf.storage_;
		length_ = buf.length_;
		return *this;
	}

	buffer& buffer::operator =(buffer&& buf) {
		storage_ = buf.storage_;
		length_ = buf.length_;
		buf.storage_ = 0;
		buf.length_ = 0;
		return *this;
	}

	std::size_t buffer::size() const {
		return length_;
	}
	
	const void* buffer::storage() const {
		return storage_;
	}

}