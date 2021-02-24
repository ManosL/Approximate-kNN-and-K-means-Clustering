#include <iostream>
#include <cmath>
#include <chrono>
#include <random>
#include <string>
#include "hypercube.h"
#include "assert.h"
#include <bits/stdc++.h>
#include "../LSH/HashFunctions/HashFunction/HashFunction.hpp"
#include "../DistanceMetrics/DistanceMetrics.hpp"


Hypercube::Hypercube(const int b, double (*distance_fun)(const std::vector<double>&, const std::vector<double>&),
                const double& w_size, const size_t& s_vals_sz,
                const int& m, const unsigned int& M, const int& probes_size, const int& hash_fns_num, const int& M_p) : window_size(w_size),
                s_values_size(s_vals_sz), power_factor((int) m), modulo_factor(M), probes(probes_size), k(hash_fns_num),
                M_points(M_p),distance_fn(distance_fun) // b = 2^d_hC = n
{
    this->number = b;
    this->probes = probes_size;
    table = new vector <Node> [number];

    hash_array = new int*[modulo_factor];
    for (int i = 0; i < modulo_factor; i++) {
        hash_array[i] = new int[k];
    }

    //initialize hash_array
    for (int i = 0; i < modulo_factor; i++){
        for (int j = 0; j < k; j ++){
            hash_array[i][j] = -1;
        }
    }

    //store modulo
    for(int i = 0; i < s_vals_sz - 1; i++){
        mod_exp_power_factors.push_back(mod_exp(m, s_vals_sz - 1 - i, modulo_factor));
    }

    //store s_value
    for (int i = 0; i < k; i++){
        hash_fun_vec.push_back(HashFunction(w_size, s_vals_sz, m, M));
    }


}

Hypercube::~Hypercube(){
    delete[] table;

    for (int i = 0; i < modulo_factor; i++) {
        delete[] hash_array[i];
    }
    delete[] hash_array;
}

void Hypercube::deleteItem(int index) // index = f
{
    // delete vector in table[index]

    if (table[index].empty() == 0){
        table[index].clear();
        table[index].shrink_to_fit();
    }

}

void Hypercube::insert(Node node) { // creates hash_array too
    string concat_result = "";
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    std::uniform_int_distribution<int> distribution(0,1);

    for(int i = 0; i < k; i++){
        int hash_value = hash_fun_vec[i].get_value(*(node.data), mod_exp_power_factors);
        if (hash_array[hash_value][i] == -1) hash_array[hash_value][i] = distribution(generator);
        concat_result += to_string(hash_array[hash_value][i]); //query binary tag
    }

    table[binaryStringToDec(concat_result)].push_back(node);
}

string Hypercube::get_querytag(vector<double> query_vector){
    string queryTag = "";
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    std::uniform_int_distribution<int> distribution(0,1);

    for (int i = 0; i < k; i++){

        int hash_value = hash_fun_vec[i].get_value(query_vector, mod_exp_power_factors);
        if (hash_array[hash_value][i] == -1) queryTag += to_string(distribution(generator)); // random
        else queryTag += to_string(hash_array[hash_value][i]);
    }

    return queryTag;
}

int* Hypercube::neighbors(string queryTag, int k){
    int* N = new int[k];

    string str; //check *
    for (int i = 0; i<k; i++){
        str = queryTag;
        // query tag has size of k
        str[i] = (str[i] == '1') ? '0' : '1';
        N[i] = binaryStringToDec(str);
    }
    return N;
}

void Hypercube::datasetInitialize(const Dataset& data){
    const int row_num = data.getRows();

    for(int i = 0; i < row_num; i++){
        const vector<double>* data_ptr = &data[i];
        Node node(data_ptr, i, NULL);
        insert(node);
    }
}

// function to display hash table
void Hypercube::displayHypercube() {
    int count = 0;
    for (int i = 0; i < pow(2,k); i++) {
        if (table[i].size() > 0) cout << i <<": ";
        for (auto& x : table[i]) {
            for (int j = 0; j < (x.data)->size(); j++)
              if (j == 0) {cout << "id: " << x.data_id; count++;} // print point(maybe id)
            cout << endl;
        }
        if (table[i].size() > 0) cout << endl;
    }
}

void Hypercube::displayNeighbors(int* N){
    for (int i = 0; i < k; i++){
        if (table[N[i]].empty() == 0){
            cout<<N[i]<<endl;
            cout<<"Neighbors:" << endl; // id
            for (auto& x : table[N[i]] ) {
                for (int j = 0; j < (x.data)->size(); j++)
                   if (j == 0 ) cout << " " << x.data_id; // print point(maybe id)
                cout << endl;
            }
            cout << endl;
        }
    }
}

