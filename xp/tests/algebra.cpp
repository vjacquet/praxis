#include <algorithm>
#include <fstream>
#include <functional>
#include <initializer_list>
#include <map>
#include <numeric>
#include <valarray>

#include "../algebra.h"
#include "../algorithm.h"
#include "../functional.h"
#include "../iterator.h"
#include "../numeric.h"

#include "testbench.h"

using namespace std;
using namespace xp;

namespace xp {

	// basic matrix class to test power
	template<typename T, unsigned N>
	struct matrix {
		static_assert(N > 0, "Dimension should not be 0");

		std::valarray<T> data;

		matrix() : data(N*N) {}
		matrix(const matrix& x) : data(x.data) {}
		matrix(matrix&& x) noexcept : data(std::move(x.data)) {}
		matrix(std::initializer_list<T> il) : data(N*N) {
			using namespace std;
			copy_at_most_n(begin(il), end(il), data.size(), begin(data));
		}

		matrix& operator=(const matrix& x) {
			data = x.data;
			return *this;
		}
		matrix& operator=(matrix&& x) {
			data = std::move(x.data);
			return *this;
		}

		T& operator[](std::pair<size_t, size_t> nm) {
			return data[nm.first + N*nm.second];
		}
		const T& operator[](std::pair<size_t, size_t> nm) const{
			return data[nm.first + N*nm.second];
		}

		// Regular
		inline friend bool operator==(const matrix& x, const matrix& y) {
			return x.data == y.data;
		}
		inline friend bool operator!=(const matrix& x, const matrix& y) {
			return !(x == y);
		}

		// arithmetic
		matrix& operator+=(const matrix& x) {
			data += x.data;
			return *this;
		}
		matrix& operator*=(const matrix& x) {
			for (unsigned i = 0; i != N; ++i)
				for (unsigned j = 0; j != N; ++j)
					data[i + N * j] = inner_product_n_nonempty(begin(x.data) + (N * j), make_stride_iterator_k<N>(begin(x.data) + i), N);
			return *this;
		}
		matrix& operator-=(const matrix& x) {
			data -= x.data;
			return *this;
		}

		inline friend matrix operator+(matrix x, const matrix& y) {
			return x += y;
		}
		inline friend matrix operator*(matrix x, const matrix& y) {
			return x *= y;
		}
		inline friend matrix operator-(matrix x, const matrix& y) {
			return x -= y;
		}
	};

	template<typename T>
	struct valmatrix {
		size_t n;
		std::valarray<T> data;

		valmatrix(size_t n) : n(n), data(n*n) {}
		valmatrix(const valmatrix& x) : n(x.n), data(x.data) {}
		valmatrix(valmatrix&& x) noexcept : n(x.n), data(std::move(x.data)) {}

		valmatrix& operator=(const valmatrix& x) {
			data = x.data;
			n = x.n;
			return *this;
		}
		valmatrix& operator=(valmatrix&& x) {
			data = std::move(x.data);
			n = x.n;
			return *this;
		}

		T& operator[](std::pair<size_t, size_t> nm) {
			return data[nm.first + n*nm.second];
		}
		const T& operator[](std::pair<size_t, size_t> nm) const{
			return data[nm.first + n*nm.second];
		}

		// Regular
		inline friend bool operator==(const valmatrix& x, const valmatrix& y) {
			return x.data == y.data;
		}
		inline friend bool operator!=(const valmatrix& x, const valmatrix& y) {
			return !(x == y);
		}

		// arithmetic
		valmatrix& operator+=(const valmatrix& x) {
			data += x.data;
			return *this;
		}
		valmatrix& operator*=(const valmatrix& x) {
			for (unsigned i = 0; i != n; ++i)
				for (unsigned j = 0; j != n; ++j)
					data[i + n * j] = inner_product_n_nonempty(begin(x.data) + (n * j), make_stride_iterator(begin(x.data) + i, n), n);
			return *this;
		}
		valmatrix& operator-=(const valmatrix& x) {
			data -= x.data;
			return *this;
		}

		inline friend valmatrix operator+(valmatrix x, const valmatrix& y) {
			return x += y;
		}
		inline friend valmatrix operator*(valmatrix x, const valmatrix& y) {
			return x *= y;
		}
		inline friend valmatrix operator-(valmatrix x, const valmatrix& y) {
			return x -= y;
		}

	};

