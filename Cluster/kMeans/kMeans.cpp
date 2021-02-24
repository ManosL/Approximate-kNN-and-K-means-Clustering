#include <algorithm>
#include <cassert>
#include <ctime>
#include <fstream>
#include <iostream>
#include <random>
#include <set>

#include "kMeans.hpp"
#include "../../LSH/HashTables/LSH_Structure/LSH_Structure.hpp"
#include "../../hyperCube/hypercube.h"
#include "../../LSH/MathUtils/MathUtils.hpp"

#define WINDOW_SIZE 1500 

#define MULTIPLY_FACTOR 6

#define MAX_ITERATIONS 1000

#define CONV_MAX_CHANGE 200

using namespace std;

// Initializing the necessary clustering parameters and fills the structure.
kMeans::kMeans(ifstream& data_stream, const int& clusters_num, 
           double (*distance_fun)(const std::vector<double>&, const std::vector<double>&),
           const int& hash_table_no, const int& hash_table_fns, const int& max_num_M_hc,
           const int& hc_dims, const int& probes_no, const AssignMethod& assign_method) : vectors(data_stream),
           clusters_number(clusters_num), clustering_method(assign_method), distance_fn(distance_fun)
{
    switch(assign_method){
        case CLASSIC:
            range_query_structure = NULL;
            break;
        case LSH:
            range_query_structure = new LSH_Structure(hash_table_no, distance_fun,
                                        vectors.getRows() / 32, hash_table_fns, 
                                        WINDOW_SIZE * MULTIPLY_FACTOR,
                                        vectors.getColumns(), vectors.LSH_findPowerFactor());
            break;
        case HYPERCUBE:
            range_query_structure = new Hypercube(pow(2,hc_dims), distance_fun, MULTIPLY_FACTOR*WINDOW_SIZE, 
                                        vectors.getColumns(), vectors.LSH_findPowerFactor(), 
                                        vectors.getRows() / 8, probes_no, hc_dims, max_num_M_hc);
            break;
        default:
            cout << "Wrong method provided" << endl;
            assert(0==1);
    }

    if(range_query_structure != NULL)
        range_query_structure->datasetInitialize(vectors);
}

// Initializing the centers with k-means++
vector<int> kMeans::centerInitialization() const{
    std::random_device          rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937                gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    uniform_int_distribution<>  dist(0, vectors.getRows() - 1); // because s_values are integers

    vector<int> centers;

    centers.push_back(dist(gen));

    for(int i = 1; i < clusters_number; i++){
        vector<int>    cand_center_ids;
        vector<double> partial_sums;
        partial_sums.push_back(0.0);
        
        // Finding the minimum distance between the current point
        // and a centroid
        for(int j = 0; j < vectors.getRows(); j++){
            double min_dist     = -1.0;
            bool   min_dist_set = false;

            for(int k = 0; k < centers.size(); k++){
                if(j == centers[k]){
                    min_dist_set = false; 
                    break;
                }

                double cand_dist = distance_fn(vectors[j], vectors[centers[k]]);

                if((!min_dist_set) || (cand_dist < min_dist)){
                    min_dist     = cand_dist;
                    min_dist_set = true;
                }
            }

            if(!min_dist_set) continue;

            // Creating the probability distribution
            partial_sums.push_back(partial_sums.back() + (min_dist * min_dist));
            cand_center_ids.push_back(j);
        }

        // Finding the new centroid by generating a real number and 
        // checking to what point's slot that number is present
        uniform_real_distribution<> real_dist(0, partial_sums.back());

        double real_dist_val = real_dist(gen);

        for(int j = 0; j < partial_sums.size() - 1; j++){
            if((partial_sums[j] < real_dist_val) && (real_dist_val <= partial_sums[j+1])){
                centers.push_back(cand_center_ids[j]);
            }
        }

        assert(centers.size() == i + 1);
    }

    return centers;
}

