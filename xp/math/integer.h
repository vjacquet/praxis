#ifndef __INTEGER_H__
#define __INTEGER_H__

#include <cstdint>
#include <cstdlib>

#include <algorithm>
#include <exception>
#include <memory>
#include <utility>

#include "../algorithm.h"
#include "../utility.h"

// /!\ work in progress
// - implement storage
// - implement natural (include 0 ? see http://en.wikipedia.org/wiki/Natural_number)
// - finaly, implement integer...

namespace xp {

	namespace details {
		template<typename T>
		std::pair<T, T> divide(const T& num, const T& denom) {
			return {num / denom, num % denom};
		}


		// layout 
		//   sign : 1
		//   capacity : 15
		//   carry : 1
		//   size : 15
		struct integer_storage_t {
			typedef std::allocator<unsigned> allocator_type;

			typedef unsigned* pointer;
			typedef const unsigned* const_pointer;
			typedef unsigned& reference;
			typedef const unsigned& const_reference;
			typedef std::size_t size_type;
			typedef std::ptrdiff_t difference_type;

			struct header_t {
				unsigned sign : 1;
				unsigned size : 15;
				unsigned capacity : 15;
				unsigned carry : 1;
			};

			//class iterator : public std::iterator<std::forward_iterator_tag, unsigned> {
			//	pointer position;

			//public:
			//	iterator() :position(nullptr) {}
			//	iterator(pointer p) : position(p) {}
			//	iterator(const iterator& x) = default;
			//	iterator& operator=(const iterator& x) = default;

			//	// dereference
			//	reference operator*() const { return *position; }
			//	pointer operator->() const { return &(**this); }

			//	// forward
			//	iterator& operator++() {
			//		++position;
			//		return *this;
			//	}
			//	iterator operator++(int) {
			//		iterator i = *this;
			//		++*this;
			//		return i;
			//	}

			//	// Regular
			//	friend bool operator==(const iterator& x, const iterator& y) {
			//		return x.position == y.position;
			//	}
			//	friend bool operator!=(const iterator& x, const iterator& y) { return !(x == y); }
			//};
			typedef unsigned* iterator;
			typedef const iterator const_iterator;

			//class reverse_iterator : public std::iterator<std::forward_iterator_tag, unsigned> {
			//	pointer position;

			//public:
			//	reverse_iterator() :position(nullptr) {}
			//	reverse_iterator(pointer p) : position(p) {}
			//	reverse_iterator(const reverse_iterator& x) = default;
			//	reverse_iterator& operator=(const reverse_iterator& x) = default;

			//	// dereference
			//	reference operator*() const { return *position; }
			//	pointer operator->() const { return &(**this); }

			//	// forward
			//	reverse_iterator& operator++() {
			//		--position;
			//		return *this;
			//	}
			//	reverse_iterator operator++(int) {
			//		reverse_iterator i = *this;
			//		++*this;
			//		return i;
			//	}

			//	// Regular
			//	friend bool operator==(const reverse_iterator& x, const reverse_iterator& y) {
			//		return x.position == y.position;
			//	}
			//	friend bool operator!=(const reverse_iterator& x, const reverse_iterator& y) { return !(x == y); }
			//}; 
			typedef std::reverse_iterator<iterator> reverse_iterator;
			typedef const reverse_iterator const_reverse_iterator;

			pointer p;

			header_t* header() { return reinterpret_cast<header_t*>(p); }
			const header_t* header() const { return reinterpret_cast<const header_t*>(p); }

			size_type unguarded_capacity() const { return header()->capacity; }
			size_type unguarded_size() const { return header()->size; }
			int unguarded_sign() const { return (1 - ((*p >> 30) & 2)); }
			bool unguarded_carry() const { return header()->carry; }

			size_type capacity() const { return p ? unguarded_capacity() : 0; }
			size_type size() const { return p ? unguarded_size() : 0; }
			int sign() const{ return p ? unguarded_sign() : 0; }
			bool carry() const { return p ? unguarded_carry() : false; }
			bool empty() const { return p == nullptr; }

			pointer allocate(size_type n) {
				auto ptr = reinterpret_cast<pointer>(std::malloc((n + 1) * sizeof(unsigned)));
				if (ptr == nullptr) throw std::bad_alloc();
				return reinterpret_cast<pointer>(ptr);
			}
			pointer reallocate(pointer p, size_type /*n*/, size_type request) {
				auto ptr = reinterpret_cast<pointer>(std::realloc(p, sizeof(unsigned) * (request + 1)));
				if (ptr == nullptr) throw std::bad_alloc();
				return reinterpret_cast<pointer>(ptr);
			}
			pointer deallocate(pointer p, size_type /*n*/) {
				free(p);
			}
			void deallocate() {
				void* tmp = p;
				p = nullptr;
				free(tmp);
			}

