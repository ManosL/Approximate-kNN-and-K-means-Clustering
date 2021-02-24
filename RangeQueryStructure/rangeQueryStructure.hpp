#ifndef __R_QUERY_STRUCT__
#define __R_QUERY_STRUCT__

#include <vector>

#include "../Dataset/Dataset.hpp"

class rangeQueryStructure {
public:
    virtual const std::vector<int> rangeSearch(const std::vector<double>& query_vec, const double& radius) = 0;
    virtual void datasetInitialize(const Dataset& data) = 0;
};

#endif