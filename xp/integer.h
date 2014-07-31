#ifndef __INTEGER_H__
#define __INTEGER_H__

#include <cstdint>
#include <cstdlib>

#include <algorithm>
#include <exception>
#include <utility>

namespace xp {

	namespace details {

		struct storage_t {
			typedef unsigned* pointer;
			typedef size_t size_type;

			unsigned* p;

			int sign() const {
				if (p == nullptr) return 0;
				return 1 - ((*p >> 30) & 2);
			}
			size_type size() const {
				if (p == nullptr) return 0;
				return *p & 0x7FFFFFFF;
			}

			pointer allocate(size_type n) {
				void* ptr = malloc(sizeof(unsigned) * n);
				if (ptr == nullptr) throw std::bad_alloc();
				return pointer(ptr);
			}

			void resize(size_type n) {
				auto sz = size();
				if (sz == n)
					return;
				auto tmp = allocate(n);
				if (sz == 0) {
					memset(tmp, 0, sizeof(unsigned) *n);
				} else if (sz < n) {
					memcpy(tmp, p, sizeof(unsigned*) * sz);
					memset(tmp + sz, 0, n - sz);
				} else {
					memcpy(tmp, p, sizeof(unsigned*) * n);
				}
				std::swap(p, tmp);
				free(tmp);
			}

			void grow(size_type n) {
				resize(n + size());
			}

			void deallocate() {
				void* tmp = p;
				p = nullptr;
				free(tmp);
			}

			storage_t() : p(nullptr) {}
			storage_t(const storage_t& x) : p(nullptr) {
				auto n = x.size();
				if (n) {
					p = allocate(n);
					memcpy(p, x.p, sizeof(unsigned) * n);
				}
			}
			~storage_t() { deallocate(); }

			friend void swap(storage_t& x, storage_t& y) {
				std::swap(x.p, y.p);
			}
			storage_t& operator=(const storage_t& x) {
				if (&x != this) {
					storage_t tmp(x);
					swap(*this, tmp);
				}
				return *this;
			}

			unsigned* header() { return p; }
			unsigned* data() { return p + ptrdiff_t(1); }
			const unsigned* data() const { return p + ptrdiff_t(1); }
		};
	}

	class integer {
		details::storage_t storage;

		static bool unsigned_less(const integer& x, const integer& y) {
			auto nx = x.storage.size();
			auto ny = y.storage.size();
			if (nx < ny)
				return true;
			if (ny < nx)
				return false;
			unsigned vx;
			unsigned vy;
			for (unsigned i = nx; i > 0; ++i) {
				vx = *(x.storage.p + i);
				vy = *(y.storage.p + i);
				if (vx < vy)
					return true;
				if (vy < vx)
					return false;
			}
			return false;
		}

	public:
		integer() {}
		integer(const integer& x) : storage(x.storage) {}
		~integer() {}

		friend void swap(integer& x, integer& y) {
			std::swap(x.storage, y.storage);
		}
		integer& operator=(const integer& x) {
			if (&x != this) {
				integer tmp(x);
				swap(*this, tmp);
			}
			return *this;
		}

		integer(int x) : storage() {
			if (x > 0) {
				storage.resize(1);
				*storage.header() = 1;
				*storage.data() = x;
			} else if (x < 0) {
				storage.resize(1);
				*storage.header() = 1 | 0xC0000000;
				*storage.data() = -x;
			}
		}

		integer& operator=(int x) {
			integer tmp(x);
			swap(*this, tmp);
			return *this;
		}

		inline friend int sign(integer i) {
			return i.storage.sign();
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
				return unsigned_less(y, x);
			} else if (sy < 0) {
				return false;
			}
			return unsigned_less(x, y);
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

	};


}

#endif __INTEGER_H__