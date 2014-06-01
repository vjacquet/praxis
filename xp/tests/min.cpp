#include <iostream>
#include <utility>

#include "testbench.h"

using namespace std;

namespace {
	template<typename T>
	inline
	const T& min(const T& a, const T& b) {
		cout << ">> std min" << endl;
		if (b < a)
			return b;
		return a;
	}

	template<typename T>
	inline
	T&& min(T&& a, T&& b) {
		cout << ">> rvalue min" << endl;
		if (b < a)
			return forward<T>(b);
		return forward<T>(a);
	}

	template<typename T>
	struct instrumented
	{
		typedef T value_type;

		T value;

		// Conversion
		instrumented(const T& v) : value(v) { cout << "ctor: " << value << endl; } // could be explicit

		// Semiregular
		instrumented() : value() { cout << "default ctor: " << value << endl; }
		~instrumented() { cout << "~instrumented: " << value << endl; }
		instrumented(const instrumented& x) : value(x.value) { cout << "copy ctor: " << value << endl; } // could be implicitly declared
		instrumented& operator=(const instrumented& x) {
			cout << "assign " << x.value << " to " << value << endl;
			value = x.value;
			return *this;
		}
		instrumented(instrumented&& x) : value(std::move(x.value)) { cout << "move ctor: " << value << endl; } // could be implicitly declared
		instrumented& operator=(instrumented&& x) {
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
		friend bool operator==(const instrumented& x, const instrumented& y) {
			return x.value == y.value;
		}
		friend bool operator!=(const instrumented& x, const instrumented& y) {
			return !(x == y);
		}

		// TotallyOrdered
		friend bool operator<(const instrumented& x, const instrumented& y) {
			return x.value < y.value;
		}
		friend bool operator>(const instrumented& x, const instrumented& y) {
			return y < x;
		}
		friend bool operator<=(const instrumented& x, const instrumented& y) {
			return !(y < x);
		}
		friend bool operator>=(const instrumented& x, const instrumented& y) {
			return !(x < y);
		}
	};

	template<typename T>
	instrumented<T> instrument(T t) {
		return instrumented<T> { t };
	}
}

TESTBENCH()

TEST(check_min) {
	cout << "min With const lvalues" << endl;
	const auto a = instrument(1);
	const auto b = instrument(2);
	min(a, b).method();
	cout << endl;

	cout << "min with rvalues and anchored result" << endl;
	auto r = min(instrument(3), instrument(4));
	r.method();
	cout << endl;

	cout << "min with rvalues and rvalue result" << endl;
	min(instrument(5), instrument(6)).method();
	cout << endl;

	cout << "min with lvalues and rvalue result" << endl;
	auto c = instrument(7);
	auto d = instrument(8);
	min(c, d).method();
	cout << endl;

	cout << "min with mixed lvalue and rvalue and anchored result" << endl;
	auto e = instrument(9);
	auto r2 = min(e, instrument(10));
	r2.method();
	cout << endl;

	cout << "min with mixed rvalue and lvalue and anchored result" << endl;
	auto f = instrument(12);
	auto& r3 = min(instrument(11), f);
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

TESTFIXTURE(min_fixture)
