#ifndef __CONCEPTS_H__
#define __CONCEPTS_H__

#include <iterator>
#include "function_traits.h"

#ifdef _MSC_VER

#define noexcept throw()

#endif

// list seen in Stepanov's programming conversations lecture 4
#define TotallyOrdered typename
//#define Pointer typename
#define Number typename
#define Integer typename
#define Unsigned typename
#define Integral typename
#define InputIterator typename
#define OutputIterator typename
#define ForwardIterator typename
#define BidirectionalIterator typename
#define RandomAccessIterator typename
#define Sorter typename
#define Incrementable typename
#define StrictWeakOrdering typename
#define Generator typename
#define UnaryOperation typename
#define BinaryOperation typename
#define EquivalenceRelation typename
#define Sequence typename
#define UnsignedIntegral typename

#define Container typename
#define Range typename
#define Function typename
#define Predicate typename
#define Relation typename

#define Size std::size_t

#define requires(...) 

#define DifferenceType(I) typename std::iterator_traits<I>::difference_type
#define ValueType(I) typename std::iterator_traits<I>::value_type

// use function traits to define Domain, Codomain, etc. as in EoP
//template<Function F> using Domain = typename std::decay<typename xp::function_traits<F>::argument<0>::type>::type;

#define Codomain(fn) typename xp::function_traits<decltype(fn)>::return_type
#define Domain(fn) typename xp::function_traits<decltype((fn))>::argument<0>::type
#define Arity(fn) xp::function_traits<decltype(fn)>::arity
#define InputType(fn, arg) typename xp::function_traits<decltype(fn)>::argument<arg>::type

#define Regular typename
#define Semiregular typename


#endif __CONCEPTS_H__
