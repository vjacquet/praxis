#ifndef __ITERATOR_H__
#define __ITERATOR_H__

#include <iterator>
#include "fakeconcepts.h"

namespace xp {

	namespace details {

		// TODO: The use of the curiously recurring template pattern makes things a little bit too complicated to my taste.
		//       Stepanov uses a template and a typedef
		template<typename I, typename Traits>
		struct iterator_scaffolding {
			typedef typename Traits::iterator_category iterator_category;
			typedef typename Traits::difference_type difference_type;
			typedef typename Traits::value_type value_type;
			typedef typename Traits::reference reference;
			typedef typename Traits::pointer pointer;

			// Regular
			friend bool operator==(const I& x, const I& y) {
				return x.state() == y.state();
			}
			friend bool operator!=(const I& x, const I& y) {
				return !(x == y);
			}

			// dereference
			reference operator*() const {
				return static_cast<const I*>(this)->source();
			}
			pointer operator->() const {
				return &(**this); 
			}

			// forward
			I& operator++() {
				auto self = static_cast<I*>(this);
				self->successor();
				return *self;
			}
			I& operator++(int) {
				I i = *this;
				++*this;
				return i;
			}

			// bidirectional
			I& operator--() {
				auto self = static_cast<I*>(this);
				self->predecessor();
				return *self;
			}
			I& operator--(int) {
				I i = *this;
				--*this;
				return i;
			}

			// random access
			I& operator+=(typename difference_type n) {
				auto self = static_cast<I*>(this);
				self->advance(n);
				return *self;
			}

			I& operator-=(typename difference_type n) {
				*this += -n;
				return *this;
			}

			reference operator[](typename difference_type n) const {
				return *(*this + n);
			}

			friend difference_type operator-(const I& x, const I& y) {
				return static_cast<const I*>(x)->distance(y);
			}

			I operator-(difference_type n) {
				return *this + (-n);
			}

			friend I operator+(I x, difference_type n) {
				return x += n;
			}

			friend I operator+(difference_type n, I x) {
				return x += n;
			}

			friend bool operator<(const I& x, const I& y) {
				return (x - y) < 0;
			}

			friend bool operator>(const I& x, const I& y) {
				return y < x;
			}

			friend bool operator<=(const I& x, const I& y) {
				return !(y < x);
			}

			friend bool operator>=(const I& x, const I& y) {
				return !(x < y);
			}
		};
	}

	// Caution: Prefer counted ranges. Do not use bounded range unless you known (last - first) % S == 0.
	template<InputIterator I, size_t S>
	struct stride_iterator_k : public details::iterator_scaffolding<stride_iterator_k<I, S>, std::iterator_traits<I>> {
		I base;

		stride_iterator_k() : base() {}
		stride_iterator_k(I i) : base(i) {}

		I& state() {
			return base;
		}

		reference source() const {
			return *base;
		}
		void successor() {
			base += S;
		}
		void predecessor() {
			base -= S;
		}
	};

	template<size_t S, InputIterator I>
	stride_iterator_k<I, S> make_stride_iterator_k(I i) {
		return stride_iterator_k<I, S>(i);
	}

	template<InputIterator I>
	struct stride_iterator : public details::iterator_scaffolding<stride_iterator<I>, std::iterator_traits<I>> {
		typedef typename std::iterator_traits<I>::difference_type difference_type;

		difference_type n;
		I base;

		stride_iterator() : n(0), base() {}
		stride_iterator(I i, difference_type n) : n(n), base(i) {}

		I& state() {
			return base;
		}

		reference source() const {
			return *base;
		}
		void successor() {
			base += n;
		}
		void predecessor() {
			base -= n;
		}
	};

	template<InputIterator I>
	stride_iterator<I> make_stride_iterator(I i, std::size_t n) {
		return stride_iterator<I>(i, n);
	}

	using std::reverse_iterator;

	template <Container C>
	class unary_insert_iterator : public std::iterator<std::output_iterator_tag, void, void, void, void>
	{
	protected:
		C* c;

	public:
		typedef C container_type;

		explicit unary_insert_iterator(C& x) : c(&x) {}

		unary_insert_iterator<C>& operator= (const typename C::value_type& value) {
			c->insert(value); 
			return *this;
		}
		unary_insert_iterator<C>& operator= (typename C::value_type&& value){
			c->insert(std::move(value));
			return *this;
		}
		unary_insert_iterator<C>& operator* () {
			return *this;
		}
		unary_insert_iterator<C>& operator++ () {
			return *this;
		}
		unary_insert_iterator<C> operator++ (int) {
			return *this;
		}
	};

	template <Container C>
	unary_insert_iterator<C> inserter(C& x) {
		return unary_insert_iterator<C> {x};
	}

	template <BidirectionalIterator I>
	reverse_iterator<I> reverse(I it) {
		return reverse_iterator {it};
	}

}

#endif __ITERATOR_H__