#ifndef __HASH_TABLE__
#define __HASH_TABLE__

#include <iostream>
#include <vector>

#include "../../HashFunctions/AmplifiedHashFunction/AmplifiedHashFunction.hpp"
#include "../LinkedList/LinkedList.hpp"

// It will be a static hash table
// It represents one of the many hash tables of LSH_Structure.
class HashTable{
private:
    const size_t          buckets_num;
    LinkedList*           buckets;
    AmplifiedHashFunction hashFunction;

public:
    HashTable(const size_t& buckets_num, const size_t& hash_fns_num, 
            const double& w_size, const size_t& s_vals_sz, 
            const int& m, const int& M);
    
    LinkedList_Iterator get_neighbors(const std::vector<double>&, const std::vector<int>&);
    void                add_vector(const std::vector<double>*&, const int&, const std::vector<int>&);

    void printBucketSize() const{
        for(size_t i = 0; i < buckets_num; i++){
            std::cout << buckets[i].getSize() << std::endl;
        }
    }

    ~HashTable(){
        delete[] buckets;
    };
};

#endif