// Classic Lloyd's algorithm
kMeans::clusteringResult kMeans::LloydsAlgorithm() const{
    clock_t start, end;

    start = clock();

    // Initializing the centers
    vector<int>       curr_centers = centerInitialization();
    cout << "Centers Initialized" << endl;
    clusteringResult  result;

    for(int i = 0; i < curr_centers.size(); i++){
        result.clusters.push_back(clusteringResult::cluster(vectors[curr_centers[i]]));
    }

    bool to_stop = false;
    int curr_iterations = 0;

    while((!to_stop) && (curr_iterations < MAX_ITERATIONS)){
        curr_iterations++;

        cout << "Started iteration " << curr_iterations << endl;    // I print that just for keep track of progress
        to_stop = true;

        assert(result.clusters.size() == clusters_number);

        // Assignment of vectors to centers
        for(int i = 0; i < vectors.getRows(); i++){
            double min_dist;
            int    min_center_index = -1;

            for(int j = 0; j < clusters_number; j++){
                double curr_dist = distance_fn(vectors[i], result.clusters[j].centroid);

                if((min_center_index == -1) || (curr_dist < min_dist)){
                    min_dist = curr_dist;
                    min_center_index = j;
                }
            }

            assert(min_center_index != -1);
            result.clusters[min_center_index].img_ids.push_back(i);
        }

        assert(result.clusters.size() == clusters_number);

        // Updating the centers
        for(int i = 0; i < clusters_number; i++){
            vector<double> old_centroid = result.clusters[i].centroid;
            vector<double> new_centroid;

            for(int j = 0; j < old_centroid.size(); j++){
                vector<double> j_th_dims;

                for(int k = 0; k < result.clusters[i].img_ids.size(); k++){
                    j_th_dims.push_back(vectors[result.clusters[i].img_ids[k]][j]);
                }

                if(j_th_dims.size() == 0){
                    new_centroid.push_back(result.clusters[i].centroid[j]);
                }
                else
                    new_centroid.push_back((double) findMedian(j_th_dims));
            }

            cout << "CHANGE " << distance_fn(new_centroid, old_centroid) << endl;
            if(new_centroid != old_centroid){
                result.clusters[i].centroid = new_centroid;
                to_stop = false;
            }
        }

        unsigned long obj_val = 0;

        for(int i = 0; i < clusters_number; i++){
            for(int j = 0; j < result.clusters[i].img_ids.size(); j++)
                obj_val += distance_fn(vectors[result.clusters[i].img_ids[j]], result.clusters[i].centroid);
        }

        cout << "OBJECTIVE " << obj_val << endl;
        
        if((!to_stop) && (curr_iterations < MAX_ITERATIONS)){
            for(int i = 0; i < result.clusters.size(); i++)
                result.clusters[i].img_ids.clear();
        }
    }
    end = clock();

    result.cluster_time = ((double) end - start) / ((double) CLOCKS_PER_SEC);

    result.find_silhouettes(vectors, distance_fn);
    
    return result;
}

