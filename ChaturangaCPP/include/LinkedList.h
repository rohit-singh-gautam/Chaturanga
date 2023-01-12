#pragma once
#include <utility>
#include <memory>

namespace rohit {
	template<class _Value> struct fixed_list_entry {
		_Value val;
		fixed_list_entry *prev = nullptr;
		fixed_list_entry *next = nullptr;
	};

	template<class _Value, class _Entry = fixed_list_entry<_Value>> class _MyIter {
		_Entry *_Ptr;

	public:
		inline _MyIter(_Entry *_Ptr) : _Ptr(_Ptr) {}

		inline _MyIter &operator--() {
			_Ptr = _Ptr->prev;
			return *this;
		}

		inline _MyIter operator--(int) {
			_Entry * _Temp = _Ptr;
			--*this;
			return _Temp;
		}

		inline _MyIter &operator++() {
			_Ptr = _Ptr->next;
			return *this;
		}

		inline _MyIter operator++(int) {
			_Entry *_Temp = _Ptr;
			++*this;
			return _Temp;
		}

		inline void detach() {
			_Ptr->next->prev = _Ptr->prev;
			_Ptr->prev->next = _Ptr->next;
		}

		// Will attach before _Where
		inline void attach(_MyIter _Where) {
			_Ptr->prev = _Where._Ptr->prev;
			_Ptr->next = _Where._Ptr;
			_Ptr->prev->next = _Ptr;
			_Where._Ptr->prev = _Ptr;
		}

		inline void move(_MyIter _Where) {
			detach();
			attach(_Where);
		}

		inline bool operator==(const _MyIter rhs) const { return _Ptr == rhs._Ptr; }
		inline bool operator!=(const _MyIter rhs) const { return _Ptr != rhs._Ptr; }

		inline _Value &operator*() { return _Ptr->val; }
		inline const _Value &operator*() const { return _Ptr->val; }
		inline _Value *operator->() { return &_Ptr->val; }
		inline const _Value *operator->() const { return &_Ptr->val; }

		inline _Entry *getPtr() { return _Ptr; }
		inline const _Entry *getPtr() const { return _Ptr; }

		template<typename, typename> friend class fixed_list;
	};

	template<class _Value, class _Entry = fixed_list_entry<_Value>> class _MyIter_const {
		const _Entry * _Ptr;

	public:
		inline _MyIter_const(const _Entry * _Ptr) : _Ptr(_Ptr) {}

		inline const _MyIter_const &operator--() {
			_Ptr = _Ptr->prev;
			return *this;
		}

		inline const _MyIter_const operator--(int) {
			_Entry *_Temp = _Ptr;
			--*this;
			return _Temp;
		}

		inline const _MyIter_const &operator++() {
			_Ptr = _Ptr->next;
			return *this;
		}

		inline const _MyIter_const operator++(int) {
			_Entry *_Temp = _Ptr;
			++*this;
			return _Temp;
		}

		inline bool operator==(const _MyIter_const &rhs) const { return _Ptr == rhs._Ptr; }
		inline bool operator!=(const _MyIter_const &rhs) const { return !(*this == rhs); }

		inline const _Value &operator*() const { return _Ptr->val; }
		inline const _Value *operator->() const { return &_Ptr->val; }

		inline const _Entry *getPtr() const { return _Ptr; }

		template<typename, typename> friend class fixed_list;
	};

	template<class _Value, class _Entry = fixed_list_entry<_Value>> class fixed_list {
	public:
		using iterator=_MyIter<_Value, _Entry>;
		using const_iterator=_MyIter_const<_Value, _Entry>;

	private:
		size_t capacity;
		std::unique_ptr<_Entry[]> store;
		size_t store_count;
		_Entry first;

	public:
		inline fixed_list(size_t capacity) : capacity{capacity}, store{std::make_unique<_Entry[]>(capacity)}, store_count{0} {
			first.next = first.prev = &first;
		}

		// State
		inline bool isFull() { return capacity == store_count; }
		inline bool isEmpty() { return store_count == 0; }
		inline void clear() {
			store_count = 0;
			first.next = first.prev = &first;
		}

		// Retriving element
		inline _Value &front() { return first.next->val; }
		inline _Value &back() { return first.prev->val; }
		inline void push_front(const _Value &val) {
			if (store_count == capacity) {
				iterator iter = last();
				iter._Ptr->val = std::move(val);
				iter.move(begin());
			}
			else {
				iterator iter = { &store[store_count++] };
				iter.attach(begin());
			}
		}

		inline void push_front_unchecked(const _Value &val) {
			iterator iter = { &store[store_count++] };
			iter._Ptr->val = std::move(val);
			auto beginIt = begin();
			iter.attach(beginIt);
		}

		inline void push_front_unchecked_full(const _Value &val) {
			iterator iter = last();
			iter._Ptr->val = std::move(val);
			auto beginIt = begin();
			iter.move(beginIt);
		}

		// Iterator operation
		inline iterator begin() { return { first.next }; }
		inline iterator end() { return { &first }; }
		inline iterator last() { return { first.prev }; }

		inline void to_begin(iterator &iter) { iter.move(begin()); }
	};

	template<class _Value, class _Entry = fixed_list_entry<_Value>> class fixed_list_simple {
	public:
		typedef _MyIter<_Value, _Entry> iterator;
		typedef _MyIter_const<_Value, _Entry> const_iterator;

	private:
		_Entry first;

	public:
		inline fixed_list_simple() {
			first.next = first.prev = &first;
		}

		// State
		inline bool isEmpty() const { return begin() == end(); }
		inline void clear() { first.next = first.prev = &first;	}

		// Adding element
		inline void push_front(_Entry *entry) {
				iterator iter = { entry };
				iter.attach(begin());
		}

		// Retriving element
		inline _Value &front() { return first.next->val; }
		inline _Value &back() { return first.prev->val; }

		inline const _Value &front() const { return first.next->val; }
		inline const _Value &back() const { return first.prev->val; }

		// Iterator operation
		inline iterator begin() { return { first.next }; }
		inline iterator end() { return { &first }; }
		inline iterator last() { return { first.prev }; }
		inline const_iterator begin() const { return { first.next }; }
		inline const_iterator end() const { return { &first }; }
		inline const_iterator last() const { return { first.prev }; }

		inline void to_begin(iterator iter) { if (iter != begin()) iter.move(begin()); }
	};

	template<class _Value, class _Entry = fixed_list_entry<_Value>> std::ostream& operator<<(std::ostream& os, const fixed_list_simple<_Value, _Entry> &list) {
		os << "<<";
		if (list.isEmpty()) os << "null";
		else {
			auto it = list.begin();
			os << *it;
			while (++it != list.end()) {
				os << "," << *it;
			}
		}
		os << ">>";
		return os;
	}

}
