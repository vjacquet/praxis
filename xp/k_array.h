#ifndef __K_ARRAY_H__
#define __K_ARRAY_H__

#include <memory>
#include <type_traits>

namespace xp {

	namespace details {

		template<unsigned N, std::size_t... Indices>
		struct get_nth;

		template<std::size_t I, std::size_t... Indices>
		struct get_nth< 0, I, Indices... > {
			static const std::size_t value = I;
		};

		template<unsigned N, std::size_t I, std::size_t... Indices>
		struct get_nth<N, I, Indices... > {
			static const std::size_t value = get_nth<N - 1, Indices... >::value;
		};

		template<std::size_t... D>
		struct dimension_size;

		template<>
		struct dimension_size<1> {
			static const std::size_t value = 1;
		};

		template<std::size_t A>
		struct dimension_size<A> {
			static const std::size_t value = A;
		};

		template<std::size_t A, std::size_t... B>
		struct dimension_size<A, B...> {
			static const std::size_t value = A * dimension_size<B...>::value;
		};

	}

	template<typename T, std::size_t... K>
	class k_array;

	template<typename T, std::size_t N>
	class k_array<T, N> {
		T a[N];

	public:
		typedef T value_type;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef T* iterator;
		typedef const T* const_iterator;
		typedef std::reverse_iterator<iterator> reverse_iterator;
		typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

		size_type size() const {
			return N;
		}
		size_type max_size() const {
			return size();
		}
		bool empty() const {
			return size() == 0;
		}

		void fill(const T& val) {
			fill_n(begin(), size(), val);
		}

		iterator begin() { return iterator(std::addressof(a[0])); }
		iterator end() { return iterator(std::addressof(a[0]) + size()); }

		const_iterator cbegin() const { return const_iterator(std::addressof(a[0])); }
		const_iterator cend() const { return const_iterator(std::addressof(a[0]) + size()); }

		const_iterator begin() const { return cbegin(); }
		const_iterator end() const { return cend(); }

		reverse_iterator rbegin() { return reverse_iterator(begin()); }
		reverse_iterator rend() { return reverse_iterator(end()); }

		const_reverse_iterator crbegin() const { return const_reverse_iterator(cbegin()); }
		const_reverse_iterator crend() const { return const_reverse_iterator(cend()); }

		const_reverse_iterator rbegin() const { return crbegin(); }
		const_reverse_iterator rend() const { return crend(); }

		T& operator[](std::size_t i) {
			return a[i];
		}
		const T& operator[](std::size_t i) const {
			return a[i];
		}

		T& at(size_type i) {
			if (N <= i) throw std::out_of_range("invalid subscript");
			return a[i];
		}
		const T& at(size_type i) const {
			if (N <= i) throw std::out_of_range("invalid subscript");
			return a[i];
		}
	};

	template<typename T, std::size_t N, std::size_t... K>
	class k_array<T, N, K...> {
		k_array<T, K...> a[N];

	public:
		typedef T value_type;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef T* iterator;
		typedef const T* const_iterator;
		typedef std::reverse_iterator<iterator> reverse_iterator;
		typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

		size_type size() const {
			return dimension_size<K...>::value;
		}
		size_type max_size() const {
			return size();
		}
		bool empty() const {
			return size() == 0;
		}

		void fill(const T& val) {
			fill_n(begin(), size(), val);
		}

		iterator begin() { return iterator(std::addressof(a[0])); }
		iterator end() { return iterator(std::addressof(a[0]) + size()); }

		const_iterator cbegin() const { return const_iterator(std::addressof(a[0])); }
		const_iterator cend() const { return const_iterator(std::addressof(a[0]) + size()); }

		const_iterator begin() const { return cbegin(); }
		const_iterator end() const { return cend(); }

		reverse_iterator rbegin() { return reverse_iterator(begin()); }
		reverse_iterator rend() { return reverse_iterator(end()); }

		const_reverse_iterator crbegin() const { return const_reverse_iterator(cbegin()); }
		const_reverse_iterator crend() const { return const_reverse_iterator(cend()); }

		const_reverse_iterator rbegin() const { return crbegin(); }
		const_reverse_iterator rend() const { return crend(); }

		k_array<T, K...>& operator[](size_type i) {
			return a[i];
		}

		k_array<T, K...>& at(size_type i) {
			return a[i];
		}
		const k_array<T, K...>& at(size_type i) const {
			return a[i];
		}

		template<typename... I>
		auto at(size_type i, I... indices) -> decltype(a[i].at(indices...)) {
			if (N <= i) throw std::out_of_range("invalid subscript");
			return a[i].at(indices...);
		}

		template<typename... I>
		auto at(size_type i, I... indices) const -> decltype(a[i].at(indices...)) {
			if (N <= i) throw std::out_of_range("invalid subscript");
			return a[i].at(indices...);
		}
	};

	//template <typename T, std::size_t... K>
	//decltype(sizeof...(K)) rank(const k_array<T, K...>&) {
	//	return sizeof...(K);
	//}

	template <std::size_t N, typename T, std::size_t... K>
	std::size_t dim(const k_array<T, K...>&) {
		return extent<k_array<T, K...>, N>::value;
	}

	template<typename T, std::size_t... K>
	std::size_t size(const k_array<T, K...>&) {
		return details::dimension_size<K...>::value;
	}

}

namespace std {
	template <typename T, std::size_t... K>
	struct rank<xp::k_array<T, K...>> : integral_constant<size_t, sizeof...(K)>
	{};

	template <unsigned I, class T, std::size_t... K>
	struct extent<xp::k_array<T, K...>, I> : integral_constant<size_t, xp::details::get_nth<I, K...>::value>
	{};
}

#endif __K_ARRAY_H__
