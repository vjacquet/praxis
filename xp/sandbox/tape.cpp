#include <cstdint>
#include <algorithm>
#include <memory>

#include "../fakeconcepts.h"
#include "../tests/testbench.h"

// Playing with stepanov's tape
namespace stepanov {

	// from extent.h ////////////////////////////////////////////////////////
	struct byte_copier {
		void copy(const uint8_t* first, const uint8_t* last, uint8_t* result) {
			std::copy(first, last, result);
		}
		void move(const uint8_t* first, const uint8_t* last, uint8_t* result) {
			std::copy(first, last, result);
		}
		void move_backward(const uint8_t* first, const uint8_t* last, uint8_t* result) {
			std::copy_backward(first, last, result);
		}
		void clean_up(uint8_t*, uint8_t*) {}
	};

	template <typename Metadata, typename Copier = byte_copier>
	struct extent {

	private:
		typedef uint8_t* pointer;
		typedef const uint8_t* const_pointer;
		typedef extent self;

		pointer start;

		pointer guarded_malloc(size_t n) {
			void* tmp = malloc(n);
			if (tmp == NULL) throw std::bad_alloc();
			return pointer(tmp);
		}

		struct header_t {
			size_t finish;
			size_t end_of_block;
			Metadata metadata;
		};

		header_t* header() {
			return (header_t*)(start)-ptrdiff_t(1);
		}

		const header_t* header() const {
			return (header_t*)(start)-ptrdiff_t(1);
		}

		size_t& end_of_block() {
			return header()->end_of_block;
		}

		const size_t& end_of_block() const {
			return header()->end_of_block;
		}

		size_t& finish() {
			return header()->finish;
		}

		const size_t& finish() const {
			return header()->finish;
		}

	public:

		pointer storage() { return start; }

		const_pointer storage() const { return start; }

		Metadata* metadata() {
			return start ? &(header()->metadata) : NULL;
		}

		const Metadata* metadata() const {
			return start ? &(header()->metadata) : NULL;
		}

		// returns the size of the contents in bytes
		size_t byte_size() const {
			return start ? finish() : size_t(0);
		}

		pointer content_end() { return storage() + byte_size(); }

		const_pointer content_end() const { return storage() + byte_size(); }

		// returns the total current data-holding capacity of the tape in bytes
		size_t byte_capacity() const {
			return start ? end_of_block() : size_t(0);
		}

		// returns the size in bytes of the allocated extent
		size_t total_byte_size() const {
			return start ? byte_capacity() + sizeof(header_t) : size_t(0);
		}

		// returns the remaining capacity for data in bytes
		size_t remaining_byte_capacity() const {
			return byte_capacity() - byte_size();
		}

		// returns true if and only if the extent is empty
		bool empty() const { return !start; }

	private:
		pointer new_block_start(size_t additional) {
			size_t increment = std::max(byte_capacity(), additional);
			size_t new_capacity = byte_capacity() + increment;
			pointer block = guarded_malloc(sizeof(header_t) + new_capacity);

			header_t* p_header = (header_t*)(block);
			p_header->metadata = start ? *metadata() : Metadata();
			p_header->finish = byte_size();
			p_header->end_of_block = new_capacity;

			return block + sizeof(header_t);
		}

		void replace_start(pointer new_start) {
			pointer old_start = start;
			start = new_start;
			if (old_start) free((void*)(old_start - sizeof(header_t)));
		}

		void deallocate() { replace_start(NULL); }

		void reallocate(size_t additional) {
			pointer new_start = new_block_start(additional);
			if (start) Copier().move(storage(), content_end(), new_start);
			replace_start(new_start);
		}

		void reallocate(size_t additional, size_t offset) {
			if (additional == 0) return;
			pointer new_start = new_block_start(additional);
			if (start) {
				Copier().move(start, start + offset, new_start);
				Copier().move(start + offset, content_end(), new_start + offset + additional);
			}
			replace_start(new_start);
		}

	public:

		void adjust_byte_capacity(size_t n) {
			if (remaining_byte_capacity() != n) {
				self tmp;
				tmp.reallocate(byte_size() + n);
				Copier().copy(start, content_end(), tmp.start);
				tmp.finish() = byte_size();
				*tmp.metadata() = start ? *metadata() : Metadata();
				std::swap(start, tmp.start);
			}
			// postcondition: assert(remaining_byte_capacity() == n)
		}


		template <typename Writer>
		pointer insert_space(pointer position, size_t inserted_byte_size, Writer writer) {
			if (!inserted_byte_size) return position;
			size_t offset(position - start);
			if (remaining_byte_capacity() < inserted_byte_size) {
				reallocate(inserted_byte_size, offset);
			} else {
				pointer new_finish_p = content_end() + inserted_byte_size;
				Copier().move_backward(start + offset, content_end(), new_finish_p);
			}
			writer(start + offset);
			finish() += inserted_byte_size;
			return start + offset;
		}

		template <typename Writer>
		pointer insert_space(size_t inserted_byte_size, Writer writer) {
			if (!inserted_byte_size) return content_end();
			if (remaining_byte_capacity() < inserted_byte_size) {
				reallocate(inserted_byte_size);
			}
			pointer write_point = content_end();
			writer(write_point);
			finish() += inserted_byte_size;
			return write_point;
		}


