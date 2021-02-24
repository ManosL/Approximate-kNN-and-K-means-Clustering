#include <algorithm>
#include <cassert>
#include <ctime>
#include <iostream> // FOR NOW
#include <set>

#include "./LSH_Structure.hpp"
#include "../LinkedList/LinkedList.hpp"
#include "../../MathUtils/MathUtils.hpp"

using namespace std;

// Initializing LSH
LSH_Structure::LSH_Structure(const size_t& hash_tables_num,
                double (*distance_fun)(const std::vector<double>&, const std::vector<double>&),
                const size_t& buckets_num, const size_t& hash_fns_num, 
                const double& w_size,  const size_t& s_vals_sz, const int& m) : 
                    hash_tables_number(hash_tables_num), distance_fn(distance_fun)
{
    hash_tables = new HashTable*[hash_tables_num];

    // Creating the hash table
    for(int i = 0; i < hash_tables_num; i++){
        hash_tables[i] = new HashTable(buckets_num, hash_fns_num, w_size, s_vals_sz, m, buckets_num);
    }

    // Keeping the modular exponentation powers of power factor in order
    // to not make each hash function find them again and again.
    for(int i = 0; i < s_vals_sz - 1; i++){
        mod_exp_power_factors.push_back(mod_exp(m, s_vals_sz - 1 - i, buckets_num));
    }
}

void LSH_Structure::datasetInitialize(const Dataset& data)
{
    const int row_num = data.getRows();

    for(int i = 0; i < row_num; i++){
        const vector<double>* data_ptr = &data[i];
        add_vector(data_ptr, i);
    }

    /* It is used to check if the hashing is good
    for(int i = 0; i < hash_tables_number; i++){
        hash_tables[i]->printBucketSize();
        cout << endl;
    }
    */
}

const vector<NNQueryReturn> LSH_Structure::kNearestNeighbors(const vector<double>& query_vec, const int& k){
    set<int>              vector_ids;
    vector<NNQueryReturn> neighbors;
    int                   checked  = 0;
    bool                  to_break = false;

    for(int i = 0; i < hash_tables_number; i++){
        // Getting the neighbors that our hash function returned
        LinkedList_Iterator cand_neighbors = hash_tables[i]->get_neighbors(query_vec, mod_exp_power_factors);
        
        // If we do not see them again we just add them to a vector as a NNQueryReturn
        while(!cand_neighbors.isNull()){
            int cand_id = cand_neighbors.getID();

            if(vector_ids.find(cand_id) == vector_ids.end()){
                checked++;

                vector_ids.insert(cand_id);

                double distance = distance_fn(query_vec, cand_neighbors.getData());

                neighbors.push_back(NNQueryReturn(cand_neighbors.getData(), distance,
                                                    cand_id));
            }

            // This is an optional optimization.
            /*
            if(checked >= 1000 * hash_tables_number){
                to_break = true;
                break;
            }*/

            cand_neighbors++;
        }

        if(to_break) break;
    }

    sort(neighbors.begin(), neighbors.end());

    if(neighbors.size() > k)
        neighbors.resize(k);

    return neighbors;
}

const vector<int> LSH_Structure::rangeSearch(const vector<double>& query_vec, 
                                            const double& radius)
{
    set<int> r_neighbors_ids;
    set<int> checked_ids;

    int      checked  = 0;
    bool     to_break = false;

    for(int i = 0; i < hash_tables_number; i++){
        // Getting the neighbors of the bucket returned
        LinkedList_Iterator cand_neighbors = hash_tables[i]->get_neighbors(query_vec, mod_exp_power_factors);

        while(!cand_neighbors.isNull()){
            // If we don see that again and has distance < radius we append it
            if(checked_ids.find(cand_neighbors.getID()) == checked_ids.end()){
                checked_ids.insert(cand_neighbors.getID());
                checked++;

                if(distance_fn(query_vec, cand_neighbors.getData()) < radius){
                    r_neighbors_ids.insert(cand_neighbors.getID());
                }
            }

            /*
            if(checked >= 1000 * hash_tables_number){
                to_break = true;
                break;
            }*/

            cand_neighbors++;
        }

        if(to_break) break;
    }

    return vector<int>(r_neighbors_ids.begin(), r_neighbors_ids.end());
}

