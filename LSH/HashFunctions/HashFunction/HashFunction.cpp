#include <cassert>
#include <cmath>

#include <iostream>

#include <random>
#include <vector>

#include "./HashFunction.hpp"
#include "../../MathUtils/MathUtils.hpp"

using namespace std;

HashFunction::HashFunction(const double& w_size, const size_t& s_vals_sz,
                const int& m, const unsigned int& M) : window_size(w_size),
                s_values_size(s_vals_sz), power_factor(m), modulo_factor(M)
{
    std::random_device          rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937                gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    uniform_int_distribution<> dist(0,w_size - 1); // because s_values are integers

    // Initializing the s_i values
    for(int i = 0; i < s_vals_sz;i++){
        s_values.push_back((double) dist(gen));
    }
}

int HashFunction::get_value(const vector<double>& query_vector,const vector<int>& mod_exp_power_factors)
{
    vector<int> a_vector;

    assert(query_vector.size() == s_values_size);

    // converting s vector to "a" vector
    for(int i = 0; i < s_values_size; i++){
        double a_i = (query_vector[i] - s_values[i]) / window_size;

        a_vector.push_back((int) floor(a_i));
    }

    assert(a_vector.size() == s_values_size);

    // we know that (a_1+...+a_n)mod K = (a_1 mod K +...+ a_n mod K) mod K
    int dimensionality = a_vector.size();
    int value          = mod(a_vector[dimensionality - 1], modulo_factor);

    // Finding the value of hash function
    for(int i = 0; i < dimensionality - 1; i++){
        int factor1 = mod_exp_power_factors[i];
        int factor2 = mod(a_vector[i], modulo_factor);

        value += mod(factor1 * factor2, modulo_factor);
    }

    value = mod(value, modulo_factor);

    assert(value >= 0);
    return value;
}
