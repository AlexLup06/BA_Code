#ifndef LRUCACHE_H
#define LRUCACHE_H

#include <unordered_map>
#include <deque>
#include <utility>
#include "definitions.h"
#include <string>
#include <list>

using namespace std;

class LRUCache {
private:
    int capacity;
    std::unordered_map<int, std::pair<int, std::list<int>::iterator>> cache;
    std::list<int> lruList;

public:
    LRUCache() :
            capacity(10) {
    }
    ;
    LRUCache(int capacity);

    int get(int key);
    int set(int key, long value);
    void remove(int key);
    bool isFull();
    std::list<int> getObjects();
};

#endif // LRUCACHE_H
