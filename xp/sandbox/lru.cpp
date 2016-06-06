#include <exception>
#include <list>
#include <string>
#include <unordered_map>
#include <utility>

#include "../tests/testbench.h"

namespace xp {

template <typename Key, typename T> class lru_cache {
private:
	typedef std::list<std::pair<Key, T>> list_type;
	typedef std::unordered_map<Key, typename list_type::iterator> map_type;
	list_type item_list;

	map_type item_map;
	size_t cache_size;

private:
	void clean(void) {
		while (item_map.size() > cache_size) {
			auto last_it = item_list.end(); last_it--;
			item_map.erase(last_it->first);
			item_list.pop_back();
		}
	};

public:
	lru_cache(int cache_size) :cache_size(cache_size) {
	};

	void put(const Key &key, const T &val) {
		auto it = item_map.find(key);
		if (it != item_map.end()) {
			item_list.erase(it->second);
			item_map.erase(it);
		}
		item_list.push_front(make_pair(key, val));
		item_map.insert(make_pair(key, item_list.begin()));
		clean();
	};

	bool exist(const Key &key) {
		return item_map.find(key) != item_map.end();
	};

	bool touch(const Key &key) {
		auto it = item_map.find(key);
		if (it == item_map.end())
			return false;
		item_list.splice(item_list.begin(), item_list, it->second);
		return true;
	}

	bool get(const Key &key, T& val) {
		auto it = item_map.find(key);
		if (it == item_map.end())
			return false;
		item_list.splice(item_list.begin(), item_list, it->second);
		val = it->second->second;
		return true;
	};
};

}

TESTBENCH()

TEST(check_cache) {
	using namespace xp;
	using namespace std;

	lru_cache<int, string> cache(4);

	cache.put(1, "one");
	cache.put(2, "two");
	cache.put(3, "three");
	cache.put(4, "four");
	cache.put(5, "five");

	string v;
	VERIFY(!cache.get(1, v));
	VERIFY(cache.get(4, v));
	VERIFY_EQ("four", v);
}

TESTFIXTURE(lru)