#ifndef __HASH_FUNCTION__
#define __HASH_FUNCTION__

#include <vector>

class HashFunction{
private:
    const double           window_size;
    const size_t           s_values_size;
    const int              power_factor; // Check its type(might be real)
    const unsigned int     modulo_factor;
    std::vector<double>    s_values;

public:
    HashFunction(const double&,const size_t&,const int&,const unsigned int&);
    int get_value(const std::vector<double>& query_vector,const std::vector<int>& mod_exp_power_factors);
    ~HashFunction(){};
};

#endif