#include <iostream>
#include <cassert>
#include <cmath>

#include "DistanceMetrics.hpp"

using namespace std;

double ManhattanDistance(const vector<double>& v1, const vector<double>& v2)
{
    assert(v1.size() == v2.size());

    double result = 0.0;

    size_t dimensionality = v1.size();

    for(int i = 0; i < dimensionality; i++)
        result += abs(v1[i] - v2[i]);
    
    if(!(result >= 0))
        std::cout <<"DIST " << result << std::endl;

    assert(result >= 0);
    
    return result;
}