		// Done in a way that is safe for concurrent readers if the erase is at the end.
		pointer erase_space(pointer first, size_t erased_byte_size) {
			if (erased_byte_size) {
				pointer last = first + erased_byte_size;
				size_t new_byte_size = byte_size() - erased_byte_size;
				pointer old_content_end = content_end();
				finish() = new_byte_size;
				if (!new_byte_size) {
					Copier().clean_up(first, last);
					deallocate();
					return NULL;
				} else {
					Copier().move(last, old_content_end, first);
					if (content_end() < last) Copier().clean_up(content_end(), last);
				}
			}
			return first;
		}

		~extent() { deallocate(); }

		extent() : start(NULL) {}

		extent(const self& x) : start(NULL) {
			if (x.start) {
				start = new_block_start(x.byte_size());
				Copier().copy(x.storage(), x.content_end(), start);
				finish() = x.byte_size();
				*metadata() = *x.metadata();
			}
		}

		friend
			void swap(self& x, self& y) {
			std::swap(x.start, y.start);
		}

		self& operator=(const self& x) {
			if (&x != this) {
				self tmp(x);
				swap(*this, tmp);
			}
			return *this;
		}
	};

	// from variabe_size_type.h /////////////////////////////////////////////
	// VJA: replaced typename InputIterator by InputIterator I
	template <InputIterator I, typename VariableSizeTypeDescriptor>
	std::pair<size_t, size_t>
		total_encoded_size(I first, I last,
		const VariableSizeTypeDescriptor& dsc) {
		size_t result(0);
		size_t n(0);
		while (first != last) {
			result += dsc.encoded_size(*first++);
			++n;
		}
		return std::make_pair(result, n);
	}

	// from iterator_adapter.h
	namespace adapter {

		template <typename IteratorBasis>
		class iterator
		{
		public:
			typedef typename IteratorBasis::state_type state_type;
			typedef typename IteratorBasis::iterator_category iterator_category;
			typedef typename IteratorBasis::value_type value_type;
			typedef typename IteratorBasis::difference_type difference_type;
			typedef typename IteratorBasis::reference reference;
			typedef typename IteratorBasis::pointer pointer;

		private:
			IteratorBasis basis;
		public:
			const state_type& state() const { return basis.state(); }

			iterator() {}

			iterator(const IteratorBasis& basis) : basis(basis) {}

			reference operator*() const {
				return basis.deref();
			}

			pointer operator->() const { return &(operator*()); }

			iterator& operator++() {
				basis.increment();
				return *this;
			}

			iterator operator++(int) {
				iterator tmp = *this;
				++*this;
				return tmp;
			}

			// requires bidirectional iterator (at least)
			iterator& operator--() {
				basis.decrement();
				return *this;
			}

			// requires bidirectional iterator (at least)
			iterator operator--(int) {
				iterator tmp = *this;
				--*this;
				return tmp;
			}

			// requires random access iterator
			iterator& operator+=(difference_type n) {
				basis.increment(n);
				return *this;
			}

			// requires random access iterator
			iterator& operator-=(difference_type n) {
				return *this += -n;
			}

			// requires random access iterator
			friend
				iterator operator+(iterator x, difference_type n) {
				return x += n;
			}

			// requires random access iterator
			friend
				iterator operator+(difference_type n, iterator x) {
				return x += n;
			}

			// requires random access iterator
			friend
				iterator operator-(iterator x, difference_type n) {
				return x -= n;
			}

			// requires random access iterator
			friend
				difference_type operator-(const iterator& x, const iterator& y) {
				return x.basis.difference(y.basis);
			}

			// requires random access iterator
			reference operator[](difference_type n) const { return *(*this + n); }

			friend
				bool operator==(const iterator& x, const iterator& y) {
				return x.state() == y.state();
			}

			friend
				bool operator!=(const iterator& x, const iterator& y) {
				return !(x == y);
			}

			// requires random access iterator
			friend
				bool operator<(const iterator& x, const iterator& y) {
				return y - x > 0;
			}

			// requires random access iterator
			friend
				bool operator>(const iterator& x, const iterator& y) {
				return y < x;
			}

			// requires random access iterator
			friend
				bool operator<=(const iterator& x, const iterator& y) {
				return !(y < x);
			}

			// requires random access iterator
			friend
				bool operator>=(const iterator& x, const iterator& y) {
				return !(x < y);
			}
		};

		class cout_basis {
		public:
			typedef void state_type;
			void state() {}
			void store(const int& x) { std::cout << x; }
			void store(const double& x) { std::cout << x; }
		};

		template <typename OutputIteratorBasis>
		class output_iterator
		{
		public:
			typedef typename OutputIteratorBasis::state_type state_type;
			typedef std::output_iterator_tag iterator_category;
			typedef void                     value_type;
			typedef void                     difference_type;
			typedef void                     pointer;
			typedef void                     reference;

		private:
			OutputIteratorBasis basis;
		public:
			const state_type& state() const { return basis.state(); }

			output_iterator() {}