			void reserve(size_type additional = 1) {
				if (!p) {
					p = allocate(additional);
					header()->capacity = additional;
				} else {
					auto cap = unguarded_capacity();
					auto n = unguarded_size() + additional;
					if (cap < n) {
						p = reallocate(p, cap, n);
						header()->capacity = n;
					}
				}
			}

			integer_storage_t() : p(nullptr) {}
			integer_storage_t(const integer_storage_t& x) : p(nullptr) {
				auto n = x.size();
				if (n) {
					p = allocate(n);
					memcpy(p, x.p, sizeof(unsigned) * n);
				}
			}
			~integer_storage_t() { deallocate(); }

			friend void swap(integer_storage_t& x, integer_storage_t& y) {
				std::swap(x.p, y.p);
			}
			integer_storage_t& operator=(const integer_storage_t& x) {
				if (&x != this) {
					integer_storage_t tmp(x);
					std::swap(p, tmp.p);
				}
				return *this;
			}

			unsigned* data() { return p + ptrdiff_t(1); }
			const unsigned* data() const { return p + ptrdiff_t(1); }

			void push(unsigned x) {
				reserve();
				p[++header()->size] = x;
			}
			void pop() {
				if (size() > 1) --(header()->size);
				else deallocate();
			}

			reference front() { return *data(); }
			const_reference front() const { return *data(); }
			reference back() { return *(p + unguarded_size()); }
			const_reference back() const { return *(p + unguarded_size()); }

			iterator begin() { return p + ptrdiff_t(1); }
			const_iterator begin() const { return p + ptrdiff_t(1); }
			const_iterator cbegin() const { return begin(); }

			iterator end() { return p + ptrdiff_t(1) + size(); }
			const_iterator end() const { return p + ptrdiff_t(1) + size(); }
			const_iterator cend() const { return end(); }

			reverse_iterator rbegin() { return reverse_iterator(p ? p + unguarded_size() : nullptr); }
			const_reverse_iterator rbegin() const { return reverse_iterator(p ? p + unguarded_size() : nullptr); }
			const_reverse_iterator crbegin() const { return rbegin(); }

			reverse_iterator rend() { return reverse_iterator(p); }
			const_reverse_iterator rend() const { return reverse_iterator(p); }
			const_reverse_iterator crend() const { return rend(); }
		};

		struct lshift {
			int shift;
			unsigned carry;

			lshift(int shift) : shift(shift){}

			unsigned operator()(unsigned x) {
				unsigned r = (x << shift) | carry;
				carry = x >> (sizeof(unsigned)*8 - shift);
				return r;
			}
		};
		struct rshift {
			int shift;
			unsigned carry;

			rshift(int shift) : shift(shift){}

			unsigned operator()(unsigned x) {
				unsigned r = (x >> shift) | carry;
				carry = x << (sizeof(unsigned) * 8 - shift);
				return r;
			}
		};
		struct addition {
			unsigned carry;

			addition() {}

			unsigned operator()(unsigned x, unsigned y) {
				unsigned r = x + y;
				if (r < x) {
					r += carry;
					carry = 1;
				} else {
					r += carry;
					carry = 0;
				}
				return r;
			}
		};
	} // namespace details

	// >= 0
	class natural {
	protected:
		details::integer_storage_t storage;

		void twice() {
			if (!storage.empty()) {
				auto op = details::lshift {1};
				transform_backward(storage.cbegin(), storage.cend(), storage.cend(), std::ref(op));
				if (op.carry)
					storage.push(1);
			}
		}

	public:
		friend void swap(natural& x, natural& y) {
			std::swap(x.storage, y.storage);
		}

		natural() : storage() {}
		~natural() {}

		natural(const natural& x) : storage(x.storage) {}
		natural& operator=(const natural& x) {
			if (&x != this) {
				natural tmp(x);
				swap(*this, tmp);
			}
			return *this;
		}

		// conversion
		natural(int x) : storage() {
			if (x) {
				storage.push(x);
			}
		}

		friend int sign(const natural& i) {
			if (i.storage.empty())
				return 0;
			return 1;
		}

		// semiregular
		friend bool operator==(const natural& x, const natural& y) {
			auto n = x.storage.size();
			if (n != y.storage.size())
				return false;
			return std::equal(x.storage.cbegin(), x.storage.cend(), y.storage.cbegin());
		}
		friend bool operator!=(const natural& x, const natural& y) { return !(x == y); }

