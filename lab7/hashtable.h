#ifndef HASHTABLE_H
#define HASHTABLE_H

// Project Identifier: 2C4A3C53CD5AD45A7BEA3AE5356A3B1622C9D04B

// INSTRUCTIONS:
// fill out the methods in the class below.

// You may assume that the key and value types can be copied and have default
// constructors.

// You can also assume that the key type has (==) and (!=) methods.

// You may assume that Hasher is a functor type with a
// size_t operator()(const Key&) overload.

// The key/value types aren't guaranteed to support any other operations.

// Do not add, remove, or change any of the class's member variables.
// The num_deleted counter is *optional*. You might find it helpful, but it
// is not required to pass the lab assignment.

// Do not change the Bucket type.

// SUBMISSION INSTRUCTIONS:
// Submit this file, by itself, in a .tar.gz.
// Other files will be ignored.

#include <cstdint>
#include <functional> // where std::hash lives
#include <vector>
#include <cassert> // useful for debugging!

using namespace std;

// A bucket's status tells you whether it's empty, occupied, 
// or contains a deleted element.
enum class Status : uint8_t {
    Empty,
    Occupied,
    Deleted
};

template<typename K, typename V, typename Hasher = std::hash<K>>
class HashTable {
    // used by the autograder; do not change/remove.
    friend class Verifier;
public:

    // A bucket has a status, a key, and a value.
    struct Bucket {
        // Do not modify Bucket.
        Status status = Status::Empty;
        K key;
        V val;
    };

    HashTable() {
        // TODO: a default constructor (possibly empty).

        // You can use the following to avoid implementing rehash_and_grow().
        // However, you will only pass the AG test cases ending in _C.
        // To pass the rest, start with at most 20 buckets and implement rehash_and_grow().
        buckets.resize(20);
    }

    size_t size() const {
        return num_elements;
    }

    // returns a reference to the value in the bucket with the key, if it
    // already exists. Otherwise, insert it with a default value, and return
    // a reference to the resulting bucket.
    V& operator[](const K& key) {
        Hasher hasher;
        size_t hashIndex = hasher(key) % buckets.size();
        size_t i = findEmpty(hashIndex, key);
        V value;
        if(buckets[i].status != Status::Occupied){
            insert(key, value);
        }
        hashIndex = hasher(key) % buckets.size();
        i = findEmpty(hashIndex, key);

        return buckets[i].val;
    }

    // insert returns whether inserted successfully
    // (if the key already exists in the table, do nothing and return false).
    bool insert(const K& key, const V& val) {
        Hasher hasher;
        size_t hashIndex = hasher(key) % buckets.size();
        size_t i = findEmpty(hashIndex, key);

        if(buckets[i].status != Status::Occupied){
            buckets[i] = Bucket{ Status::Occupied, key, val};
            num_elements++;
        }
        else{
            return false;
        }

        if(double(num_elements) / double(buckets.size()) > 0.5){
            rehash_and_grow();
        }
        return true;
    }
    // erase returns the number of items remove (0 or 1)
    size_t erase(const K& key) {
        Hasher hasher;
        size_t hashIndex = hasher(key) % buckets.size();
        size_t i = findEmpty(hashIndex, key);
        if(buckets[i].status == Status::Occupied){
            buckets[i].status = Status::Deleted;
            num_elements--;
            return 1;
        }

        return 0;
    }

private:
    size_t num_elements = 0;
    size_t num_deleted = 0; // OPTIONAL: you don't need to use num_deleted to pass
    std::vector<Bucket> buckets;

    void rehash_and_grow() {
        
        vector<Bucket> temp = buckets;
        buckets.clear();
        buckets.resize(temp.size()*2);
        num_elements = 0;
        for(auto it : temp){
            if(it.status == Status::Occupied){
                insert(it.key, it.val);
            }
        }
        
    }

    // You can add methods here if you like.
    // TODO

    size_t findEmpty(size_t index, const K& key){
        int count = 0;
        int temp = index;
        while(buckets[index].status != Status::Empty && count < buckets.size()){
            if(buckets[index].status == Status::Occupied && buckets[index].key == key){
                return index;
            }
            index = (index+1) % buckets.size();
            count++;
        }

        count = 0;

        while(buckets[temp].status == Status::Occupied && count < buckets.size()){
            temp = (temp+1) % buckets.size();
            count++;
        }

        return temp;
    }
    
};

#endif // HASHTABLE_H