// Solving the queries from query set file
void LSH_Structure::evaluateQuerySet(const Dataset& data, ifstream& querySetFile, 
                        ofstream& outFile, const int& k, const double& radius)
{
    int length = 4, int_value, magic_number;
    int numberOfImages, rows, cols;
    char* buffer = new char[length];

    int i = 0;
    while (i < 4){
    //first 4 values are 4 bytes each
    querySetFile.read(buffer,4);

    int* buffer2 = (int*) buffer; // read the 4 bytes as int
    int int_value = reverseInt(*buffer2);

    if (i == 0) {magic_number = int_value;}
    else if (i == 1) {numberOfImages = int_value;}
    else if (i == 2) {rows = int_value;}
    else {cols = int_value;}
    i++;
    }

    double total_lsh_time  = 0.0;
    double total_true_time = 0.0;

    double max_error = 0.0;
    double avg_error = 0.0;
    int not_found = 0;
    int how_many  = 0;

    //buffer = (char*) realloc(buffer,1);
    // Reading the query vector
    for(int i=0; !querySetFile.eof() ; ++i)
    {
        vector<double> img_vector;
        assert(img_vector.size() == 0);

        for(int r = 0; r < rows ; ++r)
        {
            for(int c = 0; c < cols; ++c)
            {
                querySetFile.read(buffer,1);
                unsigned char* buffer2 = (unsigned char*) buffer;
                unsigned char pixel = buffer2[0];
                img_vector.push_back((double) pixel);
            }
        }
        
        clock_t start, end;

        // Finding approximante kNN, real kNN and range query
        start = clock();
        const vector<NNQueryReturn> approx_k_neighbors     = kNearestNeighbors(img_vector, k);
        end   = clock();

        total_lsh_time = ((double) end - start) / ((double) CLOCKS_PER_SEC);

        const vector<int>           range_query_res = rangeSearch(img_vector, radius);
        
        start = clock();
        const vector<double>        true_k_nearest_dists = data.getKNearestDistances(distance_fn, img_vector, k);
        end   = clock();

        total_true_time = ((double) end - start) / ((double) CLOCKS_PER_SEC);

        // Doing the necessary writes to output file
        outFile << "Query: " << i+1 << endl;
        
        for(int i = 0; i < k; i++){
            if(i < approx_k_neighbors.size()){
                assert(approx_k_neighbors[i].dist_from_query >= true_k_nearest_dists[i]);

                outFile << "Nearest neighbor-" << i+1 << ":" << approx_k_neighbors[i].neighbor_id << endl;
                outFile << "distanceLSH: " << approx_k_neighbors[i].dist_from_query << endl;
            } else{
                not_found++;
                outFile << "Nearest neighbor-" << i+1 << ":" << "-" << endl;
                outFile << "distanceLSH: " << "-" << endl;
            }

            outFile << "distanceTrue: " << true_k_nearest_dists[i] << endl;
        }
        
        outFile << "tLSH: "  << total_lsh_time  << endl;
        outFile << "tTrue: " << total_true_time << endl;
        outFile << "R-near neighbors:" << endl;
        
        for(int i = 0; i < range_query_res.size(); i++){
            outFile << range_query_res[i] << endl;
        }

        outFile << endl;

        assert(img_vector.size() == rows * cols);

        if(i % 100 == 0)
            cout << i << endl;

        double curr_error = 0.0;

        
        if(approx_k_neighbors.size() && (true_k_nearest_dists[0] != 0)){
            curr_error = approx_k_neighbors[0].dist_from_query / true_k_nearest_dists[0];
            avg_error += curr_error;
            how_many++;
        }

        if(curr_error > max_error){
            max_error = curr_error;
        }
    }

    cout << "Max Error Factor " << max_error << endl;
    cout << "Average Error " << avg_error / ((double) how_many) << endl;
    cout << "Not found nearest neighbors " << not_found << endl;

    delete[] buffer;
}
