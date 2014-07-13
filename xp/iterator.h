#ifndef __ITERATOR_H__
#define __ITERATOR_H__

#include <iterator>
#include "fakeconcepts.h"

namespace xp {

	namespace details {

		// TODO: The use of the curiously recurring template pattern makes things a little bit too complicated to my taste.
		//       Stepanov uses a template and a typedef

		/* 
		 *                          curiously recurring template pattern
		 * +--------------------------------------------+--------------------------------------------+
		 * |                    pros                   |                   cons                    |
		 * +--------------------------------------------+--------------------------------------------+
		 * | * the implementation defines the ctrs     | * must make iterator_scaffolding friend   |
		 * | * the state is the implemention itself    |   to hide implementation details          |
		 * |                                           | * must define all typedefs or adapt trait |
		 * |                                           | * must cast scaffolding as implementation |
		 * |                                           |   before calling                          |
		 * +--------------------------------------------+--------------------------------------------+
		 *
		 *                          templatized implementation and typedef
		 * +--------------------------------------------+--------------------------------------------+
		 * |                    pros                   |                   cons                    |
		 * +--------------------------------------------+--------------------------------------------+
		 * | * the implementation can be in a separate | * the scaffoling must forward the ctrs to |
		 * |   namespace and the typedef makes it      |   the implementation                      |
		 * |   opaque                                  | * the state is a data member              |
		 * | * the implementation can inherit from     |                                           |
		 * |   iterator to define the typedefs         |                                           |
		 * +--------------------------------------------+--------------------------------------------+
		 *
		 */

		template<typename I, typename Category, typename T, typename Diff = std::ptrdiff_t, typename Pointer = T*, typename Reference = T&>
		struct iterator_scaffolding : std::iterator<Category, T, Diff, Pointer, Reference> {

			// Regular
			friend bool operator==(const I& x, const I& y) {
				return x.position() == y.position();
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

		template<typename I, typename Traits> 
		using iterator_scaffolding_adapter = iterator_scaffolding<I
			, typename Traits::iterator_category
			, typename Traits::value_type
			, typename Traits::difference_type
			, typename Traits::pointer
			, typename Traits::reference>;
	}

	// Caution: Prefer counted ranges. Do not use bounded range unless you known (last - first) % S == 0.
	template<InputIterator I, size_t S>
	struct stride_iterator_k : public details::iterator_scaffolding_adapter<stride_iterator_k<I, S>, std::iterator_traits<I>> {
		I base;

		stride_iterator_k() : base() {}
		stride_iterator_k(I i) : base(i) {}

		I& position() {
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
		void advance(difference_type n) {
			base += n * S;
		}
		difference_type distance(stride_iterator_k x) {
			return (x.base - base) / S;
		}
	};

	template<size_t S, InputIterator I>
	stride_iterator_k<I, S> make_stride_iterator_k(I i) {
		return stride_iterator_k<I, S>(i);
	}

	template<InputIterator I>
	struct stride_iterator : public details::iterator_scaffolding_adapter<stride_iterator<I>, std::iterator_traits<I>> {
		difference_type step;
		I base;

		stride_iterator() : step(0), base() {}
		stride_iterator(I i, difference_type step) : step(step), base(i) {}

		I& position() {
			return base;
		}

		reference source() const {
			return *base;
		}
		void successor() {
			base += step;
		}
		void predecessor() {
			base -= step;
		}
		void advance(difference_type n) {
			base += n * step;
		}
		difference_type distance(stride_iterator x) {
			return (x.base - base) / step;
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