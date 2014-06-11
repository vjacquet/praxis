#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <algorithm>
#include <memory>
#include "fakeconcepts.h"

namespace xp {

	template <Semiregular T>
	void destroy(T* p) {
		p->~T();
	}

	template<Semiregular T>
	void construct(T* p) {
		::new (static_cast<void*>(p)) T();
	}

	template<Semiregular T>
	void construct(T* p, T&& v) {
		::new (static_cast<void*>(p)) T(std::forward<T>(v));
	}

	template<ForwardIterator I>
	void destroy(I f, I l) {
		while (f != l) {
			destroy(std::addressof(*f));
			++f;
		}
	}

	template<InputIterator I, ForwardIterator O>
	O uninitialized_move(I f, I l, O o)
	{
		while (f != l) {
			construct(std::addressof(*o), std::move(*f));
			++f;
			++o;
		}
		return o;
	}

} // namespace xp

#endif __NUMERIC_H__