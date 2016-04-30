#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "../tests/testbench.h"

TESTBENCH()

int uid() {
	static int latest = 0;
	return ++latest;
}

template <class T>
using owner = T;


struct Base {
	int id;
	std::string name;

	Base(std::string name) : id(uid()), name(name) {}
	Base(Base const& base) : id(uid()), name("copy of " + base.name) {}

	Base& operator=(Base const& x) {
		name = "copy of " + x.name;
		return *this;
	}

	virtual ~Base() {}

	virtual owner<Base*> clone() {
		return new Base(*this);
	}

	virtual std::string to_string() {
		std::stringstream msg;
		msg << "base " << name << " #" << id;
		return msg.str();
	}
};

struct Derived : public Base {
	std::string src;

	Derived(std::string name) : Base(name), src{ name } {}
	Derived(Derived const&) = default;

	Derived& operator=(Derived const&) = default;

	virtual owner<Derived*> clone() override {
		return new Derived(*this);
	}

	virtual std::string to_string() override {
		std::stringstream msg;
		msg << "derived " << name << " #" << id;
		return msg.str();
	}
};

TEST(can_clone) {
	Derived d{ "b" };
	auto p = std::unique_ptr<Derived>(d.clone());

	VERIFY_EQ("derived copy of b #2", p->to_string());
}

TESTFIXTURE(clone)