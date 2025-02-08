#pragma once
#include <utility>

namespace rohit {

	template<class _Value, size_t N> class SimpleStore
	{
		_Value store[N];
		size_t index;
	public:
		SimpleStore() : store(), index(0) {}

		inline size_t size() const { return index; }

		inline const _Value *begin() const { return store; }
		inline const _Value *end() const { return store + index; }

		inline _Value &operator[](size_t index) { return store[index]; }
		inline const _Value operator[](size_t index) const { return store[index]; }
		inline _Value &back_uncheck() { return store[index - 1]; }
		inline const _Value &back_uncheck() const { return store[index - 1]; }

		inline void push_back_uncheck(const _Value &val) { 
			assert(index != N);
			store[index++] = std::move(val);
		}

		inline void pop_back_uncheck() { 
			assert(index != 0);
			index--;
		}
		inline size_t capacity() const { return N; }
		
		inline bool empty() const { return index == 0; }
		inline bool isFull() const { return index >= N; }

		inline void clear() { index = 0; }
	};

}