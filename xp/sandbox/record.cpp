#include <cstdint>

#include <string>
#include <tuple>
#include <utility>

#include "../tests/testbench.h"

namespace xp {
	using std::string;
	using std::wstring;
	using std::size_t;

	template<size_t N>
	struct constant { 
		static const size_t value = N;
	};

	template <class T, size_t N, class... Types>
	struct index_of {
		static const size_t value = N;
	};

	template <class T, size_t N, class F, class... Types>
	struct index_of<T, N, F, Types...> {
		static const size_t value = std::conditional<std::is_same<T, typename F::tag>::value
			, constant<N>
			, index_of<T, N + 1, Types...>>::type::value;
	};
	//template <class T, class... Types>
	//struct index_of<T, Types...> {
	//	static const size_t value = index_of<T, 0, Types...>::value;
	//};

	template<class T, class... Types>
	typename std::tuple_element<index_of<T, 0, Types...>::value, std::tuple<Types...>>::type& get(std::tuple<Types...>& t) {
		return std::get<index_of<T, 0, Types...>::value>(t);
	}
	template<class T, class... Types>
	const typename std::tuple_element<index_of<T, 0, Types...>::value, std::tuple<Types...>>::type& get(const std::tuple<Types...>& t) {
		return std::get<index_of<T, 0, Types...>::value>(t);
	}

	template<class T, class... Types>
	size_t get_index(const std::tuple<Types...>& t) {
		return index_of<T, 0, Types...>::value;
	}

	template <typename Tag, typename T> struct field_tag {
		typedef std::string value_type;

		template<typename U>
		std::pair<Tag, T> operator=(const U& x) {
			return std::make_pair(Tag {}, T(x));
		}
	};

	template <typename Tag>
	struct field {
		typedef Tag tag;
		typedef typename Tag::value_type value_type;

		field() : val() {}
		field(const value_type& x) : val(x) {}
		field(const field& x) : val(x.val) {}
		field(field&& x) : val(std::move(x.val)) {}

		template<typename U>
		field(U&& x) : val(std::forward<U>(x)) {}

		value_type val;

		field operator=(const field& x) {
			return *this;
		}
		tag operator=(const tag& x) {
			return *this;
		}
	};

	template <typename... Fields>
	struct record {
		template<typename F>
		using index_type = typename index_of<F, 0, Fields...>;

		typedef std::tuple<Fields...> storage;
		storage fields;

		//template<typename F>
		//using field_type = typename std::tuple_element<index_type<F>::value, storage>::type;

		typedef record base;

		record(const record& x) : fields(x.fields) { }

		template<typename... Args>
		record(Args&&... values) : fields (std::forward_as_tuple(values...)) {
		}

		template<typename F>
		typename std::tuple_element<index_type<F>::value, storage>::type::value_type& operator[](F) {
			return get<F>(fields).val;
		}
		template<typename F>
		typename const std::tuple_element<index_type<F>::value, storage>::type::value_type& operator[](F) const {
			return get<F>(fields).val;
		}

		template<typename... Args>
		void update(Args&&... values) {
			int count[] = {0, (set(values), 1)...};
		}

		template<typename Tag, typename V>
		void set(const std::pair<Tag, V>& arg) {
			get<Tag>(fields).val = arg.second;
		}
	};

	struct name_tag : field_tag < name_tag, std::string > { using field_tag::operator=; } fullname;
	struct phone_number_tag : field_tag<phone_number_tag, std::string> { using field_tag::operator=; } phone_number;
	struct email_tag : field_tag < email_tag, std::string > { using field_tag::operator=; } email;


	//struct Name : field < string, Name > { using field::field;/*using field<string, Name>::operator =;*/ } fullname;
	//struct PhoneNumber : field < string, PhoneNumber > { /*using field<string, PhoneNumber>::operator =;*/ } phone_number;

	struct Person : record< 
		field<decltype(fullname)>,
		field<decltype(email)>,
		field<decltype(phone_number)>
	> {
		Person(string name, string email, string phone_number) : base(std::move(name), std::move(email), std::move(phone_number)) {}
	};

} // namespace xp

TESTBENCH()

TEST(can_create_record) {
	using namespace xp;
	Person vjacquet {"vjacquet", "vjacquet@example.com", "+1 555 033 1234"};

	VERIFY_EQ(0, get_index<name_tag>(vjacquet.fields));
	VERIFY_EQ(3 * sizeof(std::string), sizeof(Person));

	VERIFY_EQ("vjacquet", vjacquet[fullname]);
	VERIFY_EQ("+1 555 033 1234", vjacquet[phone_number]);
	VERIFY_EQ("vjacquet@example.com", vjacquet[email]);
}

TEST(can_update_record) {
	using namespace xp;
	Person vjacquet {"vjacquet", "jacquet@example.com", "+1 555 033 9871"};

	VERIFY_EQ("+1 555 033 9871", vjacquet[phone_number]);
	VERIFY_EQ("jacquet@example.com", vjacquet[email]);

	vjacquet.update(phone_number = "+1 555 033 1234", email = "vjacquet@example.com");

	VERIFY_EQ("vjacquet", vjacquet[fullname]);
	VERIFY_EQ("+1 555 033 1234", vjacquet[phone_number]);
	VERIFY_EQ("vjacquet@example.com", vjacquet[email]);
}

TESTFIXTURE(record)