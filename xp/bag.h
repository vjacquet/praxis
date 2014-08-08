#ifndef __BAG_H__
#define __BAG_H__

#pragma warning(push)
#pragma warning(disable:4996)

#include <algorithm>
#include <iterator>
#include <memory>
#include <type_traits>
#include <vector>

#include "memory.h"

#include "fakeconcepts.h"

namespace xp {

	// The bag is similar to std::vector except for the erase function, which takes the last items to fill the gap.
	template<Semiregular T, typename Alloc = std::allocator<T>>
	class bag {
		using AllocTraits = std::allocator_traits<Alloc>;

	public:
		typedef T value_type;
		typedef value_type& reference;
		typedef const value_type& const_reference;

		typedef typename Alloc allocator_type;
		typedef typename std::allocator_traits<allocator_type>::pointer pointer;
		typedef typename std::allocator_traits<allocator_type>::const_pointer const_pointer;
		typedef typename std::allocator_traits<allocator_type>::size_type size_type;
		typedef typename std::allocator_traits<allocator_type>::difference_type difference_type;

		typedef typename std::allocator_traits<allocator_type>::pointer iterator;
		typedef typename std::allocator_traits<allocator_type>::const_pointer const_iterator;
		typedef std::reverse_iterator<iterator> reverse_iterator;
		typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	private:
		pointer start;
		pointer finish;
		pointer end_of_storage;
		allocator_type alloc;

		void bound_check(size_type& n) const {
			if (size() < n) throw std::out_of_range("invalid bag<T> subscript");
		}

		pointer allocate(size_type n) {
			return AllocTraits::allocate(alloc, size_type(n));
		}
		void deallocate(pointer p, size_type n) {
			AllocTraits::deallocate(alloc, p, n);
		}
		template<class... Args>
		void construct(pointer p, Args&&... args) {
			AllocTraits::construct(alloc, p, std::forward<Args>(args)...);
		}
		void destroy(pointer p) {
			AllocTraits::destroy(alloc, p);
		}
		void destroy(pointer first, pointer last) {
			while (first != last) {
				destroy(first);
				++first;
			}
		}

		pointer relocate(pointer f, pointer l, pointer o) {
			o = uninitialized_move_if_noexcept(f, l, o);
			destroy(f, l);
			return o;
		}

		void reallocate(size_type n) {
			auto tmp = AllocTraits::allocate(alloc, size_type(n));
			relocate(start, finish, tmp);
			deallocate(start, capacity());
			finish = tmp + size();
			start = tmp;
			end_of_storage = start + n;
		}

		void grow(size_type n) {
			auto len = size();
			reallocate(len + std::max(len, n));
		}

		void reserve_1_more() {
			if (finish == end_of_storage)
				grow(std::max(size_type(1), size_type(4096 / sizeof(value_type))));
		}
		void reserve_n_more(size_type n) {
			auto avail = end_of_storage - finish;
			if (avail < n)
				grow(n - avail);
		}

		void tidy() {
			destroy(start, finish);
			deallocate(start, capacity());
			end_of_storage = finish = start = nullptr;
		}

		template<InputIterator I>
		bag(I first, I last, const allocator_type& alloc, std::input_iterator_tag) : bag(alloc) {
			while (first != last) {
				insert(*first);
			}
		}

		template<InputIterator I>
		bag(I first, I last, const allocator_type& alloc, std::forward_iterator_tag) : bag(alloc) {
			auto n = std::distance(first, last);
			start = allocate(size_type(n));
			end_of_storage = finish = std::uninitialized_copy_n(first, n, start);
		}

		bag(const bag&& x, const allocator_type& alloc, std::true_type) : start(x.start), finish(x.finish), end_of_storage(x.end_of_storage), alloc(x.alloc) {
			x.end_of_storage = x.finish = x.start = nullptr;
		}
		bag(const bag&& x, const allocator_type& alloc, std::false_type) : alloc(alloc) {
			auto n = x.size();
			start = allocate(size_type(n));
			end_of_storage = finish = std::uninitialized_copy_n(make_move_iterator(x.begin()), n, start);
		}