kMeans::clusteringResult kMeans::ReverseAssignment() const
{
    clock_t start, end;

    start = clock();

    // Centers initialization
    vector<int>      curr_centers = centerInitialization();
    cout << curr_centers.size() << " Centers Initialized" << endl;
    clusteringResult result;

    for(int i = 0; i < curr_centers.size(); i++){
        result.clusters.push_back(clusteringResult::cluster(vectors[curr_centers[i]]));
    }

    // Items are already indexed at the necessary structure

    bool to_stop = false;
    int curr_iterations = 0;

    while((!to_stop) && (curr_iterations < MAX_ITERATIONS)){
        curr_iterations++;
        cout << "Started Iteration " << curr_iterations <<endl;
        to_stop = true;

        // Finding initial radius, finding min distance between centers
        double   curr_radius;
        set<int> assigned_points;

        for(int i = 0; i < result.clusters.size(); i++){
            for(int j = i + 1; j < result.clusters.size(); j++){
                double curr_dist = distance_fn(result.clusters[i].centroid, 
                                            result.clusters[j].centroid);

                if((i == 0 && j == 1) || (curr_dist < curr_radius)){
                    curr_radius = curr_dist;
                }
            }
        }

        curr_radius = curr_radius / 2.0;
        bool assignment_to_stop = false;

        // Doing Reverse Assignment
        while(!assignment_to_stop){
            assignment_to_stop = true;

            set<int> curr_assigned_points;

            for(int i = 0; i < result.clusters.size(); i++){
                // For each centroid getting its range neighbors
                vector<int> range_neighbors = range_query_structure->rangeSearch(result.clusters[i].centroid, curr_radius);
                
                for(int j = 0; j < range_neighbors.size(); j++){
                    int curr_img_id = range_neighbors[j];

                    // If the neighbor is assigned to previous iteration it is ignored
                    if(assigned_points.find(curr_img_id) != assigned_points.end()){
                        continue;
                    }

                    // If it is assigned to this iteration but on another cluster
                    // we keep the neighbor to its closest cluster(the cluster with
                    // the closest centoid)

                    if(curr_assigned_points.find(curr_img_id) == curr_assigned_points.end()){
                        curr_assigned_points.insert(curr_img_id);

                        result.clusters[i].img_ids.push_back(curr_img_id);
                    }else{
                        assert(i > 0);

                        for(int k = 0; k < i; k++){
                            std::vector<int>::iterator clust_img_ids_it;

                            clust_img_ids_it = find(result.clusters[k].img_ids.begin(), 
                                            result.clusters[k].img_ids.end(), curr_img_id);

                            if(clust_img_ids_it != result.clusters[k].img_ids.end()){
                                assert(curr_img_id == *clust_img_ids_it);

                                double curr_dist = distance_fn(result.clusters[k].centroid, 
                                                            vectors[*clust_img_ids_it]);

                                double new_dist  = distance_fn(result.clusters[i].centroid,
                                                            vectors[curr_img_id]);
                                
                                if(new_dist < curr_dist){
                                    result.clusters[k].img_ids.erase(clust_img_ids_it);
                                    result.clusters[i].img_ids.push_back(curr_img_id);
                                }

                                break;
                            }

                            assert(k != (i-1)); // because somewhere should exist the id
                        }
                    }
                }
            }
            // We stop when we do not have new assigned points to a cluster
            if(curr_assigned_points.size() > 0){
                assignment_to_stop = false;

                curr_radius *= 2;
                assigned_points.insert(curr_assigned_points.begin(), curr_assigned_points.end());
            }
        }

        cout << "Length of R.A. is " << assigned_points.size() << endl;

        // Assignment of unassigned vectors from Reverse Assignment to centers
        for(int i = 0; i < vectors.getRows(); i++){
            if(assigned_points.find(i) != assigned_points.end()){
                /* double min_dist;
                int    min_center_index = -1;

                for(int j = 0; j < clusters_number; j++){
                    double curr_dist = distance_fn(vectors[i], result.clusters[j].centroid);

                    if((min_center_index == -1) || (curr_dist < min_dist)){
                        min_dist = curr_dist;
                        min_center_index = j;
                    }
                }

                bool hypo = find(result.clusters[min_center_index].img_ids.begin(),
                            result.clusters[min_center_index].img_ids.end(), i) != result.clusters[min_center_index].img_ids.end();

                if(!hypo){
                    cout << "ID IS " << i << endl;
                }
                assert(hypo); */
                continue;
            }
            
            double min_dist;
            int    min_center_index = -1;

            for(int j = 0; j < clusters_number; j++){
                double curr_dist = distance_fn(vectors[i], result.clusters[j].centroid);

                if((min_center_index == -1) || (curr_dist < min_dist)){
                    min_dist = curr_dist;
                    min_center_index = j;
                }
            }

            assert(min_center_index != -1);
            result.clusters[min_center_index].img_ids.push_back(i);
        }

        // Updating the centers
        for(int i = 0; i < clusters_number; i++){
            vector<double> old_centroid = result.clusters[i].centroid;
            vector<double> new_centroid;

            for(int j = 0; j < old_centroid.size(); j++){
                vector<double> j_th_dims;

                for(int k = 0; k < result.clusters[i].img_ids.size(); k++){
                    j_th_dims.push_back(vectors[result.clusters[i].img_ids[k]][j]);
                }

                if(j_th_dims.size() == 0)
                    new_centroid.push_back(result.clusters[i].centroid[j]);
                else
                    new_centroid.push_back((double) findMedian(j_th_dims));
            }

            if(distance_fn(new_centroid,old_centroid) >= CONV_MAX_CHANGE){
                cout << "CHANGE " << distance_fn(new_centroid,old_centroid) << endl;
                result.clusters[i].centroid = new_centroid;
                to_stop = false;
            }
        }

        unsigned long obj_val = 0;

        for(int i = 0; i < clusters_number; i++){
            for(int j = 0; j < result.clusters[i].img_ids.size(); j++)
                obj_val += distance_fn(vectors[result.clusters[i].img_ids[j]], result.clusters[i].centroid);
        }

        cout << "OBJECTIVE " << obj_val << endl;
        if(!to_stop && (curr_iterations < MAX_ITERATIONS)){
            for(int i = 0; i < result.clusters.size(); i++)
                result.clusters[i].img_ids.clear();
        }
    }

    end = clock();

    result.cluster_time = ((double) end - start) / ((double) CLOCKS_PER_SEC);


    result.find_silhouettes(vectors, distance_fn);
    
    return result;
}

