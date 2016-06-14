#include <list>
#include <set>
#include <utility>
#include <vector>

#include "../iterator.h"

#include "testbench.h"

using namespace std;
using namespace xp;

namespace {

template<typename T>
struct segmented {
	typedef T value_type;
	typedef std::vector<T> segment;
	std::list<segment> segments;
};

template<typename T>
struct segmented_iterator {
	typename std::list<segmented<T>>::iterator seg;
	typename segmented<T>::segment::iterator it;

	T& operator*() const {
		return *it;
	}
	segmented_iterator& operator++() {
		if (++it == seg->end()) {
			++seg;
			return it = seg->begin();
		}
		return ++it;
	}
};

}

TESTBENCH()

TEST(check_inserter) {
	set<int> s {1, 2, 3};

	auto ins = inserter(s);
	*ins++ = 4;
	*ins++ = 5;

	VERIFY(s.size() == 5);
	VERIFY(s.find(4) != s.end());
}

TEST(check_stride_iterator) {
	vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9};
	stride_iterator_k<vector<int>::iterator, 2> i = v.begin();
	++i;
	VERIFY_EQ(3, *i);
}

TESTFIXTURE(iterator)