		void swap(bag& x, std::false_type) {
			std::swap(start, x.start);
			std::swap(finish, x.finish);
			std::swap(end_of_storage, x.end_of_storage);
		}
		void swap(bag& x, std::true_type) {
			swap(x, false_type());
			std::swap(alloc, x.alloc);
		}

		template<InputIterator I>
		void insert(I first, I last, std::input_iterator_tag) {
			while (first != last) {
				insert(*first)
			}
		}

		template<InputIterator I>
		void insert(I first, I last, std::forward_iterator_tag) {
			auto n = std::distance(first, last);
			reserve_n_more(n);
			finish = std::uninitialized_copy_n(first, n, finish);
		}

	public:
		bag() : bag(allocator_type()) {}
		explicit bag(const allocator_type& alloc) : start(nullptr), finish(nullptr), end_of_storage(nullptr), alloc(alloc) {}
		explicit bag(size_type n) : bag(n, T()) {}
		bag(size_type n, const T& val) : bag(n, val, allocator_type()) {}
		bag(size_type n, const T& val, const allocator_type& alloc) : alloc(alloc) {
			start = allocate(n);
			end_of_storage = finish = std::uninitialized_fill_n(start, n, val);
		}
		template<InputIterator I, class Cat = std::enable_if<!std::is_integral<I>::value, typename std::iterator_traits<I>::iterator_category>::type>
		bag(I first, I last)
			: bag(first, last, allocator_type(), Cat()) {}
		template<InputIterator I, class Cat = std::enable_if<!std::is_integral<I>::value, typename std::iterator_traits<I>::iterator_category>::type>
		bag(I first, I last, const allocator_type& alloc)
			: bag(first, last, alloc, Cat()) {}

		bag(const bag& x) : bag(x.begin(), x.end(), alloc.select_on_container_copy_construction(), std::random_access_iterator_tag()) {}
		bag(const bag& x, const allocator_type& alloc) : bag(x.begin(), x.end(), alloc) {}

		bag(const bag&& x) : bag(std::forward<bag>(x), x.alloc, true_type) {}
		bag(const bag&& x, const allocator_type& alloc) : bag(std::forward<bag>(x), alloc, AllocTraits::propagate_on_container_move_assignment()){}

		bag(std::initializer_list<value_type> il)
			: bag(std::begin(il), std::end(il), allocator_type(), std::random_access_iterator_tag()){}
		bag(std::initializer_list<value_type> il, const allocator_type& alloc)
			: bag(std::begin(il), std::end(il), alloc, std::random_access_iterator_tag()) {
		}

		~bag() {
			destroy(start, finish);
			deallocate(start, capacity());
		}

		void clear() {
			destroy(start, finish);
			finish = start;
		}

		bag& operator=(const bag& x) {
			if (&x != this) {
				if (AllocTraits::propagate_on_container_copy_assignment::value && alloc != x.alloc) {
					tidy();
					alloc = x.alloc;
				} else {
					clear();
				}
				insert(x.begin(), x.end());
			}
			return *this;
		}
		bag& operator=(const bag&& x) {
			if (&x != this) {
				if (AllocTraits::propagate_on_container_move_assignment::value && alloc != x.alloc) {
					tidy();
					alloc = x.alloc;
				}
				std::swap(start, x.start);
				std::swap(finish, x.finish);
				std::swap(end_of_storage, x.end_of_storage);
			}
			return *this;
		}
		bag& operator=(std::initializer_list<value_type> il) {
			clear();
			insert(std::begin(il), std::end(il), std::iterator_traits<std::initializer_list<value_type>::iterator>::iterator_category());
		}
		void swap(bag& x) {
			swap(x, typename AllocTraits::propagate_on_container_swap());
		}
		inline friend void swap(bag& x, bag& y) {
			x.swap(y, typename AllocTraits::propagate_on_container_swap());
		}

