#include <iostream>
#include <utility>

#include "testbench.h"

//using namespace std;
using std::cout;
using std::endl;
using std::forward;

namespace {

	template<typename T>
	inline
	T&& min(T&& a, T&& b) {
		cout << ">> rvalue min" << endl;
		if (b < a)
			return forward<T>(b);
		return forward<T>(a);
	}

	template<typename T>
	inline
	const T& min(const T& a, const T& b) {
		cout << ">> std min" << endl;
		if (b < a)
			return b;
		return a;
	}

	//template<typename T>
	//inline
	//T& min(T& a, T& b) {
	//	cout << ">> lvalue min" << endl;
	//	if (b < a)
	//		return b;
	//	return a;
	//}

	template<typename T>
	struct logged
	{
		typedef T value_type;

		T value;

		// Conversion
		logged(const T& v) : value(v) { cout << "ctor: " << value << endl; } // could be explicit

		// Semiregular
		logged() : value() { cout << "default ctor: " << value << endl; }
		~logged() { cout << "~logged: " << value << endl; }
		logged(const logged& x) : value(x.value) { cout << "copy ctor: " << value << endl; } // could be implicitly declared
		logged& operator=(const logged& x) {
			cout << "assign " << x.value << " to " << value << endl;
			value = x.value;
			return *this;
		}
		logged(logged&& x) : value(std::move(x.value)) { cout << "move ctor: " << value << endl; } // could be implicitly declared
		logged& operator=(logged&& x) {
			cout << "move " << x.value << " to " << value << endl;
			value = std::move(x.value);
			return *this;
		}

		void method() {
			cout << "mutating method " << value << endl;
		}

		void method() const {
			cout << "readonly method " << value << endl;
		}

		void mutating() {
			cout << "mutating " << value << endl;
		}
		//void mutating() && = delete;

		void readonly() const {
			cout << "readonly " << value << endl;
		}

		// Regular
		friend bool operator==(const logged& x, const logged& y) {
			return x.value == y.value;
		}
		friend bool operator!=(const logged& x, const logged& y) {
			return !(x == y);
		}

		// TotallyOrdered
		friend bool operator<(const logged& x, const logged& y) {
			return x.value < y.value;
		}
		friend bool operator>(const logged& x, const logged& y) {
			return y < x;
		}
		friend bool operator<=(const logged& x, const logged& y) {
			return !(y < x);
		}
		friend bool operator>=(const logged& x, const logged& y) {
			return !(x < y);
		}
	};

	template<typename T>
	logged<T> make_logged(T t) {
		return logged<T> { t };
	}
}

TESTBENCH()

TEST(check_min) {
	cout << "min With const lvalues" << endl;
	const auto a = make_logged(1);
	const auto b = make_logged(2);
	min(a, b).method();
	cout << endl;

	cout << "min with rvalues and anchored result" << endl;
	auto r = min(make_logged(3), make_logged(4));
	r.method();
	cout << endl;

	cout << "min with rvalues and rvalue result" << endl;
	min(make_logged(5), make_logged(6)).method();
	cout << endl;

	cout << "min with lvalues and rvalue result" << endl;
	auto c = make_logged(7);
	auto d = make_logged(8);
	min(c, d).method();
	cout << endl;

	cout << "min with mixed lvalue and rvalue and anchored result" << endl;
	auto e = make_logged(9);
	auto r2 = min(e, make_logged(10));
	r2.method();
	cout << endl;

	cout << "min with mixed rvalue and lvalue and anchored result" << endl;
	auto f = make_logged(12);
	auto& r3 = min(make_logged(11), f);
	r3.method();
	cout << endl;

	cout << "min with mixed reference and literal" << endl;
	int g = 13;
	int r4 = min(g, 14);
	cout << r4 << endl;
	cout << endl;

	cout << "min with literal" << endl;
	cout << min(15, 16) << endl;
	cout << endl;

	cout << "end" << endl;
}

TESTFIXTURE(min)