			output_iterator(const OutputIteratorBasis& basis) : basis(basis) {}

			template <typename T>
			void operator=(const T& value) {
				basis.store(value);
			}

			output_iterator& operator*() { return *this; }
			output_iterator& operator++() { return *this; }
			output_iterator& operator++(int) { return *this; }
		};

		template <typename OutputIteratorBasis>
		inline
			output_iterator<OutputIteratorBasis>
			make_output_iterator(const OutputIteratorBasis& basis) {
			return output_iterator<OutputIteratorBasis>(basis);
		}

	} // namespace adapter

	// from variable_size_type_iterator.h ///////////////////////////////////
	template <typename VariableSizeTypeDescriptor,
		bool prefixed_size,
		typename IteratorCategory>
	struct variable_size_iterator_basis;

	template <typename VariableSizeTypeDescriptor>
	struct variable_size_iterator_basis_base {
		typedef VariableSizeTypeDescriptor descriptor_type;

		struct state_type {
			const uint8_t* position;
			descriptor_type dsc;
			state_type(const uint8_t* position, descriptor_type dsc) :
				position(position), dsc(dsc) {}
			state_type() {}
		};

		friend bool
			operator==(const state_type& x,
			const state_type& y) {
			return x.position == y.position;
		}

		typedef typename VariableSizeTypeDescriptor::iterator_category iterator_category;
		typedef typename VariableSizeTypeDescriptor::value_type value_type;
		typedef ptrdiff_t difference_type;
		typedef value_type reference;
		typedef void pointer;

		state_type st;

		variable_size_iterator_basis_base() {}
		variable_size_iterator_basis_base(const uint8_t* position, const descriptor_type& dsc) : st(position, dsc) {}

	};


	template <typename VariableSizeTypeDescriptor>
	struct variable_size_iterator_basis<VariableSizeTypeDescriptor,
		true,
		std::forward_iterator_tag>
		: variable_size_iterator_basis_base < VariableSizeTypeDescriptor >
	{
	private:
		typedef variable_size_iterator_basis_base<VariableSizeTypeDescriptor> base;
	public:
		variable_size_iterator_basis() {}

		variable_size_iterator_basis(const uint8_t*,
									 const uint8_t* position,
									 const VariableSizeTypeDescriptor& dsc)
									 : base(position, dsc) {}

		const typename base::state_type&
			state() { return this->st; }

		typename base::reference
			deref() const { return this->st.dsc.decode(this->st.position); }

		void increment() { this->st.position += this->st.dsc.size(this->st.position); }
	};

	template <typename VariableSizeTypeDescriptor>
	struct variable_size_iterator_basis<VariableSizeTypeDescriptor,
		true,
		std::bidirectional_iterator_tag>
		: variable_size_iterator_basis_base < VariableSizeTypeDescriptor >
	{
	private:
		typedef variable_size_iterator_basis_base<VariableSizeTypeDescriptor> base;
	public:
		const uint8_t* origin;

		variable_size_iterator_basis() : base(), origin(NULL) {}

		variable_size_iterator_basis(const uint8_t* origin,
									 const uint8_t* position,
									 const VariableSizeTypeDescriptor& dsc)
									 : base(position, dsc), origin(origin) {}

		const typename base::state_type&
			state() { return this->st; }

		typename base::reference
			deref() const { return this->st.dsc.decode(this->st.position); }

		void increment() { this->st.position += this->st.dsc.size(this->st.position); }

		void decrement() { this->st.position = this->st.dsc.previous(origin, this->st.position); }
	};


	template <typename VariableSizeTypeDescriptor>
	struct variable_size_iterator_basis<VariableSizeTypeDescriptor,
		false,
		std::forward_iterator_tag>
		: variable_size_iterator_basis_base < VariableSizeTypeDescriptor >
	{
	private:
		typedef variable_size_iterator_basis_base<VariableSizeTypeDescriptor> base;
		std::pair<typename base::value_type, size_t> cache;
	public:
		variable_size_iterator_basis() : base() {
			cache.second = size_t(0);
		}

		variable_size_iterator_basis(const uint8_t*,
									 const uint8_t* position,
									 const VariableSizeTypeDescriptor& dsc)
									 : base(position, dsc) {
			cache.second = size_t(0);
		}

		const typename base::state_type&
			state() { return this->st; }

		typename base::reference
			deref() const {
			if (cache.second == size_t(0)) {
				const_cast<variable_size_iterator_basis*>(this)->cache =
					this->st.dsc.attributes(this->st.position);
			}
			return cache.first;
		}

		void increment() {
			this->st.position += cache.second ? cache.second : this->st.dsc.size(this->st.position);
			cache.second = size_t(0);
		}
	};

