#include <algorithm>
#include <exception>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

void ranges_fixture(size_t& pass, size_t& fail);
void get_lowest_fixture(size_t& pass, size_t& fail);
void algorithm_fixture(size_t& pass, size_t& fail);
void while_each_fixture(size_t& pass, size_t& fail);
void numeric_fixture(size_t& pass, size_t& fail);
void function_traits_fixture(size_t& pass, size_t& fail);
void min_fixture(size_t& pass, size_t& fail);
void bag_fixture(size_t& pass, size_t& fail);

int main(int argc, char* argv[])
try {
	size_t pass = 0, fail = 0;

	typedef void(*fixture_t)(size_t& pass, size_t& fail);
	typedef pair<string, fixture_t> named_fixture_t;

	vector<named_fixture_t> fixtures {
		{"ranges", ranges_fixture},
		{"get_lowest", get_lowest_fixture},
		{"while_each", while_each_fixture},
		{"algorithm", algorithm_fixture},
		{"numeric", numeric_fixture},
		{"function_traits", function_traits_fixture},
		{"min", min_fixture},
		{"bag", bag_fixture},
	};

	if (argc > 1) {
		// run the specified fixtures in the requested order
		for (auto first = argv + 1, last = argv + argc; first != last; ++first) {
			string name = *first;
			auto found = find_if(fixtures.cbegin(), fixtures.cend(), [&name](const named_fixture_t& x) { return x.first == name; });
			if (found != fixtures.cend())
				found->second(pass, fail);
		}
	} else {
		// run all fixtures
		for (auto& f : fixtures)
			f.second(pass, fail);
	}

	cout << "pass: " << pass << ", fail: " << fail << endl;
	if(fail) {
		cerr << "ERRORS PRESENT." << endl;
	} else if(!pass) {
		cerr << "ERROR, no tests run" << endl;
	}

	cin.get();
}
catch(const exception& e) {
	cerr << "Exception \"" << e.what() << "\" caught!" << endl;
	return -1;
}
catch(...) {
	cerr << "Unknown exception caught!" << endl;
	return -1;
}
