#pragma once

namespace jodocus {
	// Buffer	
	template <typename Type, typename Allocator>
	buffer::buffer(const typename std::vector<Type, Allocator>& vector, typename std::vector<Type, Allocator>::size_type size)
		: storage_(&vector[0]), length_(sizeof(Type) * ((size == 0) ? vector.size() : size))
	{ }

	template <typename Type, std::size_t N>
	buffer::buffer(const typename std::array<Type, N>& array, typename std::array<Type, N>::size_type size)
		: storage_(&array[0]), length_(sizeof(Type) * ((size == 0) ? N : size))
	{ }

	template <typename Ch, typename Traits, typename Allocator>
	buffer::buffer(const typename std::basic_string<Ch, Traits, Allocator>& string, typename std::basic_string<Ch, Traits, Allocator>::size_type size)
		: storage_(string.c_str()), length_(sizeof(Ch) * ((size == 0) ? string.size() : size))
	{ }

}