		// iterators bunch
		iterator begin() noexcept {return start;}
		const_iterator begin() const noexcept {return start;}
		iterator end() noexcept {return finish;}
		const_iterator end() const noexcept {return finish;}
		const_iterator cbegin() const noexcept {return start;}
		const_iterator cend() const noexcept {return finish;}
		reverse_iterator rbegin() noexcept {return reverse_iterator(end());}
		const_reverse_iterator rbegin() const noexcept {return const_reverse_iterator(end()); }
		reverse_iterator rend() noexcept {return reverse_iterator(begin());}
		const_reverse_iterator rend() const noexcept {return const_reverse_iterator(begin()); }
		const_reverse_iterator crbegin() const noexcept {return const_reverse_iterator(end());}
		const_reverse_iterator crend() const noexcept {return const_reverse_iterator(begin());}

		// accessors and mutators
		size_type size() const noexcept {return size_type(finish - start); }
		size_type max_size() const noexcept {return AllocTraits::max_size(alloc); }
		size_type capacity() const noexcept {return size_type(end_of_storage - start);}
		bool empty() const noexcept {return start == finish;}

		reference operator[](size_type n) { return *(start + n); }
		const_reference operator[](size_type n) const { return *(start + n); }

		reference at(size_type n) { bound_check(n); return *(start + n); }
		const_reference at(size_type n) const { bound_check(n); return *(start + n); }

		pointer data() noexcept {return start;}
		const_pointer data() const noexcept {return start;}

		template<class... Args>
		void emplace(Args&&... args) {
			reserve_1_more();
			construct(finish, std::forward<Args>(args)...);
			++finish;
		}

		void insert(const value_type& val) {
			reserve_1_more();
			construct(finish, val);
			++finish;
		}
		void insert(value_type&& val) {
			reserve_1_more();
			construct(finish, std::forward<value_type>(val));
			++finish;
		}
		template<InputIterator I, class Cat = std::enable_if<!std::is_integral<I>, typename std::iterator_traits<I>::iterator_category>>
		void insert(I first, I last) {
			insert(first, last, std::iterator_traits<I>::iterator_category())
		}

		iterator erase(const_iterator pos) {
			--finish;
			if (finish != pos) {
				*pos = std::move_if_noexcept(*finish);
			}
			destroy(finish);
			return pos;
		}
		size_type erase(const value_type& val) {
			auto f = finish;
			auto i = start;
			while (i != finish) {
				if (*i == val)
					erase(i);
				else
					++i;
			}
			return size_type(f - finish);
		}
		iterator erase(const_iterator first, const_iterator last) {
			pointer f1 = const_cast<pointer>(first);
			pointer l1 = const_cast<pointer>(last);

			pointer f2 = finish - (last - first);
			pointer l2 = finish;

			if (f2 < l1) {
				finish = relocate(l1, l2, f1);
			} else {
				relocate(f2, l2, f1);
				finish = f2;
			}
			return f1;
		}

		void reserve(size_type n) {
			if (capacity() < n) {
				reallocate(n);
			}
		}
		void resize(size_type n) { resize(n, value_type()); }
		void resize(size_type n, const value_type& val) {
			reallocate(n);
			if (n > size()) {
				finish = std::uninitialized_fill_n(finish, n, val);
			}
		}
		void shrink_to_fit() {
			reallocate(size());
		}

		allocator_type get_allocator() const noexcept {
			return alloc;
		}

		// methods required for stack but, except pop_back,
		// they are merely wrappers around existing methods.
		// moreover, the notion of front, back, push_back or even pop_back 
		// are meaningless in regard of the notion of bag. 
		// adobe defines three storage categories block, continuous and node.
		// So may be there is a difference between storage and container, 
		// a separation of concerns. This would also address the following issue:
		// shouldn't set, multiset, map, multimap be adaptors ? The storage could be
		// balanced binary trees, vectors or hashtable? 
		reference front() { return (*this)[0]; }
		const_reference front() const { return (*this)[0]; }

		reference back() { return (*this)[size() - 1]; }
		const_reference back() const { return (*this)[size() - 1]; }

		template<class... Args>
		void emplace_back(Args&&... args) {
			emplace(std::forward<Args>(args)...);
		}

		void push_back(const value_type& val) {
			insert(val);
		}
		void push_back(value_type&& val) {
			insert(std::forward<value_type>(val));
		}

		void pop_back() {
			destroy(--finish);
		}

	};

} // namespace xp

#pragma warning(pop)

#endif __BAG_H__