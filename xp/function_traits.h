#ifndef __FUNCTION_TRAITS_H__
#define __FUNCTION_TRAITS_H__

#include <cstddef>
#include <tuple>

// The function traits are not my own. They were found at <http://functionalcpp.wordpress.com/2013/08/05/function-traits/>
// and proved very useful for expressing requirements for EoP algorithms.

namespace xp {

	// function traits comes from 
	template<typename F>
	struct function_traits;

	template<typename R, typename... Args>
	struct function_traits<R(Args...)>
	{
		using return_type = R;

		static constexpr std::size_t arity = sizeof...(Args);

		template <std::size_t N>
		struct argument
		{
			static_assert(N < arity, "error: invalid parameter index.");
			using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
		};
	};

	template<typename R, typename... Args>
	struct function_traits<R(*)(Args...)> : public function_traits<R(Args...)>
	{};

	// member function pointer
	template<typename C, typename R, typename... Args>
	struct function_traits<R(C::*)(Args...)> : public function_traits<R(C&, Args...)>
	{};

	// const member function pointer
	template<typename C, typename R, typename... Args>
	struct function_traits<R(C::*)(Args...) const> : public function_traits<R(C&, Args...)>
	{};

	// member object pointer
	template<typename C, typename R>
	struct function_traits<R(C::*)> : public function_traits<R(C&)>
	{};

	// functor
	template<class F>
	struct function_traits
	{
	private:
		using call_type = function_traits<decltype(&F::operator())>;

	public:
		using return_type = typename call_type::return_type;

		static constexpr std::size_t arity = call_type::arity - 1;

		template <std::size_t N>
		struct argument
		{
			static_assert(N < arity, "error: invalid parameter index.");
			using type = typename call_type::template argument<N + 1>::type;
		};
	};

	template<class F>
	struct function_traits<F&> : public function_traits<F>
	{};

	template<class F>
	struct function_traits<F&&> : public function_traits<F>
	{};

} // namespace xp

#endif __FUNCTION_TRAITS_H__