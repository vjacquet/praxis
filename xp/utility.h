#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <iostream>
#include <string>
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
		std::string stringize(const T& val, std::true_type) {
			//static_assert(false, "unsupported type");
			return typeid(val).name();
		}

	} // namespace details

	template<typename T>
	std::string stringize(const T& val) {
		using namespace std;
		using namespace details;
		return stringize(val, is_same<stream_fallback_t, decltype(cout << val)>::type());
	}

} // namespace xp

#endif __UTILITY_H__