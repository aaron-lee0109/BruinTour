#ifndef HASHMAP_H
#define HASHMAP_H

#include <string>
#include <vector>
#include <algorithm>
#include <functional>

template <typename T>
class HashMap
{
public:
	HashMap(double max_load = 0.75) // constructor
		: numItems(0), maxLoadFactor(max_load) {
		buckets.resize(10);
		if (maxLoadFactor <= 0)
			maxLoadFactor = 0.75;
	}

	// deleting copying and assignment
	HashMap(const HashMap&) = delete;
	HashMap& operator=(const HashMap&) = delete;

	~HashMap() {
		// destructor; deletes all of the items in the hashmap
	}

	int size() const {
		// return the number of associations in the hashmap
		return numItems;
	}

	// The insert method associates one item (key) with another (value).
	// If no association currently exists with that key, this method inserts
	// a new association into the hashmap with that key/value pair. If there is
	// already an association with that key in the hashmap, then the item
	// associated with that key is replaced by the second parameter (value).
	// Thus, the hashmap must contain no duplicate keys.
	void insert(const std::string& key, const T& value) {
		if (static_cast<double>(numItems + 1) / buckets.size() > maxLoadFactor)
			rehash();

		size_t index = std::hash<std::string>()(key) % buckets.size();
		std::vector<KeyValuePair>& bucket = buckets[index];

		for (int i = 0; i < bucket.size(); i++) {
			if (bucket[i].key == key) {
				// Key exists, update
				bucket[i].value = value;
				return;
			}
		}
		// Key doesn't exist, add it
		bucket.push_back({ key, value });
		numItems++;
	}
	// If no association exists with the given key, return nullptr; otherwise,
	// return a pointer to the value associated with that key. This pointer can be
	// used to examine that value or modify it directly within the map.
	const T* find(const std::string& key) const {
		size_t index = std::hash<std::string>()(key) % buckets.size();
		const std::vector<KeyValuePair>& bucket = buckets[index];

		for (int i = 0; i < bucket.size(); i++) {
			if (bucket[i].key == key) {
				// if key found, return point to value
				return &(bucket[i].value);
			}
		}
		// if key not found, return nullptr
		return nullptr;
	}
	// Defines the bracket operator for HashMap, so you can use your map like this:
	// your_map["david"] = 2.99;
	// If the key does not exist in the hashmap, this will create a new entry in
	// the hashmap and map it to the default value of type T. Then it will return a
	// reference to the newly created value in the map.
	T& operator[](const std::string& key) {
		T* valPtr = find(key);
		if (valPtr != nullptr)
			// if key found 
			return *valPtr;
		else {
			// if key not found
			insert(key, T());
			return *find(key);
		}
	}
	// If no association exists with the given key, return nullptr; otherwise,
	// return a pointer to the value associated with that key. This pointer can be
	// used to examine that value or modify it directly within the map.
	T* find(const std::string& key) {
		const auto& hm = *this;
		return const_cast<T*>(hm.find(key));
	}
private:
	struct KeyValuePair {
		std::string key;
		T value;
	};
	std::vector<std::vector<KeyValuePair>> buckets;
	int numItems;
	double maxLoadFactor;

	void rehash() {
		size_t newSize = buckets.size() * 2;
		std::vector<std::vector<KeyValuePair>> newBuckets(newSize);

		for (auto& bucket : buckets) {
			for (auto& pair : bucket) {
				size_t index = std::hash<std::string>()(pair.key) % newSize;
				newBuckets[index].push_back(pair);
			}
		}
		buckets = std::move(newBuckets);
	}
};

#endif //HASHMAP_H