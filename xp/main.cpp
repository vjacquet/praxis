#include <exception>
#include <iostream>

using namespace std;

void ranges_fixture(size_t& pass, size_t& fail);
void get_lowest_fixture(size_t& pass, size_t& fail);
void algorithm_fixture(size_t& pass, size_t& fail);
void while_each_fixture(size_t& pass, size_t& fail);
void numeric_fixture(size_t& pass, size_t& fail);
void function_traits_fixture(size_t& pass, size_t& fail);
void min_fixture(size_t& pass, size_t& fail);

int main(int argc, char* argv[])
try {
	size_t pass = 0, fail = 0;

	ranges_fixture(pass, fail);
	get_lowest_fixture(pass, fail);
	while_each_fixture(pass, fail);
	algorithm_fixture(pass, fail);
	numeric_fixture(pass, fail);
	function_traits_fixture(pass, fail);
	min_fixture(pass, fail);

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
