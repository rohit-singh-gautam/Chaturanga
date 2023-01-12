#pragma once
#include "LinkedList.h"
#include <unordered_map>

namespace rohit {

	template<class _Key, class _Value> class lru_map
	{
		using iterator = typename fixed_list<std::pair<_Key, _Value>>::iterator;
		using _MyPair = std::pair<_Key, _Value>;
		using _MyMap = std::unordered_map<_Key, iterator>;
		size_t capacity;
		fixed_list<_MyPair> linkedList;
		_MyMap hashMap;

	public:
		inline lru_map(size_t capacity) : capacity(capacity), linkedList(capacity), hashMap(capacity) {}
		inline ~lru_map() {}

		inline void clear() { linkedList.clear(); hashMap.clear(); }
		inline void push_front(_Key key, _Value value) {
			if (linkedList.isFull()) {
				hashMap.erase(linkedList.back().first);
				linkedList.push_front_unchecked_full(std::make_pair(key, value));
			} else {
				linkedList.push_front_unchecked(std::make_pair(key, value));
			}
			hashMap.insert(std::make_pair(key, linkedList.begin()));
		}

		inline iterator end() { return linkedList.end(); }
		inline iterator find(_Key key) {
			auto it = hashMap.find(key);
			if (it == hashMap.end()) {
				return linkedList.end();
			}
			else {
				auto listIt = it->second;
				auto beginIt = linkedList.begin();
				listIt.move(beginIt);
				return listIt;
			}
		}

		inline iterator find_no_move(_Key key) {
			auto it = hashMap.find(key);
			if (it == hashMap.end()) {
				return linkedList.end();
			}
			else {
				auto listIt = it->second;
				return listIt;
			}
		}

		inline void hasKey(_Key key) { return hashMap.find(key) != hashMap.end(); }
		inline _Value& operator[](_Key key) {
			return hashMap[key]->second;
		}
	};
}

