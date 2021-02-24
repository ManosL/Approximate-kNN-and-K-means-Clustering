#include <iostream>
#include <cassert>
#include <string>
#include <vector>

#include "AmplifiedHashFunction.hpp"

using namespace std;

// Just creates its hash functions.
AmplifiedHashFunction::AmplifiedHashFunction(const size_t& hash_fns_num,
            const double& w_size, const size_t& s_vals_sz, const int& m,
            const int& M) : hash_functions_num(hash_fns_num)
{
    for(int i = 0; i < hash_fns_num; i++){
        hash_functions.push_back(HashFunction(w_size, s_vals_sz, m, M));
    }
}

// Returns the result after concatenating the values of the hash function
const string AmplifiedHashFunction::get_value(vector<double> query_vector, const vector<int>& mod_exp_power_factors)
{
    string concat_result = "";

    for(int i = 0; i < hash_functions_num; i++){
        concat_result += to_string(hash_functions[i].get_value(query_vector, mod_exp_power_factors));
    }

    return concat_result;
}