	template <typename VariableSizeTypeDescriptor>
	struct variable_size_iterator_basis<VariableSizeTypeDescriptor,
		false,
		std::bidirectional_iterator_tag>
		: variable_size_iterator_basis_base < VariableSizeTypeDescriptor >
	{
	private:
		typedef variable_size_iterator_basis_base<VariableSizeTypeDescriptor> base;
		std::pair<typename base::value_type, size_t> cache;
	public:
		const uint8_t* origin;

		variable_size_iterator_basis() : base(), origin(NULL) {
			cache.second = size_t(0);
		}

		variable_size_iterator_basis(const uint8_t* origin,
									 const uint8_t* position,
									 const VariableSizeTypeDescriptor& dsc)
									 : base(position, dsc), origin(origin) {
			cache.second = size_t(0);
		}

		const typename base::state_type&
			state() const { return this->st; }

		typename base::reference
			deref() const {
			if (cache.second == size_t(0)) {
				const_cast<variable_size_iterator_basis*>(this)->cache =
					this->st.dsc.attributes(this->st.position);
			}
			return cache.first;
		}

		void increment() {
			this->st.position += cache.second ? cache.second : this->st.dsc.size(this->st.position);
			cache.second = size_t(0);
		}

		void decrement() {
			cache = this->st.dsc.attributes_backward(origin, this->st.position);
			this->st.position -= cache.second;
		}
	};


	/* Equality and Inequality Operators */

	template <typename VariableSizeTypeDescriptor,
		bool prefixed_size,
		typename IteratorCategory>
		inline
		bool operator==(const variable_size_iterator_basis < VariableSizeTypeDescriptor,
		prefixed_size,
		IteratorCategory > x,
		const variable_size_iterator_basis<VariableSizeTypeDescriptor,
		prefixed_size,
		IteratorCategory> y) {
		return x.st.position == y.st.position;
	}

	template <typename VariableSizeTypeDescriptor,
		bool prefixed_size,
		typename IteratorCategory>
		inline
		bool operator!=(const variable_size_iterator_basis < VariableSizeTypeDescriptor,
		prefixed_size,
		IteratorCategory > x,
		const variable_size_iterator_basis<VariableSizeTypeDescriptor,
		prefixed_size,
		IteratorCategory> y) {
		return !(x == y);
	}

	/* Specialized Equal */

	template <typename VariableSizeTypeDescriptor,
		bool prefixed_size,
		typename IteratorCategory>
		inline
		bool
		equal(adapter::iterator<variable_size_iterator_basis<VariableSizeTypeDescriptor,
		prefixed_size,
		IteratorCategory> >first1,
		adapter::iterator<variable_size_iterator_basis<VariableSizeTypeDescriptor,
		prefixed_size,
		IteratorCategory> >last1,
		adapter::iterator<variable_size_iterator_basis < VariableSizeTypeDescriptor,
		prefixed_size,
		IteratorCategory > >first2) {
		if (first1.state().dsc.equality_preserving) {
			return std::equal(first1.state().position,
							  last1.state().position,
							  first2.state().position);
		} else {
			return std::equal(first1, last1, first2);
		}
	}


	/* Output Iterator Basis */

	template <typename WritableVariableSizeTypeDescriptor>
	struct variable_size_output_iterator_basis {
		typedef WritableVariableSizeTypeDescriptor descriptor_type;
		typedef typename descriptor_type::value_type value_type;

		struct state_type {
			uint8_t* position;
			descriptor_type dsc;
			state_type(uint8_t* position, descriptor_type dsc) :
				position(position), dsc(dsc) {}
			state_type() {}
		};

		state_type st;

		variable_size_output_iterator_basis() : st(NULL, descriptor_type()) {}
		variable_size_output_iterator_basis(uint8_t* position,
											const descriptor_type& dsc) :
											st(position, dsc) {}

		const state_type& state() { return st; }

		void store(const value_type& v) {
			st.position = st.dsc.encode(v, st.position);
		}
	};

	/* Specialized Copy */

	template <typename WritableVariableSizeTypeDescriptor,
		bool prefixed_size,
		typename IteratorCategory>
		inline
		adapter::output_iterator<variable_size_output_iterator_basis<WritableVariableSizeTypeDescriptor> >
		copy(adapter::iterator<variable_size_iterator_basis<WritableVariableSizeTypeDescriptor,
		prefixed_size,
		IteratorCategory> > first,
		adapter::iterator<variable_size_iterator_basis<WritableVariableSizeTypeDescriptor,
		prefixed_size,
		IteratorCategory> > last,
		adapter::output_iterator < variable_size_output_iterator_basis<WritableVariableSizeTypeDescriptor> >
		result) {
		variable_size_output_iterator_basis<WritableVariableSizeTypeDescriptor> basis
			(std::copy(first.state().position, last.state().position, result.state().position),
			result.state().dsc);
		return adapter::make_output_iterator(basis);
	}

	// from tape.h //////////////////////////////////////////////////////////
	template <typename WritableVariableSizeTypeDescriptor>
	class tape {
	public:
		typedef WritableVariableSizeTypeDescriptor descriptor_type;
		typedef typename descriptor_type::value_type value_type;
		typedef value_type reference;
		typedef value_type const_reference;
		//typedef void pointer;
		typedef variable_size_iterator_basis <
			descriptor_type,
			descriptor_type::prefixed_size,
			typename descriptor_type::iterator_category > iterator_state;
		typedef adapter::iterator<iterator_state> const_iterator;
		typedef const_iterator iterator;
		typedef typename const_iterator::difference_type difference_type;
		typedef size_t  size_type;

