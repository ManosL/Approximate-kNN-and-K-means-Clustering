#ifndef HYPERCUBE_H
#define HYPERCUBE_H

#include <iostream>
#include <vector>
#include "../RangeQueryStructure/rangeQueryStructure.hpp"
#include "../LSH/MathUtils/MathUtils.hpp"
#include "../LSH/HashTables/LSH_Structure/LSH_Structure.hpp"
#include "../LSH/HashFunctions/HashFunction/HashFunction.hpp"
#include "../LSH/HashTables/LinkedList/LinkedList.hpp"

using namespace std;

class Hypercube : public rangeQueryStructure
{
    int number;    // No. of buckets
    int probes;
    int M_points;
    int k;
    int** hash_array;

    const double           window_size;
    const size_t           s_values_size;
    const int              power_factor; // Check its type(might be real)
    const unsigned int     modulo_factor;

    // Pointer to an array containing buckets
    vector<Node> *table;
    vector<HashFunction> hash_fun_vec;
    //modulo factors
    vector<int>            mod_exp_power_factors;

    double                 (*distance_fn)(const std::vector<double>&, const std::vector<double>&);
public:
    Hypercube(int, double (*distance_fun)(const std::vector<double>&, const std::vector<double>&),
                    const double&, const size_t&,
                    const int&, const unsigned int&, const int&, const int&, const int&);  // Constructor

    ~Hypercube();
    // inserts a key into hash table
    void insert(Node query_vector);

    // deletes a key from hash table
    void deleteItem(int index);

    string get_querytag(vector<double>);

    void datasetInitialize(const Dataset&);

    int* neighbors(string, int);

    void displayHypercube();

    void displayNeighbors(int* N);

    const vector<int> rangeSearch(const vector<double>&, const double&);

    vector<NNQueryReturn> kNearestNeighbors(const vector<double>&, const int&);

    void evaluateQuerySet(const Dataset&, ifstream&,
                            ofstream&, const int&, const double&);
};


int binaryStringToDec(string);

#endif //HYPERCUBE_H