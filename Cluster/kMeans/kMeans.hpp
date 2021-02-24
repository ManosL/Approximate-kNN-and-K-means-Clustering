#ifndef __K_MEANS__
#define __K_MEANS__

#include <cassert>
#include <fstream>
#include <vector>

#include "../../Dataset/Dataset.hpp"
#include "../../RangeQueryStructure/rangeQueryStructure.hpp"

enum AssignMethod {CLASSIC, LSH, HYPERCUBE};

// This class will serve the same purpose as LSH_Structure

class kMeans{
private:
    struct clusteringResult {
        struct cluster{
            std::vector<double> centroid;
            std::vector<int>    img_ids;

            cluster(const std::vector<double>& center): centroid(center){}
        };

        std::vector<cluster> clusters;
        double               cluster_time;
        std::vector<double>  silhouettes;

        void find_silhouettes(const Dataset& vectors, 
                double (*distance_fn)(const std::vector<double>&, const std::vector<double>&));
    };
    
    Dataset              vectors;
    const int            clusters_number;
    const AssignMethod   clustering_method;
    double               (*distance_fn)(const std::vector<double>&, const std::vector<double>&);

    rangeQueryStructure* range_query_structure; // An abstract class that will be parent 
                                                // of LSH_STructure and hypercube structure

    // k-means++ implementation, will return the indexes of the
    // centroids.
    std::vector<int> centerInitialization() const;

    clusteringResult  LloydsAlgorithm() const;
    clusteringResult  ReverseAssignment() const;

    // This also can be moved to public if at the future
    // the programmer wants to handle the clusteringResult
    // structure
    clusteringResult find_clusters() const{ 
        if(clustering_method == CLASSIC){
            return LloydsAlgorithm();
        } else {
            assert((clustering_method == LSH) || (clustering_method == HYPERCUBE));
            return ReverseAssignment();
        }
    }

    void printResults(const kMeans::clusteringResult&, const bool&, std::ofstream&) const;

public:
    kMeans(std::ifstream& data_stream, const int& clusters_num, 
           double (*distance_fun)(const std::vector<double>&, const std::vector<double>&), 
           const int& hash_table_no, const int& hash_table_fns, const int& max_num_M_hc, 
           const int& hc_dims, const int& probes_no, const AssignMethod& assign_method);

    void solve(const bool& complete, std::ofstream& outstream){
        printResults(find_clusters(), complete, outstream);
    }

    ~kMeans(){
        if(range_query_structure) 
            delete range_query_structure; 
    }
};

#endif