#ifndef __LAZY_H__
#define __LAZY_H__

#include <future>
#include <mutex>
#include <type_traits>

namespace xp {

namespace details {

	template<typename T>
	struct box {
		typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type buffer;

		box() {}

		T* data() {
			return static_cast<T*>(static_cast<void*>(&buffer));
		}

		template<typename... Args>
		void construct(Args&&... args) {
			new(static_cast<void*>(&buffer)) T(std::forward<Args>(args)...);
		}

		template<typename... Args>
		void operator()(Args&&... args) {
			new(static_cast<void*>(&buffer)) T(std::forward<Args>(args)...);
		}

		void destroy() {
			data()->~T();
		}
	};

} // namespace details


template<typename T>
class lazy {
	mutable details::box<T> val;
	mutable std::future<void> initializer;

public:
	template<typename... Args>
	lazy(Args&&... args) : initializer {std::async(std::launch::deferred, std::ref(val), std::forward<Args>(args)...)} {}

	template<typename... Args>
	lazy(std::launch policy, Args&&... args) : initializer {std::async(policy, std::ref(val), std::forward<Args>(args)...)} {}

	~lazy() {
		if (!initializer.valid())
			val.destroy();
	}

	T& get() {
		if (initializer.valid()) {
			initializer.get();
		}
		return *val.data();
	}
	T const& get() const {
		if (initializer.valid()) {
			initializer.get();
		}
		return *val.data();
	}
};

} // namespace xp

#endif __LAZY_H__