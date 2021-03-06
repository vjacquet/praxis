#ifndef __HEAP_H__
#define __HEAP_H__

#include <algorithm>
#include <functional>
#include <utility>
#include <vector>
#include <queue>

#include "fakeconcepts.h"

namespace xp {

	// The heap does not have ==, !=, <, >, <=, >= operators because the items might pop in the same order 
	// without being stored in the same order. So it would require poping items to compare two heaps.
	template<Semiregular T, Predicate Pred = std::less<T>, typename Cont = std::vector<T>>
	class heap {
		Cont c;
		Pred p;

	public:
		typedef Cont container_type;
		typedef Pred key_compare;
		typedef typename Cont::value_type value_type;
		typedef typename Cont::size_type size_type;
		typedef typename Cont::reference reference;
		typedef typename Cont::const_reference const_reference;

		heap() : c(), p() {}
		heap(const heap& x) : c(x.c), p(x.p) {}
		explicit heap(const Cont& c) : c(c), p() {}
		explicit heap(const key_compare& p) : c(), p(p) {}

		heap& operator=(const heap& x) {
			c = x.c;
			return *this;
		}

		template<class Alloc, class = typename std::enable_if<std::uses_allocator<Cont, Alloc>::value, void>::type>
		explicit heap(const Alloc& a) : c(a), p() {}
		template<class Alloc, class = typename std::enable_if<std::uses_allocator<Cont, Alloc>::value, void>::type>
		heap(const key_compare& p, const Alloc& a) : c(a), p(p) {}
		template<class Alloc, class = typename std::enable_if<std::uses_allocator<Cont, Alloc>::value, void>::type>
		heap(const heap& x, const Alloc& a) : c(x.c, a), p(x.p) {}
		template<class Alloc, class = typename std::enable_if<std::uses_allocator<Cont, Alloc>::value, void>::type>
		heap(const Cont& c, const Alloc& a) : c(c, a), p() {}
		template<class Alloc, class = typename std::enable_if<std::uses_allocator<Cont, Alloc>::value, void>::type>
		heap(const key_compare& p, const Cont& c, const Alloc& a) : c(c, a), p(p) {}

		// TODO: add noexcept when implemented in VS :(
		heap(heap&& x) : c(std::move(x.c)), p(std::move(x.p)) {}
		explicit heap(Cont&& c) : c(std::move(c)), p() {}
		template<class Alloc, class = typename std::enable_if<std::uses_allocator<Cont, Alloc>::value, void>::type>
		heap(heap&& x, const Alloc& a) : c(std::move(x.c), a), p(std::move(x.p)) {}
		template<class Alloc, class = typename std::enable_if<std::uses_allocator<Cont, Alloc>::value, void>::type>
		heap(Cont&& c, const Alloc& a) : c(std::move(c), a), p() {}
		heap& operator=(heap&& x) {
			c = std::move(x.c);
			p = std::move(x.p);
			return *this;
		}

		void push(const value_type& x) {
			c.push_back(x);
			std::push_heap(begin(c), end(c), p);
		}
		void push(value_type&& x) {
			c.push_back(std::move(x));
			std::push_heap(begin(c), end(c), p);
		}
		template<class... Args>
		void emplace(Args&&... args) {
			c.emplace_back(std::forward<Args>(args)...);
			std::push_heap(begin(c), end(c), p);
		}

		bool empty() const {
			return c.empty();
		}
		size_type size() const {
			return c.size();
		}
		reference top() {
			return c.front();
		}
		const_reference top() const {
			return c.front();
		}
		void pop() {
			std::pop_heap(begin(c), end(c), p);
			c.pop_back();
		}

		void swap(heap& x) {
			swap(c, x.c);
			swap(p, x.p);
		}

		// 
		//inline friend bool operator==(const heap& x, const heap& y) {
		//	// is it true ? the items might pop in the same order without being stored in the same order...
		//	return x.c == y.c;
		//}
		//inline friend bool operator!=(const heap& x, const heap& y) {
		//	return !(x == y);
		//}
		//inline friend bool operator<(const heap& x, const heap& y) {
		//	return x.c < y.c;
		//}
		//inline friend bool operator <=(const heap& x, const heap& y) {
		//	return !(y < x);
		//}
		//inline friend bool operator >(const heap& x, const heap& y) {
		//	return y < x;
		//}
		//inline friend bool operator >=(const heap& x, const heap& y) {
		//	return !(x < y);
		//}
	};

}

#endif __HEAP_H__