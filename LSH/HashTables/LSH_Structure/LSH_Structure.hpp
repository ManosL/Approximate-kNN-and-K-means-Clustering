#ifndef __LSH_STRUCT__
#define __LSH_STRUCT__

#include <cstdbool>
#include <fstream>
#include <vector>

#include "../../../Dataset/Dataset.hpp"
#include "../../HashFunctions/AmplifiedHashFunction/AmplifiedHashFunction.hpp"
#include "../HashTable/HashTable.hpp"
#include "../../../RangeQueryStructure/rangeQueryStructure.hpp"

struct NNQueryReturn{
    const std::vector<double>* neighbor_vec;
    double                     dist_from_query;
    int                        neighbor_id;

    NNQueryReturn(){}
    
    NNQueryReturn(const std::vector<double>& vec, const double& query_dist, 
                const int& id) : neighbor_vec(&vec), dist_from_query(query_dist),neighbor_id(id){} 
    
    bool operator<(const NNQueryReturn& other) const { return this->dist_from_query < other.dist_from_query; }
};

class LSH_Structure : public rangeQueryStructure{
private:
    const size_t                      hash_tables_number;
    HashTable**                       hash_tables;
    double                            (*distance_fn)(const std::vector<double>&, const std::vector<double>&);
    std::vector<int>                  mod_exp_power_factors;

    void add_vector(const std::vector<double>*& new_vector, const int& id){
        for(int i = 0; i < hash_tables_number; i++)
            hash_tables[i]->add_vector(new_vector, id, mod_exp_power_factors);
    }

    const std::vector<NNQueryReturn> kNearestNeighbors(const std::vector<double>& query_vec, const int& k);

public: 
    LSH_Structure(const size_t& hash_tables_num, double (*distance_fun)(const std::vector<double>&, const std::vector<double>&),
                const size_t& buckets_num, const size_t& hash_fns_num, 
                const double& w_size,  const size_t& s_vals_sz, const int& m);
    
    void datasetInitialize(const Dataset& data);

    const std::vector<int> rangeSearch(const std::vector<double>& query_vec, const double& radius);

    void evaluateQuerySet(const Dataset& data, std::ifstream& querySetFile, 
                    std::ofstream& outFile, const int& k, const double& radius);

    ~LSH_Structure(){
        for(int i = 0; i < hash_tables_number; i++)
            delete hash_tables[i];

        delete[] hash_tables;
    };
};

#endif