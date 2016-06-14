#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <functional>
#include <iostream>
#include <string>
#include <sstream>
#include <type_traits>
#include <utility>

// The stringize function is a complete rework of code written by Joakim Karlsson & Kim Gräsman,
//  <https://github.com/joakimkarlsson/snowhouse/blob/master/snowhouse/stringize.h>

namespace xp {

	namespace details {

		struct stream_fallback_t {
			template<typename T>
			stream_fallback_t(const T&);

			friend stream_fallback_t operator<<(std::ostream&, const stream_fallback_t&);
		};

		template<typename T>
		std::string stringize(const T& val, std::false_type) {
			std::ostringstream buf;
			buf << val;
			return buf.str();
		}

		template<typename T>
		std::string stringize(const T& /*val*/, std::true_type) {
			//static_assert(false, "unsupported type");
			return typeid(T).name();
		}

	} // namespace details

	template<typename T>
	std::string stringize(const T& val) {
		using namespace std;
		using namespace details;
		return stringize(val, is_same<stream_fallback_t, decltype(cout << val)>::type());
	}

	// inspired by Jon Kalb at_scope_exit but removed the possibility to
	// reset the action. The only use case I found so far is if you want to
	// rollback (perform the cleanup) or commit (do nothing). But then, it should 
	// be another utility.
	class at_scope_exit {
		typedef std::function<void(void)> action_type;
		action_type action;

	public:
		template<typename F>
		at_scope_exit(F fn) try : action(fn) {} catch (...) { fn(); }
		~at_scope_exit() { action(); }

		at_scope_exit() = delete;
		at_scope_exit(const at_scope_exit&) = delete;
		at_scope_exit& operator=(const at_scope_exit&) = delete;
	};

	// from Sedgewick
	template<typename T>
	T bits(T x, int k, int j) {
		static_assert(std::is_unsigned<T>::value, "bits requires unsigned");
		return (x >> k) & ~(~0 << j);
	}

	// from Stroustup (c++ programming language, 4th edition, p. 299)
	template<typename T, typename U>
	T narrow_cast(U x) {
		auto r = static_cast<T>(x);
		if (static_cast<U>(r) != x)
			throw std::runtime_error("narrowing failed.");
		return r;
	}

	// from Stroustrup (Programming Principles & Practice Using C++, 2nd edition, p. 854)
	template<typename T, typename U>
	T convert_cast(U x) {
		std::stringstream io;
		T result;
		if (!(io << x) || !(io >> result) || !(io >> std::ws).eof())
			throw std::runtime_error{ "conversion_cast failed." };
		return result;
	}

} // namespace xp

#endif __UTILITY_H__