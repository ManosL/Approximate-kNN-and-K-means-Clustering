#ifndef __DATASET__
#define __DATASET__

#include <fstream>
#include <vector>

// This structure is used just to hold some vectors.
class Dataset {
private:
    std::vector<std::vector<double> > pictures;

public:
    Dataset(std::ifstream&);

    int    getRows() const    { return pictures.size(); }
    int    getColumns() const { return pictures[0].size(); }
    
    int    LSH_findPowerFactor() const;
    double LSH_getWindowSize(double (*dist_fn)(const std::vector<double>&, 
                    const std::vector<double>&), const int& picts_num=1000) const; 

    std::vector<double> getKNearestDistances(double (*dist_fn)(const std::vector<double>&, const std::vector<double>&),
                                            const std::vector<double>&, const int&) const;

    const std::vector<double>& operator[](const int& i) const {
        return pictures[i];
    }

    ~Dataset(){}
};

#endif
