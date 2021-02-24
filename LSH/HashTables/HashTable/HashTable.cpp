#include "./HashTable.hpp"
#include "../../MathUtils/MathUtils.hpp"

using namespace std;

HashTable::HashTable(const size_t& bucks_num, const size_t& hash_fns_num,
            const double& w_size, const size_t& s_vals_sz, const int& m, 
            const int& M) : buckets_num(bucks_num), 
            hashFunction(hash_fns_num, w_size, s_vals_sz, m, M)
{
    buckets = new LinkedList[buckets_num];
}

// Getting the corresponding bucket
LinkedList_Iterator HashTable::get_neighbors(const vector<double>& query_vector,const vector<int>& mod_exp_power_factors)
{
    string hash_fn_value = hashFunction.get_value(query_vector, mod_exp_power_factors);

    int index = mod_str(hash_fn_value, buckets_num);

    return buckets[index].begin();
}

void HashTable::add_vector(const std::vector<double>*& vec, const int& id, const vector<int>& mod_exp_power_factors)
{
    string hash_fn_value = hashFunction.get_value(*vec, mod_exp_power_factors);

    int index = mod_str(hash_fn_value, buckets_num);

    buckets[index].add(vec, id);
}