	private:
		typedef uint8_t* pointer;
		typedef const uint8_t* const_pointer;
		typedef variable_size_output_iterator_basis<descriptor_type> output_iterator_state;
		typedef adapter::output_iterator<output_iterator_state> output_iterator;

		struct tape_metadata {
			size_t number_of_elements;
		};

		extent<tape_metadata> ext;
		descriptor_type dsc;

		size_type& number_of_elements() { // only safe when non-empty
			return ext.metadata()->number_of_elements;
		}

		const size_type& number_of_elements() const { // only safe when non-empty
			return ext.metadata()->number_of_elements;
		}

		struct back_insert_iterator_basis {
			typedef tape* state_type;
			state_type tape_p;
			state_type state() { return tape_p; }

			back_insert_iterator_basis() : tape_p(NULL) {}
			back_insert_iterator_basis(tape* tape_p) : tape_p(tape_p) {}

			struct writer {
				value_type value;
				descriptor_type dsc;

				writer(const value_type& value,
					   const descriptor_type& dsc) : value(value), dsc(dsc) {}
				void operator()(pointer p) {
					dsc.encode(value, p);
				}
			};

			void store(const value_type& value) {
				tape_p->ext.insert_space(tape_p->dsc.encoded_size(value),
										 writer(value, tape_p->dsc));
				++tape_p->number_of_elements();
			}
		};

		static
			const_pointer pos(const const_iterator& x) {
			return x.state().position;
		}

		static
			pointer pos_non_const(const const_iterator& x) {
			return const_cast<pointer>(x.state().position);
		}

	public:
		const extent<tape_metadata>& get_extent() const { return ext; }

		bool empty() const { return get_extent().empty(); }

		// returns the number of values currently stored in the tape
		size_type size() const { return empty() ? size_type(0) : number_of_elements(); }

		// returns (a conservative estimate of) the number of values the tape can hold
		// without reallocation
		size_type capacity() const {
			return size() + (get_extent().remaining_byte_capacity() / sizeof(value_type));
		}

		descriptor_type descriptor() const { return dsc; }

		const_iterator begin() const {
			const_pointer p = get_extent().storage();
			return const_iterator(iterator_state(p, p, dsc));
		}

		const_iterator end() const {
			const_pointer p = ext.storage();
			return const_iterator(iterator_state(p, p + get_extent().byte_size(), dsc));
		}

		friend
			inline
			bool operator==(const tape& x, const tape& y) {
			if (x.size() != y.size()) return false;
			if (x.dsc.equality_preserving &&
				x.get_extent().byte_size() != y.get_extent().byte_size()) return false;
			return equal(x.begin(), x.end(), y.begin()); // calls our equal that optimizes based on equality_preserving
		}

		friend
			inline
			bool operator!=(const tape& x, const tape& y) {
			return !(x == y);
		}

		friend
			inline
			bool operator<(const tape& x, const tape& y) {
			if (x.dsc.order_preserving) {
				const_pointer x_p = x.ext.storage();
				const_pointer y_p = y.ext.storage();
				return std::lexicographical_compare(x_p, x_p + x.get_extent().byte_size(),
													y_p, y_p + y.get_extent().byte_size());
			} else {
				return std::lexicographical_compare(x.begin(), x.end(),
													y.begin(), y.end());
			}
		}

		friend
			inline
			bool operator>=(const tape& x, const tape& y) {
			return !(x < y);
		}

		friend
			inline
			bool operator>(const tape& x, const tape& y) {
			return (y < x);
		}

		friend
			inline
			bool operator<=(const tape& x, const tape& y) {
			return !(x > y);
		}

		typedef adapter::output_iterator<back_insert_iterator_basis> back_insert_iterator;

		back_insert_iterator back_inserter() {
			return back_insert_iterator(back_insert_iterator_basis(this));
		}

		void push_back(const value_type& v) {
			*back_inserter() = v;
		}

	private:
		std::pair<const_iterator, const_iterator>
			inserted_range(pointer x, pointer y) {
			return std::make_pair(const_iterator(iterator_state(ext.storage(), x, dsc)),
								  const_iterator(iterator_state(ext.storage(), y, dsc)));
		}

		// VJA: replaced typename InputIterator by InputIterator I
		template <InputIterator I>
		std::pair<const_iterator, const_iterator>
			insert(const_iterator position, I first, I last,
			std::input_iterator_tag) {
			size_type insertion_offset(pos(position) - ext.storage());
			size_type old_byte_size(get_extent().byte_size());
			std::copy(first, last, back_inserter()); // might reallocate and invalidate position
			pointer begin_inserted_range = ext.storage() + insertion_offset;
			size_t increment = get_extent().byte_size() - old_byte_size;
			pointer end_inserted_range = begin_inserted_range + increment;
			std::rotate(begin_inserted_range, ext.storage() + old_byte_size, ext.content_end());
			return inserted_range(begin_inserted_range, end_inserted_range);
		}

