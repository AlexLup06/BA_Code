#include "LRUCache.h"

#include <omnetpp.h>

#include <cstdio>
#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

LRUCache::LRUCache(int capacity) {
    this->capacity = capacity;
}

std::list<int> LRUCache::getObjects() {
    return lruList;
}

int LRUCache::get(int key) {
    if (cache.find(key) == cache.end())
        return -1;

    // Move the accessed element to the front of the LRU list
    lruList.splice(lruList.begin(), lruList, cache[key].second);

    return cache[key].first;
}

int LRUCache::set(int key, long value) {
    if (cache.find(key) != cache.end()) {
        // Update the value of the existing key
        cache[key].first = value;
        // Move the accessed element to the front of the LRU list
        lruList.splice(lruList.begin(), lruList, cache[key].second);
    } else {
        // Add the new key-value pair
        lruList.push_front(key);
        cache[key] = { value, lruList.begin() };
    }

    return lruList.size() > capacity ? lruList.back() : -1;
}

void LRUCache::remove(int key) {
    if (cache.find(key) != cache.end()) {
        lruList.erase(cache[key].second);
        cache.erase(key);
    }
}

bool LRUCache::isFull() {
    return lruList.size() > capacity;

}