	vector<pair<string, string>> get_relations() {
		vector<pair<string, string>> v = {
			{"asterix", "obelix"},
			{"asterix", "panoramix"},
			{"obelix", "idefix"},
			{"panoramix", "barometrix"},
			{"panoramix", "prefix"},
			{"panoramix", "cicatrix"},
			{"panoramix", "pronostix"},
			{"panoramix", "septantesix"},
			{"panoramix", "amnesix"},
			{"asterix", "abraracourcix"},
			{"bonemine", "abraracourcix"},
			{"agecanonix", "boufiltre"},
			{"asterix", "falbala"},
			{"asterix", "fanzine"},
			{"agecanonix", "assurancetourix"},
			{"assurancetourix", "cetautomatix"},
			{"cetautomatix", "ordralfabetix"},

			{"tintin", "tournesol"},
			{"tintin", "haddock"},
			{"tintin", "dupont"},
			{"tintin", "dupond"},
			{"dupond", "dupont"},
			{"haddock", "castafiore"},
			{"haddock", "seraphin lampion"},
			{"tintin", "rastapopoulos"},
			{"tintin", "general alcazar"},
			{"tintin", "dr muller"},

			{"spirou", "fantasio"},
			{"spirou", "spip"},
			{"spip", "marsupilami"},
			{"spirou", "zorglub"},
			{"spirou", "seccotine"},
			{"spirou", "compte de champignac"},

			{"gaston lagaffe", "fantasio"},
			{"gaston lagaffe", "prunelle"},
			{"gaston lagaffe", "lebrac"},
			{"gaston lagaffe", "mlle jeanne"},
			{"gaston lagaffe", "m. de mesmaeker"},
		};
		return v;
	}

	template <OutputIterator O>
	struct expand_output_iterator : std::iterator<std::output_iterator_tag, void, void, void, void>
	{
		O output;

		expand_output_iterator(O output) : output {output} {}
		expand_output_iterator() {}

		template <typename T>
		void operator=(const std::pair<T, T>& value) {
			output = value.first;
			++output;
			output = value.second;
			++output;
		}

		expand_output_iterator& operator*() { return *this; }
		expand_output_iterator& operator++() { return *this; }
		expand_output_iterator& operator++(int) { return *this; }
	};

	template <OutputIterator O>
	expand_output_iterator<O> expand(O output) {
		return expand_output_iterator<O> {output};
	}

}

TESTBENCH()

TEST(check_matrix_multiplies) {
	matrix<int, 3> m = {
		1, 1, 1,
		1, 1, 1, 
		1, 1, 1
	};

	auto p = m * m;
	VERIFY(std::count(begin(p.data), end(p.data), 3) == 9);
}

TEST(check_boolean_semiring) {
	typedef semiring<bool, logical_or<bool>, logical_and<bool>> boolean;

	boolean a = true;
	boolean b = false;

	VERIFY((a + b) == true);
	VERIFY((a * b) == false);
}

TEST(check_who_knows_who_power) {
	// Check knuth Vol I, page 354: Process equivalence relation for an algorithm using trees.

	typedef semiring<bool, logical_or<bool>, logical_and<bool>> boolean;

	auto relations = get_relations();

	//vector<string> persons;
	//persons.reserve(n * 2);
	//std::copy_n(relations.begin(), n, expand(std::back_inserter(persons)));
	//std::sort(persons.begin(), persons.end());
	//auto end = std::unique_copy(persons.begin(), persons.end(), persons.begin());
	//persons.resize(end - persons.begin());

	map<string, int> indexes;
	for (auto& p : relations) {
		indexes.insert(make_pair(p.first, indexes.size()));
		indexes.insert(make_pair(p.second, indexes.size()));
	}

	valmatrix<boolean> m {indexes.size()};
	for (auto& p : relations) {
		int i = indexes[p.first];
		int j = indexes[p.second];
		m[make_pair(i, i)] = true;
		m[make_pair(i, j)] = true;
		m[make_pair(j, i)] = true;
		m[make_pair(j, j)] = true;
	}

	m = power_semigroup(m, indexes.size());

	ofstream ofs("bd.txt");
	for (auto& first : indexes) {
		ofs << first.first << ": ";
		for (auto& second : indexes) {
			if (first != second && m[make_pair(first.second, second.second)]) {
				ofs << second.first << "; ";
			}
		}
		ofs << endl;
	}
}

TESTFIXTURE(algebra)