vector<NNQueryReturn> Hypercube::kNearestNeighbors(const vector<double>& query_vec,const int& nearest_neighbors){
    vector<NNQueryReturn> dist_id;

    int *N;
    N = neighbors(get_querytag(query_vec), k);
    NNQueryReturn dist_node;
    int counter = 0;

    for (int j = 0; j < probes && j < k; j++){
        for (auto& x : table[N[j]]){
            counter++; // not empty probe has been checked
            dist_node.neighbor_vec = x.data;
            dist_node.dist_from_query = distance_fn(query_vec, *(x.data));
            dist_node.neighbor_id = x.data_id;
            dist_id.push_back(dist_node);

            if (counter == M_points /*&& nearest_neighbors == 1*/){ //visit another neighbor,M_points
                counter= 0;
                break;
            }
        }
    }

    delete N;
    sort(dist_id.begin(), dist_id.end());
    if(dist_id.size() > nearest_neighbors)
            dist_id.resize(nearest_neighbors);
    return dist_id;

}

const vector<int> Hypercube::rangeSearch(const vector<double>& query_vec,
                                            const double& radius)
{
    vector<int> in_range;
    int *N;
    N = neighbors(get_querytag(query_vec), k);
    for (int i = 0;  i < probes && i < k; i++){
        int counter = 0;
        for (auto& x : table[N[i]]){
            counter++;
            if (distance_fn(query_vec, *(x.data)) < radius){
                counter++;
                in_range.push_back(x.data_id);
            }

            if (counter == M_points /*&& nearest_neighbors == 1*/){ //visit another neighbor,visit M_points
                counter= 0;
                break;
            }

        }
    }

    //delete N;
    sort(in_range.begin(), in_range.end());
    return in_range;
}


int binaryStringToDec(string binary){
    int value = 0;
    int indexCounter = 0;
    int power = 1;
    for(int i = 0; i < binary.length(); i++){

        if(binary[i]=='1'){
            value += power;
        }
        power*=2;
        indexCounter++;
    }
    return value;
}


void Hypercube::evaluateQuerySet(const Dataset& data, ifstream& querySetFile,
                        ofstream& outFile, const int& k, const double& radius){
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

    double total_hc_time  = 0.0;
    double total_true_time = 0.0;

    double max_error = 0.0;
    double avg_error = 0.0;
    int how_many = 0;

    //buffer = (char*) realloc(buffer,1);
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

        start = clock();
        const vector<NNQueryReturn> approx_k_neighbors = kNearestNeighbors(img_vector, k);
        end   = clock();

        total_hc_time = ((double) end - start) / ((double) CLOCKS_PER_SEC);

        const vector<int>           range_query_res = rangeSearch(img_vector, radius);

        start = clock();
        const vector<double>        true_k_nearest_dists = data.getKNearestDistances(distance_fn, img_vector, k);
        end   = clock();

        total_true_time = ((double) end - start) / ((double) CLOCKS_PER_SEC);
        //cout << "here\n";
        outFile << "Query: " << i+1 << endl;

        for(int i = 0; i < k; i++){
            if(i < approx_k_neighbors.size()){
                assert(approx_k_neighbors[i].dist_from_query >= true_k_nearest_dists[i]);

                outFile << "Nearest neighbor-" << i+1 << ":" << approx_k_neighbors[i].neighbor_id << endl;
                outFile << "distanceHC: " << approx_k_neighbors[i].dist_from_query << endl;
            } else{
                outFile << "Nearest neighbor-" << i+1 << ":" << "-" << endl;
                outFile << "distanceHC: " << "-" << endl;
            }

            outFile << "distanceTrue: " << true_k_nearest_dists[i] << endl;
        }

        outFile << "tHC: "  << total_hc_time  << endl;
        outFile << "tTrue: " << total_true_time << endl;
        outFile << "R-near neighbors:" << endl;

        for(int i = 0; i < range_query_res.size(); i++){
            outFile << range_query_res[i] << endl;
        }

        outFile << endl;

        assert(img_vector.size() == rows * cols);

        /*if(i % 100 == 0)
            cout << i << endl;

        double curr_error = 0.0;

        if(approx_k_neighbors.size() && (true_k_nearest_dists[0] != 0)){
            curr_error = approx_k_neighbors[0].dist_from_query / true_k_nearest_dists[0];
            avg_error += curr_error;
            how_many++;
        }

        if(curr_error > max_error){
            max_error = curr_error;
        }*/
    }
}