#pragma once
#include <array>
#include <vector>
#include <string>

namespace jodocus {
	
	struct buffer {
		buffer();
		buffer(const void*, std::size_t);
		buffer(const buffer&);
		
		template <typename Type, typename Allocator>
		explicit buffer(const typename std::vector<Type, Allocator>&, typename std::vector<Type, Allocator>::size_type = 0);

		template <typename Type, std::size_t N>
		explicit buffer(const typename std::array<Type, N>&, typename std::array<Type, N>::size_type = 0);

		template <typename Ch, typename Traits, typename Allocator>
		explicit buffer(const typename std::basic_string<Ch, Traits, Allocator>&, typename std::basic_string<Ch, Traits, Allocator>::size_type = 0);

		buffer& operator =(const buffer&);
		buffer& operator =(buffer&&);
		
		std::size_t size() const;
		const void* storage() const;

	private:
		const void* storage_;
		std::size_t length_;
	};


}

#include "src/buffer_tpl.hpp"