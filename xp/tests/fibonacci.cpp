#include <cstdint>
#include <cmath>

#include <algorithm>
#include <functional>
#include <utility>
#include <vector>

#include "../benchmark.h"

#include "testbench.h"

using namespace std;
using namespace xp;

namespace ewilliams {

	int find(int N)
	{
		int returnVal = 0;
		int first = 1;
		int second = 1;
		while (second < N)
		{
			second = first + second;
			first = second - first;

			// cout << "Second: " << second << " : first " << first << endl;
		}

		int val1 = N - first;
		int val2 = second - N;

		if (val1 < val2)
		{
			returnVal = first;
		} else
		{
			returnVal = second;
		}
		return returnVal;
	}

}

namespace slewis {

	const double FPF = pow(5, 0.5);
	const double PHI = (1 + FPF) / 2;

	int fib(int n) {
		// Binets formula
		return (int)rint((pow(PHI, n) - pow((1 - PHI), n)) / FPF);
	}

	int fibinv(int n) {
		if (n < 2) return n;
		// Inverse Binet, dropping the 1-PHi^n.
		return fib((int)rint(log(n * FPF) / log(PHI)));
	}

}
namespace vjacquet {

	template<typename T>
	pair<T, T> fibonaccci_step(const T& fn_1, const T& fn_2) {
		// Fn = Fn-1 + Fn-2
		return {fn_1 + fn_2, fn_1};
	}

	template<typename T>
	pair<T, T> bounding_fibonacci(const T& x) {
		T fn_2 = {0};
		T fn_1 = {1};
		if (x > 1) {
			do
				tie(fn_1, fn_2) = fibonaccci_step(fn_1, fn_2);
			while (fn_1 < x);
		}
		return {fn_1, fn_2};
	}

	template<typename T>
	T nearest_fibonnaci(const T& x) {
		T fn_2, fn_1;
		tie(fn_1, fn_2) = bounding_fibonacci(x);
		if ((x - fn_2) < (fn_1 - x))
			return fn_2;
		return fn_1;
	}
}

TESTBENCH()

TEST(generate_fibonacci_series) {
	// int32_t overflows at 48.
	// int64_t overflows at 94.
	vector<int64_t> v(93, 0);
	v[1] = 1;
	transform(begin(v), end(v) - 2, begin(v) + 1, begin(v) + 2, plus<int64_t>());
}

TEST(check_get_nearest_fibonacci_ewilliams) {
	using namespace ewilliams;

	VERIFY_EQ(46368, find(50000));
	VERIFY_EQ(75025, find(75000));
}

TEST(check_get_nearest_fibonacci_slewis) {
	using namespace slewis;

	VERIFY_EQ(46368, fibinv(50000));
	VERIFY_EQ(75025, fibinv(75000));
}

TEST(check_get_nearest_fibonacci_vjacquet) {
	using namespace vjacquet;

	VERIFY_EQ(46368, nearest_fibonnaci(50000));
	VERIFY_EQ(75025, nearest_fibonnaci(75000));
}

TEST(benchmark) {
	using namespace std::chrono;

	typedef high_resolution_clock C;
	typedef int value_type;

	const value_type Val = 50000;
	const int Repeat = 25000000;

	auto ew = measure<C>(Repeat, bind(ewilliams::find, Val));
	cout << "  ewilliams: " << (double)Repeat / duration_cast<microseconds>(ew).count() << " executions per microseconds." << endl;
	auto sl = measure<C>(Repeat, bind(slewis::fibinv, Val));
	cout << "  slewis:    " << (double)Repeat / duration_cast<microseconds>(sl).count() << " executions per microseconds." << endl;
	auto vj = measure<C>(Repeat, bind(vjacquet::nearest_fibonnaci<value_type>, Val));
	cout << "  vjacquet:  " << (double)Repeat / duration_cast<microseconds>(vj).count() << " executions per microseconds." << endl;

}

TESTFIXTURE(fibonacci)