		// VJA: replaced typename InputIterator by InputIterator I
		template <InputIterator I>
		struct writer {
			I first;
			I last;
			descriptor_type dsc;
			writer(I first, I last, const descriptor_type& dsc)
				: first(first), last(last), dsc(dsc) {}
			template <typename T>
			void write(pointer p, T) {
				output_iterator result(output_iterator_state(p, dsc));
				std::copy(first, last, result);
			}
			void write(pointer p, const_iterator) {
				std::copy(pos(first), pos(last), p);
			}
			void operator()(pointer p) { write(p, first); }
		};

		// VJA: replaced typename InputIterator by InputIterator I
		template <InputIterator I>
		std::pair<size_type, size_type>
			size_count(I first, I last) {
			return total_encoded_size(first, last, dsc);
		}


		std::pair<size_type, size_type>
			size_count(const_iterator first, const_iterator last) {
			return std::make_pair(pos(last) - pos(first), std::distance(first, last));
		}

		// VJA: replaced typename ForwardIterator by ForwardIterator I
		template <ForwardIterator I>
		std::pair<const_iterator, const_iterator>
			insert(const_iterator position,
			I first, I last,
			std::forward_iterator_tag) {
			std::pair<size_type, size_type>  size_and_count = size_count(first, last);
			pointer insert_position = pos_non_const(position);
			writer<I> w(first, last, dsc);
			pointer begin_inserted = ext.insert_space(insert_position, size_and_count.first, w);
			pointer end_inserted = begin_inserted + size_and_count.first;
			if (size_and_count.second) number_of_elements() += size_and_count.second;
			return inserted_range(begin_inserted, end_inserted);
		}

	public:

		void adjust_byte_capacity(size_type n) {
			ext.adjust_byte_capacity(n);
		}

		// VJA: replaced typename InputIterator by InputIterator I
		template <InputIterator I>
		std::pair<const_iterator, const_iterator>
			insert(const_iterator position, I first, I last) {
			typename std::iterator_traits<I>::iterator_category tag;
			return insert(position, first, last, tag);
		}

		const_iterator erase(const_iterator first, const_iterator last) {
			size_t number_of_erased_elements = size_t(std::distance(first, last));
			size_t size_of_erased_elements = pos(last) - pos(first);
			if (ext.erase_space(pos_non_const(first), size_of_erased_elements)) {
				number_of_elements() -= number_of_erased_elements;
				return first;
			} else {
				return const_iterator();
			}
		}

		tape(const descriptor_type& dsc = descriptor_type())
			: dsc(dsc) {}

		template <InputIterator I>
		tape(I first, I last,
			 const descriptor_type& dsc = descriptor_type())
			 : dsc(dsc) {
			insert(end(), first, last);
		}

		friend
			void swap(tape& x, tape& y) {
			std::swap(x.ext, y.ext);
		}

	};

	// from vbyte_descriptor.h //////////////////////////////////////////////
	struct vbyte_descriptor {
		typedef uint64_t value_type;
		enum { equality_preserving = true };
		enum { order_preserving = false };
		enum { prefixed_size = false };
		typedef std::bidirectional_iterator_tag iterator_category;

		uint8_t* encode(value_type x, uint8_t* dst) const {
			uint8_t tmp;
			while (x & ~value_type(0x7f)) {
				tmp = (x & 0x7f) | 0x80;
				*dst++ = tmp;
				x >>= 7;
			}
			*dst++ = (uint8_t)x; // VJA: removes warning
			return dst;
		}

		// we assume that all the byte sequences passed to these functions are well-formed;
		// a well-formed sequence of bytes is the one which is generated by the encode function
		// for some value of uint64_t

		size_t encoded_size(value_type value) const {
			const value_type one(1);
			if (value < (one << 7))       return size_t(1);
			if (value < (one << 14))      return size_t(2);
			if (value < (one << 21))      return size_t(3);
			if (value < (one << 28))      return size_t(4);
			if (value < (one << 35))      return size_t(5);
			if (value < (one << 42))      return size_t(6);
			if (value < (one << 49))      return size_t(7);
			if (value < (one << 56))      return size_t(8);
			if (value < (one << 63))      return size_t(9);
			return size_t(10);
		}
		// 10*7 > 64 > 9*7

		value_type decode(const uint8_t* p) const {
			value_type result = *p++;
			// we special case the frequent case of one-byte values
			if (result < 0x80) return result;
			result &= 0x7f;
			int shift = 7;
			value_type next = *p;
			while (next >= 0x80) {
				result |= (next & 0x7f) << shift;
				shift += 7;  // for well-formed sequences shift < 64
				next = *++p;
			}
			return result | (next << shift);
		}

		size_t size(const uint8_t* p) const {
			size_t result(1);
			while (*p >= 0x80) {
				++result;
				++p;
			}
			return result;
		}

		// attributes is almost identical to decode
		// the only difference is the return of the size
		std::pair<value_type, size_t> attributes(const uint8_t* p) const {
			const uint8_t* initial_p = p;
			value_type result = *p++;
			if (result < 0x80) return std::make_pair(result, 1);
			result &= 0x7f;
			uint32_t shift = 7;
			value_type next = *p++;
			while (next >= 0x80) {
				result |= ((next & 0x7f) << shift);
				shift += 7;
				next = *p++;
			}
			return std::make_pair(result | (next << shift), p - initial_p);
		}

