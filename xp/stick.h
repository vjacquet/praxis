#ifndef __STICK_H__
#define __STICK_H__

#include <iostream>
#include <memory>
#include <string>

namespace xp {

	// a stick is a mutable window on a const string.
	template<class T, class Traits = std::char_traits<T>, class Alloc = std::allocator<T>>
	class basic_stick {
		typedef std::basic_string<T, Traits, Alloc>   storage_type;

		std::shared_ptr<const storage_type> ptr;
		size_t pos;
		size_t count;

	public:
		typedef T                                     value_type;
		typedef Traits                                traits_type;
		typedef typename storage_type::allocator_type allocator_type;

		typedef const value_type*                     const_pointer;
		typedef const value_type&                     const_reference;
		typedef size_t                                size_type;
		typedef ptrdiff_t                             difference_type;

		typedef const value_type*                     const_iterator;
		typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

		// Constructors and assignments.
		basic_stick(const Alloc& a = Alloc())
			: ptr(std::make_shared<const storage_type>(a)), pos(0), count(0)
		{}
		basic_stick(T* s, size_type count, const Alloc& a = Alloc())
			: ptr(std::make_shared<const storage_type>(s, count, a)), pos(0), count(count)
		{}
		basic_stick(T* s, const Alloc& a = Alloc())
			: ptr(std::make_shared<const storage_type>(s, a)), pos(0), count(ptr->size())
		{}
		basic_stick(storage_type&& s)
			: ptr(std::make_shared<const storage_type>(move(s))), pos(0), count(ptr->size())
		{}
		basic_stick(storage_type&& s, const Alloc& a)
			: ptr(std::make_shared<const storage_type>(s, a)), pos(0), count(ptr->size())
		{}

		basic_stick(const basic_stick& x)
			: ptr(x.ptr), pos(x.pos), count(x.count)
		{}
		basic_stick(const basic_stick& x, size_type pos, size_type count = storage_type::npos)
			: ptr(x.ptr), pos(x.pos + pos), count(count == storage_type::npos ? x.count : count)
		{}
		basic_stick(const basic_stick&& x)
			: ptr(x.ptr), pos(x.pos), count(x.count)
		{}

		void swap(basic_stick& x) {
			ptr.swap(x.ptr);
			swap(pos, x.pos);
			swap(count, x.count);
		}

		// Iterators.
		const_iterator begin() const {
			return ptr->data() + pos;
		}
		const_iterator end() const {
			return ptr->data() + pos + count;
		}
		const_iterator cbegin() const {
			return ptr->data() + pos;
		}
		const_iterator cend() const {
			return ptr->data() + pos + count;
		}

		const_reverse_iterator rbegin() const {
			return const_reverse_iterator(ptr->data() + pos + count);
		}
		const_reverse_iterator rend() const {
			return const_reverse_iterator(ptr->data() + pos);
		}

		// Size and other properties.
		size_type size() const {
			return count;
		}
		size_type length() const {return size();}
			bool empty() const {
			return count == 0;
		}

		long use_count() const { return ptr.use_count(); }
		const T* data() const { return (*ptr) + pos; }

		allocator_type get_allocator() const { return ptr->get_allocator(); }

		// Element access.
		const_reference operator[](size_type n) const {
			return (*ptr)[pos + n];
		}
		const_reference at(size_type n) const {
			if (n >= size()) throw std::out_of_range("basic_stick");
			return ptr->at(pos + n);
		}

		// Mutators.
		basic_stick slice(size_type pos, size_type count = storage_type::npos) {
			return basic_stick(*this, pos, min(this->count - pos, count));
		}
		basic_stick slice(const_iterator begin, const_iterator end) {
			return basic_stick(*this, begin - begin(), end - begin);
		}

		void pop_front() {
			++pos;
		}
		void pop_back() {
			--count;
		}

		// Comparisons.
		int compare(const basic_stick& str) const {
			return ptr->compare(pos, count, *str.ptr, str.pos, str.count);
		}
		int compare(size_type pos1, size_type count1, const basic_stick& str) const {
			return ptr->compare(pos + pos1, count1, *str.ptr, str.pos, str.count);
		}
		int compare(size_type pos1, size_type count1, const basic_stick& str, size_type pos2, size_type count2) const {
			return ptr->compare(pos + pos1, count1, *str.ptr, str.pos + str.pos2, count2);
		}
		int compare(const T* s) const {
			return ptr->compare(pos, count, s);
		}
			int compare(size_type pos1, size_type count1, const T* s) const {
			return ptr->compare(pos + pos1, count1, s);
		}
		int compare(size_type pos1, size_type count1, const T* s, size_type count2) const {
			return ptr->compare(pos + pos1, count1, s, count2);
		}

		// semiregular
		friend bool operator==(const basic_stick& x, const basic_stick& y) {
			return x.compare(y) == 0;
		}

		friend bool operator==(const T* s, const basic_stick& y) {
			return y.compare(s) == 0;
		}

		friend bool operator==(const basic_stick& x, const T* s) {
			return x.compare(s) == 0;
		}

		friend bool operator!=(const basic_stick& x, const basic_stick& y) {
			return !(x == y);
		}

		friend bool operator!=(const T* s, const basic_stick& y) {
			return !(s == y);
		}

		friend bool operator!=(const basic_stick& x, const T* s) {
			return !(x == s);
		}

		// regular
		friend bool operator<(const basic_stick& x, const basic_stick& y) {
			return x.compare(y) < 0;
		}

		friend bool operator<(const T* s, const basic_stick& y) {
			return y.compare(s) > 0;
		}

		friend bool operator<(const basic_stick& x, const T* s) {
			return x.compare(s) < 0;
		}

		friend bool operator>(const basic_stick& x, const basic_stick& y) {
			return y < x;
		}

		friend bool operator>(const T* s, const basic_stick& y) {
			return y < s;
		}

		friend bool operator>(const basic_stick& x, const T* s) {
			return s < x;
		}

		friend bool operator<=(const basic_stick& x, const basic_stick& y) {
			return !(y < x);
		}

		friend bool operator<=(const T* s, const basic_stick& y) {
			return !(y < s);
		}

		friend bool operator<=(const basic_stick& x, const T* s) {
			return !(s < x);
		}

		friend bool operator>=(const basic_stick& x, const basic_stick& y) {
			return !(x < y);
		}

		friend bool operator>=(const T* s, const basic_stick& y) {
			return !(s < y);
		}

		friend bool operator>=(const basic_stick& x, const T* s) {
			return !(x < s);
		}
	};

	// I/O.
	template <class T, class Traits, class Alloc>
	inline std::basic_ostream<T, Traits>& operator<<(std::basic_ostream<T, Traits>& os, const basic_stick<T, Traits, Alloc>& s)
	{
		using namespace std;

		typename basic_ostream<T, Traits>::sentry sentry(os);
		if (!sentry)
			os.setstate(ios_base::failbit);

		const bool left = (os.flags() & ios::left) != 0;
		size_t n = s.size();
		const streamsize w = os.width(0);
		const streamsize pad_len = (w != 0 && n < w) ? w - n : 0;

		ostream_iterator<T> oi(os);
		if (!left) oi = fill_n(oi, pad_len, os.fill());
		oi = copy(s.begin(), s.end(), oi);
		if (left) oi = fill_n(oi, pad_len, os.fill());

		return os;
	}

	// Typedefs.
	typedef basic_stick<char> stick;
	typedef basic_stick<wchar_t> wstick;
	typedef basic_stick<char16_t> u16stick;
	typedef basic_stick<char32_t> u32stick;

}

#endif __STICK_H__