		// totally orderd
		friend bool operator<(const natural& x, const natural& y) {
			auto nx = x.storage.size();
			auto ny = y.storage.size();
			if (nx < ny)
				return true;
			if (ny < nx)
				return false;
			return std::equal(x.storage.crbegin(), x.storage.crend(), y.storage.crbegin(), std::less<unsigned> {});
		}
		friend bool operator <=(const natural& x, const natural& y) { return !(y < x); }
		friend bool operator >(const natural& x, const natural& y) { return y < x; }
		friend bool operator >=(const natural& x, const natural& y) { return !(x < y); }

		// shifts
		natural& operator<<=(int x) {
			if (!storage.empty()) {
				int q = x / (sizeof(unsigned) * 8);
				int r = x % (sizeof(unsigned) * 8);
				if (bits(storage.back(), sizeof(unsigned) * 8 - r, r))
					++q;
				storage.reserve(q);
				auto op = details::lshift {r};
				auto e = transform_backward(storage.cbegin(), storage.cend(), storage.end() + q, std::ref(op));
				storage.header()->size += q;
				if (q && op.carry)
					*e-- = op.carry;
				std::fill(storage.begin(), e, 0);
			}
			return *this;
		}
		natural operator <<(unsigned x) {
			natural tmp(*this);
			tmp <<= x;
			return tmp;
		}
		natural& operator>>=(unsigned x) {
			auto q = x / (sizeof(unsigned) * 8);
			if (!storage.empty()) {
				if (storage.unguarded_size() < q)

				auto r = x % (sizeof(unsigned) * 8);
			} else {
				storage.deallocate();
			}
			return *this;
		}
		natural operator >>(unsigned x) {
			natural tmp(*this);
			tmp >>= x;
			return tmp;
		}

		// arithmetics
		natural& operator+=(const natural& x) {
			if (x.storage.empty())
				return *this;
			if (storage.empty())
				return *this = x;
			if (&x == this) {
				twice();
			} else {
				auto op = details::addition {};
				transform(storage.cbegin(), storage.cend(), x.storage.cbegin(), storage.begin(), std::ref(op));
				if (op.carry)
					storage.push(1);
			}
			return *this;
		}
		inline friend natural operator +(const natural& x, const natural& y) {
			natural tmp(x);
			tmp += y;
			return tmp;
		}
	};

	class integer : natural {
	public:
		integer() : natural() {}
		~integer() {}

		integer(const integer& x) : natural(x) {}
		integer& operator=(const integer& x) {
			if (&x != this) {
				integer tmp(x);
				swap(*this, tmp);
			}
			return *this;
		}

		// conversion
		integer(int x) : natural(abs(x)) {
			if (x < 0) {
				auto h = storage.header()->sign = 1;
			}
		}

		// regular
		inline friend bool operator==(const integer& x, const integer& y) {
			auto n = x.storage.size();
			if (n != y.storage.size() || x.storage.sign() != y.storage.sign())
				return false;
			for (unsigned i = 1; i <= n; ++i)
				if (*(x.storage.p + i) != *(y.storage.p + i))
					return false;
			return true;
		}
		inline friend bool operator!=(const integer& x, const integer& y) {
			return !(x == y);
		}

		// totally ordered
		inline friend bool operator<(const integer& x, const integer& y) {
			auto sx = sign(x);
			auto sy = sign(y);
			if (sx < 0) {
				if (sy >= 0)
					return true;
				return operator <(static_cast<const natural&>(y), static_cast<const natural&>(x));
			} else if (sy < 0) {
				return false;
			}
			return operator <(static_cast<const natural&>(x), static_cast<const natural&>(y));
		}
		inline friend bool operator <=(const integer& x, const integer& y) {
			return !(y < x);
		}
		inline friend bool operator >(const integer& x, const integer& y) {
			return y < x;
		}
		inline friend bool operator >=(const integer& x, const integer& y) {
			return !(x < y);
		}

		// arithmetic
		integer& operator+=(integer x) {
			integer& y = *this;

			auto sx = sign(x);
			auto sy = sign(y);

			if (sx == sy) {
				// similar to unsigned addition
			} else {
				// equivalent to substraction
			}

			//swap(*this, tmp);
			return *this;
		}
		inline friend integer operator +(const integer& x, const integer& y) {
			integer tmp(x);
			tmp += y;
			return std::move(tmp);
		}

		friend int sign(const integer& i) {
			return i.storage.sign();
		}
	};
}

#endif __INTEGER_H__