		// we need to know the beginning of the encoded stream
		// since we search for the previous byte without a continuation bit
		// but need to be guarded if it does not exist
		// as usually done with iterators we assume that the range [origin, current) is valid
		// (pointers are iterators, after all)

		const uint8_t* previous(const uint8_t* origin,
								const uint8_t* current) const {
			if (current == origin) return current;
			--current;
			while (current != origin) {
				if (*--current < 0x80) return current + 1;
			}
			return current;
		}

		std::pair<value_type, size_t> attributes_backward(const uint8_t* origin,
														  const uint8_t* current) const {
			if (current == origin) return std::make_pair(value_type(0), size_t(0));
			const uint8_t* p = current - 1;
			value_type result = *p;
			while (p != origin) {
				value_type byte = *--p;
				if (byte < 0x80) return std::make_pair(result, current - (p + 1));
				result = (result << 7) | (byte & 0x7f);
			}
			return std::make_pair(result, current - p);
		}

		// we have the copy function for the sake of completeness;
		// it allows a client to copy a single encoded value
		// encoded integers can be copied as a group using std::copy(const uint8_t*, const uint8_t*, uint8_t*)
		// or, memcopy
		std::pair<const uint8_t*, uint8_t*>
			copy(const uint8_t* src, uint8_t* dst) const {
			uint8_t value;
			do {
				value = *src++;
				*dst++ = value;
			} while (value & 0x80);
			return std::make_pair(src, dst);
		}
	};
}

namespace xp {

	// changes made:
	// - the extent inherit from the Copier that some copier could maintain states while we still have empty 
	//   base class optimization
	// - in the extent:
	//   - removed new_block_start as allocate_bigger
	//   - renamed replace_start as relocate
	//   - added unguarded_ version of header access methods
	//   - removed _space suffix
	// - removed byte_ prefix from the extent methods as it is a byte extent.
	// - added allocator

	typedef std::uint8_t byte;

	struct byte_copier {
		byte* copy(const byte* first, const byte* last, byte* result) {
			return std::copy(first, last, result);
		}
		byte* move(const byte* first, const byte* last, byte* result) {
			return std::copy(first, last, result);
		}
		byte* move_backward(const byte* first, const byte* last, byte* result) {
			return std::copy_backward(first, last, result);
		}
		void clean_up(byte*, byte*) {}
	};

	// I do not feel the name Copier is adequate, as it also moves and cleans up.
	template<typename Metadata, typename Copier = byte_copier, typename Alloc = std::allocator<byte>>
	class extent : private Copier, private Alloc {
		using AllocTraits = std::allocator_traits<Alloc>;

		typedef typename Alloc allocator_type;
		typedef typename std::allocator_traits<allocator_type>::pointer pointer;
		typedef typename std::allocator_traits<allocator_type>::const_pointer const_pointer;
		typedef typename std::allocator_traits<allocator_type>::size_type size_type;
		typedef typename std::allocator_traits<allocator_type>::difference_type difference_type;

		pointer start;

		struct header_t {
			size_type capacity;
			size_type size;
			Metadata metadata;
		};
		header_t* unguarded_header() {
			return reinterpret_cast<header_t*>(start)-1;
		}
		const header_t* unguarded_header() const {
			return reinterpret_cast<const header_t*>(start)-1;
		}
		size_type unguarded_size() const {
			return unguarded_header()->size;
		}
		size_type unguarded_capacity() const {
			return unguarded_header()->capacity;
		}

		header_t* allocate(size_type n) {
			return reinterpret_cast<header_t*>(AllocTraits::allocate(get_allocator(), n + sizeof(header_t)));
		}
		void deallocate(header_t* p) {
			AllocTraits::deallocate(get_allocator(), reinterpret_cast<pointer>(p), p->capacity + sizeof(header_t));
		}
		void destroy(header_t* h) {
			(&h->metadata)->~Metadata();
		}
		void relocate(pointer p) {
			if (start) {
				auto h = unguarded_header();
				start = p;
				destroy(h);
				deallocate(h);
			} else {
				start = p;
			}
		}

		pointer initialize_storage(size_type n) {
			auto h = allocate(n);
			h->capacity = n;
			h->size = 0;
			new(&h->metadata) Metadata();
			return reinterpret_cast<pointer>(h+1);
		}

		void reallocate(size_type n) {
			auto h = unguarded_header();
			auto p = allocate(n);
			p->capacity = n;
			p->size = h->size;

			pointer data = reinterpret_cast<pointer>(p + 1);
			move(start, start + h->size, data);

			new(&h->metadata) Metadata(std::move_if_noexcept(h->metadata));

			start = data;
			deallocate(h);
		}
		void grow(size_type additional) {
			auto n = unguarded_capacity();
			reallocate(n + std::max(n, additional));
		}
		void grow(size_type additional, size_type offset) {
			auto h = unguarded_header();
			auto n = h->capacity;
			n += std::max(n, additional);
			auto p = allocate(n);
			p->capacity = n;
			p->size = h->size;

			pointer data = reinterpret_cast<pointer>(p + 1);
			move(start, start + offset, data);
			move(start, start + h->size, data + offset + additional);

			new(&h->metadata) Metadata(std::move_if_noexcept(h->metadata));

			start = data;
			deallocate(h);
		}

