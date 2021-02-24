#ifndef __AMPLIFIED_HASH_FN__
#define __AMPLIFIED_HASH_FN__

#include <string>
#include <vector>

#include "../HashFunction/HashFunction.hpp"

class AmplifiedHashFunction{
private:
    const size_t              hash_functions_num;
    std::vector<HashFunction> hash_functions;

public:
    AmplifiedHashFunction(const size_t& hash_fns_num, const double& w_size, 
                        const size_t& s_vals_sz, const int& m, const int& M);

    const std::string get_value(std::vector<double> query_vector, const std::vector<int>& mod_exp_power_factors);

    ~AmplifiedHashFunction(){};
};

#endif