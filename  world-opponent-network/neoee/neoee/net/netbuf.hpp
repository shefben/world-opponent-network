#pragma once
#include <sstream>
#include <locale>
#include "../jodocus.hpp"

JODOCUS_NS_BEGIN

namespace net {

	template <typename Ch, typename Traits = std::char_traits<Ch>>
	class basic_codebuf : public std::basic_stringbuf<Ch, Traits> {
		typedef std::codecvt<Ch, char, typename Traits::state_type> cvt_type;
		const cvt_type* cvt_ptr_;
		typename Traits::state_type state_;
	public:
		basic_codebuf(std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out)
			: std::basic_stringbuf<Ch, Traits>(mode), cvt_ptr_(nullptr), state_(0) { }

		basic_codebuf(const std::basic_string<Ch, Traits>& str, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out)
			: std::basic_stringbuf<Ch, Traits>(str, mode), cvt_ptr_(nullptr), state_(0) { }

	private:
		virtual void imbue(const std::locale& loc) {
			if(std::has_facet<cvt_type>(loc)) {
				const cvt_type* facet = &std::use_facet<cvt_type>(loc);
				if(facet->always_noconv())
					cvt_ptr_ = nullptr;
				cvt_ptr_ = facet;
			}
		}

		virtual int_type overflow(int_type c = Traits::eof()) {
			if(cvt_ptr_) {
				char to[2] = { 0, 0 };
				char* to_next = 0;
				Ch from = Traits::to_char_type(c);
				const Ch* from_next = 0;
				typename Traits::state_type state = 0;
				// convert the full put area into itself
				switch(cvt_ptr_->out(state_, pbase(), pptr(), from_next, pbase(), pptr(), to_next)) {
				//case cvt_type::partial: // max_length() > 1, 
				//	cvt_ptr->out(state, &c, &c + 1, from_next, to, to + cvt_ptr->max_length(), to_next);
				//	if(Traits::eq_int_type(this->std::basic_stringbuf<Ch, Traits>::overflow(Traits::to_int_type(*from_next)), Traits::eof()))
				//		return Traits::eof();

				case cvt_type::ok:
					
					if(!sputn(to, 2))
						return Traits::eof();

				case cvt_type::noconv:
					return this->std::basic_stringbuf<Ch, Traits>::overflow(Traits::to_int_type(from));
				case cvt_type::error:
				default:
					return Traits::eof();
				}	
			}
			return this->std::basic_stringbuf<Ch, Traits>::overflow(c);
		}

		virtual int_type underflow() {
			return this->std::basic_stringbuf<Ch, Traits>::underflow();
		}
	};

}

JODOCUS_NS_END