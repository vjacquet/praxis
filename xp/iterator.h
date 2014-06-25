#ifndef __ITERATOR_H__
#define __ITERATOR_H__

#include <iterator>
#include "fakeconcepts.h"

namespace xp {
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