	public:
		extent() : start(nullptr) {}
		extent(const extent& x) : start(nullptr) {
			if (!x.empty()) {
				auto n = x.unguarded_size();
				auto h = allocate(n);
				h->capacity = n;
				h->size = n;
				start = reinterpret_cast<pointer>(h + 1);

				copy(x.start, x.start  + n, start);

				new(h&->metadata) Metadata(*x.unguarded_header()->metadata);
			}
		}
		~extent() { relocate(nullptr); }

		extent& operator=(const extent& x) {
			if (&x != this) {
				extent tmp(x);
				swap(*this, tmp);
			}
			return *this;
		}
		friend void swap(extent& x, extent& y) {
			std::swap(x.start, y.start);
		}

		const allocator_type& get_allocator() const noexcept {
			return *static_cast<const Alloc*>(this);
		}
		allocator_type& get_allocator() noexcept {
			return *static_cast<Alloc*>(this);
		}

		pointer begin() { return start; }
		const_pointer begin() const { return start; }
		pointer end() { return start + size(); }
		const_pointer end() const { return start + size(); }

		pointer data() { return start; }
		const_pointer data() const { return start; }

		Metadata* metadata() {
			return start ? &(unguarded_header()->metadata) : nullptr;
		}
		const Metadata* metadata() const {
			return start ? &(unguarded_header()->metadata) : nullptr;
		}

		size_type total_byte_size() const {
			return start ? unguarded_capacity() + sizeof(header_t) : size_type(0);
		}

		size_type size() const {
			return start ? unguarded_size() : size_type(0);
		}
		size_type capacity() const {
			return start ? unguarded_capacity() : size_type(0);
		}

		bool empty() const { return start == nullptr; }

		size_type remaining_capacity() const {
			return start ? unguarded_capacity() - unguarded_size() : size_type(0);
		}
		void remaining_capacity(size_type n) {
			if (remaining_capacity() == n)
				return;

			if (start) {
				reallocate(unguarded_size() + n);
			} else {
				start = n ? initialize_storage(n) : nullptr;
			}
		}

		template <typename Writer>
		pointer insert(pointer position, size_t inserted_byte_size, Writer writer) {
			if (!inserted_byte_size) return position;
			if (remaining_capacity() < inserted_byte_size) {
				size_t offset = position - start;
				grow(inserted_byte_size, offset);
				position = start + offset;
			} else {
				pointer last = end();
				pointer p = last + inserted_byte_size;
				move_backward(position, last, p);
			}
			writer(position);
			unguarded_header()->size += inserted_byte_size;
			return position;
		}

		template <typename Writer>
		pointer insert(size_t inserted_byte_size, Writer writer) {
			if (!inserted_byte_size) return end();
			if (remaining_capacity() < inserted_byte_size) {
				grow(inserted_byte_size);
			}
			pointer last = end();
			writer(last);
			unguarded_header()->size += inserted_byte_size;
			return last;
		}

		// Done in a way that is safe for concurrent readers if the erase is at the end.
		pointer erase(pointer first, size_t erased_byte_size) {
			if (erased_byte_size) {
				pointer last = first + erased_byte_size;
				auto n = unguarded_header()->size - erased_byte_size;
				if (!n) {
					clean_up(first, last);
					relocate(nullptr);
					return nullptr;
				} else {
					move(last, end(), first);
					unguarded_header()->size = n;
					if (end() < last) clean_up(end(), last);
				}
			}
			return first;
		}
	};

	template<typename Descriptor>
	class tape : private Descriptor {
		struct metadata_t {
			std::size_t number_of_elements;
		};
		extent<metadata_t> ext;

		std::size_t& number_of_elements() { // only safe when non-empty
			return ext.metadata()->number_of_elements;
		}
		const std::size_t& number_of_elements() const { // only safe when non-empty
			return ext.metadata()->number_of_elements;
		}

	public:
		typedef Descriptor descriptor_type;
		typedef typename Descriptor::value_type value_type;
		typedef std::size_t size_type;

		bool empty() const { return ext.empty(); }
		
		size_type size() const { return empty() ? size_type(0) : number_of_elements(); }
		// returns (a conservative estimate of) the number of values the tape can hold
		// without reallocation
		size_type estimated_capacity() const {
			return size() + (ext.remaining_byte_capacity() / sizeof(value_type));
		}
	};

}

namespace test {
	struct empty_t {};

	struct B {
		char c;
	};

	struct D : B, empty_t {};
}

TESTBENCH()

TEST(check_empty_base_optimization) {
	using namespace test;
	VERIFY_EQ(sizeof(D), sizeof(B));
}

TEST(check_storage) {
	using namespace xp;

	extent<int> ext;
	VERIFY_EQ(1U, sizeof(std::allocator<byte>));
	SKIP_EQ(sizeof(void*), sizeof(ext), "Damn you, Visual C++! Code boat with > 1 empty base class.");
	VERIFY(ext.empty());

	ext.remaining_capacity(16);
	VERIFY_EQ(16U, ext.capacity());
}

TESTFIXTURE(tape)
