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
		// equivalent to uninitialized_copy(make_move_iterator(f), make_move_iterator(l), o);
		while (f != l) {
			construct(std::addressof(*o), std::move(*f));
			++f;
			++o;
		}
		return o;
	}

	template<InputIterator I, ForwardIterator O>
	O uninitialized_move_if_noexcept(I f, I l, O o)
	{
		while (f != l) {
			construct(std::addressof(*o), std::move_if_noexcept(*f));
			++f;
			++o;
		}
		return o;
	}

	template<InputIterator I, typename N, ForwardIterator O>
	O uninitialized_move_n(I f, N n, O o)
	{
		// equivalent to uninitialized_copy_n(make_move_iterator(f), n, o);
		for (; 0 < n; --n, ++f, ++o) {
			construct(std::addressof(*o), std::move(*f));
		}
		return o;
	}

	template<InputIterator I, typename N, ForwardIterator O>
	O uninitialized_move_if_noexcept_n(I f, N n, O o)
	{
		for (; 0 < n; --n, ++f, ++o) {
			construct(std::addressof(*o), std::move_if_noexcept(*f));
		}
		return o;
	}

} // namespace xp

#endif __NUMERIC_H__