void kMeans::clusteringResult::find_silhouettes(const Dataset& vectors, 
                            double (*distance_fn)(const vector<double>&, 
                                                const vector<double>&))
{
    double total_silhouettes_sum = 0.0;
    int    total_silhouettes_num = 0;

    for(int i = 0; i < clusters.size(); i++){
        double cluster_silhouettes_sum = 0.0;
        int    cluster_silhouettes_num = 0;

        if(clusters[i].img_ids.size() == 0){
            silhouettes.push_back(0.0);
            continue;
        }

        for(int j = 0; j < clusters[i].img_ids.size(); j++){
            /* Finding average distance with points in same cluster */
            vector<double> curr_img_vector = vectors[ clusters[i].img_ids[j] ];
            double distance_sum = 0.0;
            int    distances    = 0;

            for(int k = 0; k < clusters[i].img_ids.size(); k++){
                if(k == j) continue;

                distance_sum += distance_fn(curr_img_vector, 
                                    vectors[ clusters[i].img_ids[k] ]);
                
                distances++;
            }

            double clust_avg_dist = distance_sum / ((double) distances); // For point p this is a(p)

            /* Finding index of 2nd nearest cluster */
            int    nearest_clust_index = -1;
            double nearest_clust_dist  = 0;

            for(int k = 0; k < clusters.size(); k++){
                if(k == i) continue;

                double cand_dist = distance_fn(curr_img_vector, clusters[k].centroid);

                if((nearest_clust_index == -1) || (cand_dist < nearest_clust_dist)){
                    nearest_clust_dist  = cand_dist;
                    nearest_clust_index = k; 
                }
            }

            /* Finding average distace from points of 2nd nearest cluster */
            distance_sum = 0.0;
            distances    = 0;

            for(int k = 0; k < clusters[nearest_clust_index].img_ids.size(); k++){
                distance_sum += distance_fn(curr_img_vector, 
                                    vectors[ clusters[nearest_clust_index].img_ids[k] ]);
                
                distances++;
            }

            double nearest_clust_avg_dist; // For point p this is b(p)

            if(distances == 0){  // Empty cluster
                nearest_clust_avg_dist = 0.0;
            }else{
                nearest_clust_avg_dist = distance_sum / ((double) distances);
            }

            /* Finding Silhouette */
            double curr_silhouette = 0.0;

            if(clust_avg_dist < nearest_clust_avg_dist){
                curr_silhouette = 1.0 - (clust_avg_dist / nearest_clust_avg_dist);
            } else if(clust_avg_dist > nearest_clust_avg_dist){
                curr_silhouette = (nearest_clust_avg_dist / clust_avg_dist) - 1.0;
            }

            assert((curr_silhouette >= -1.0) && (curr_silhouette <= 1.0));

            cluster_silhouettes_sum += curr_silhouette;
            cluster_silhouettes_num++;
        }

        total_silhouettes_sum += cluster_silhouettes_sum;
        total_silhouettes_num += cluster_silhouettes_num;

        silhouettes.push_back(cluster_silhouettes_sum / ((double) cluster_silhouettes_num));
    }

    silhouettes.push_back(total_silhouettes_sum / ((double) total_silhouettes_num));
}

void kMeans::printResults(const kMeans::clusteringResult& result, 
                        const bool& complete, ofstream& outstream) const
{
    outstream << "Algorithm: ";

    switch(clustering_method){
        case CLASSIC:
            outstream << "Lloyds" << endl;
            break;
        case LSH:
            outstream << "Range Search LSH" << endl;
            break;
        case HYPERCUBE:
            outstream << "Range Search Hypercube" << endl;
            break;
        default:
            assert(0 == 1);
    }

    assert(result.clusters.size() == clusters_number);

    for(int i = 0; i < result.clusters.size(); i++){
        outstream << "CLUSTER-" << i + 1 << " {size:" << result.clusters[i].img_ids.size();
        outstream << ", centroid: [";

        for(int j = 0; j < result.clusters[i].centroid.size(); j++){
            outstream << result.clusters[i].centroid[j] << " ";
        }

        outstream << "]}" << endl;
    }

    outstream << "clustering_time: " << result.cluster_time << endl;
    outstream << "Silhouette: " << "[";

    for(int i = 0; i < result.silhouettes.size() - 1; i++){
        outstream << result.silhouettes[i] << ", ";
    }

    outstream << result.silhouettes.back() << "]" << endl;

    if(complete){
        for(int i = 0; i < result.clusters.size(); i++){
            outstream << "CLUSTER-" << i + 1 << " {[";

            for(int j = 0; j < result.clusters[i].centroid.size(); j++){
                outstream << result.clusters[i].centroid[j] << " ";
            }

            outstream << "], " << endl;

            for(int j = 0; j < result.clusters[i].img_ids.size() - 1; j++){
                outstream << result.clusters[i].img_ids[j] << ", ";
            }

            outstream << result.clusters[i].img_ids.back() << "}" << endl;
        }
    }
}