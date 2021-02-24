#include <cstdbool>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <bitset>

#include <fstream>
#include <istream>
#include <ostream>
#include <iostream>

#include <string>
#include <vector>

#include "../Dataset/Dataset.hpp"
#include "../DistanceMetrics/DistanceMetrics.hpp"
#include "../LSH/HashTables/LSH_Structure/LSH_Structure.hpp"
#include "./hypercube.h"

using namespace std;

#define WINDOW_SIZE 1500 //7500 works good 10000 works better in terms of error, 15000 has also good times
                         // 20000 the same but does time thus the w will be 1500
#define MULTIPLY_FACTOR 4

int main(int argc, char* argv[]){
    int    hash_functions     = 4;   // -k
    bool   hash_funs_given    = false;

    int    M_points          = 10;   // -M
    bool   M_points_given    = false;

    int    probes            = 2;    // -probes
    bool   probes_given      = false;

    int    nearest_neighbors = 1;   // -N
    bool   nns_given         = false;

    double search_radius     = 10000.0; // -R
    bool   search_rad_given  = false;

    char*  input_file_c_str  = NULL;
    char*  query_file_c_str  = NULL;
    char*  output_file_c_str = NULL;


    if((argc - 1) % 2 == 1){
        cout << "Usage: ./cube  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
        cout << "              [-M  <int>]  [-probes <int>]  [-ο  <output  file>]  [-Ν <number of nearest>] [-R <radius>]" << endl;
        return 1;
    }

    for(int i = 1; i < argc; i=i+2){
        if(!strcmp(argv[i], "-d")){
            if(input_file_c_str != NULL){
                cout << "Cannot give same argument twice" << endl;
                cout << "Usage: ./cube  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
                cout << "              [-M  <int>]  [-probes <int>]  [-ο  <output  file>]  [-Ν <number of nearest>] [-R <radius>]" << endl;

                return 1;
            }

            input_file_c_str = argv[i+1];
        } else if(!strcmp(argv[i], "-q")){
            if(query_file_c_str != NULL){
                cout << "Cannot give same argument twice" << endl;
                cout << "Usage: ./cube  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
                cout << "              [-M  <int>]  [-probes <int>]  [-ο  <output  file>]  [-Ν <number of nearest>] [-R <radius>]" << endl;

                return 1;
            }

            query_file_c_str = argv[i+1];
        } else if(!strcmp(argv[i], "-o")){
            if(output_file_c_str != NULL){
                cout << "Cannot give same argument twice" << endl;
                cout << "Usage: ./cube  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
                cout << "              [-M  <int>]  [-probes <int>]  [-ο  <output  file>]  [-Ν <number of nearest>] [-R <radius>]" << endl;

                return 1;
            }

            output_file_c_str = argv[i+1];
        } else if(!strcmp(argv[i], "-k")){
            if(hash_funs_given){
                cout << "Cannot give same argument twice" << endl;
                cout << "Usage: ./cube  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
                cout << "              [-M  <int>]  [-probes <int>]  [-ο  <output  file>]  [-Ν <number of nearest>] [-R <radius>]" << endl;

                return 1;
            }

            hash_funs_given = true;
            hash_functions  = atoi(argv[i+1]);
        } else if(!strcmp(argv[i], "-M")){
            if(M_points_given){
                cout << "Cannot give same argument twice" << endl;
                cout << "Usage: ./cube  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
                cout << "              [-M  <int>]  [-probes <int>]  [-ο  <output  file>]  [-Ν <number of nearest>] [-R <radius>]" << endl;

                return 1;
            }

            M_points_given = true;
            M_points = atoi(argv[i+1]);

        } else if(!strcmp(argv[i], "-probes")){
                    if(probes_given){
                        cout << "Cannot give same argument twice" << endl;
                        cout << "Usage: ./cube  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
                        cout << "              [-M  <int>]  [-probes <int>]  [-ο  <output  file>]  [-Ν <number of nearest>] [-R <radius>]" << endl;

                        return 1;
                    }

            probes_given = true;
            probes       = atoi(argv[i+1]);

        } else if(!strcmp(argv[i], "-N")){
            if(nns_given){
                cout << "Cannot give same argument twice" << endl;
                cout << "Usage: ./cube  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
                cout << "              [-M  <int>]  [-probes <int>]  [-ο  <output  file>]  [-Ν <number of nearest>] [-R <radius>]" << endl;

                return 1;
            }

            nns_given         = true;
            nearest_neighbors = atoi(argv[i+1]);

        } else if(!strcmp(argv[i], "-R")){
            if(search_rad_given){
                cout << "Cannot give same argument twice" << endl;
                cout << "Usage: ./cube  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
                cout << "              [-M  <int>]  [-probes <int>]  [-ο  <output  file>]  [-Ν <number of nearest>] [-R <radius>]" << endl;

                return 1;
            }

            search_rad_given = true;
            search_radius    = atof(argv[i+1]);
        } else{
                cout << "Cannot give same argument twice" << endl;
                cout << "Usage: ./cube  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
                cout << "              [-M  <int>]  [-probes <int>]  [-ο  <output  file>]  [-Ν <number of nearest>] [-R <radius>]" << endl;

            return 1;
        }
    }

    string rerun_response = "y";

    while(!rerun_response.compare("y")){
        string input_file_path;
        string query_file_path;
        string output_file_path;

        // Checking the files
        if(input_file_c_str == NULL){
            cout << "Give the path of the input file: " << endl;
            cin  >> input_file_path;
        }else{
            input_file_path = (const char*) input_file_c_str;
        }

        ifstream input_file_stream(input_file_path, std::ios::binary);

        if(!input_file_stream.is_open()){
            cout << "Cannot open input file " << input_file_path << endl;
            return 1;
        }

        Dataset pictures(input_file_stream);

        if(query_file_c_str == NULL){
            cout << "Give the path of the query file: " << endl;
            cin  >> query_file_path;
        }else{
            query_file_path = (const char*) query_file_c_str;
        }

        ifstream query_file_stream(query_file_path, std::ios::binary);

        if(!query_file_stream.is_open()){
            cout << "Cannot open query file " << query_file_path << endl;
            return 1;
        }

        //Dataset pictures_query(query_file_stream);
        //add id as last element


        double  power_factor = pictures.LSH_findPowerFactor();
        double  window_size  = MULTIPLY_FACTOR * WINDOW_SIZE ; // I tried to search W through NN dists but it did not do good hashing
        //4*pictures.LSH_getWindowSize(ManhattanDistance, n)
        int     dimensions   = pictures.getColumns();

        cout << "Power factor " << power_factor << endl;
        cout << "Window Size "  << window_size << endl;

        Hypercube hypercube(pow(2,hash_functions), ManhattanDistance, window_size, dimensions, pictures.LSH_findPowerFactor(),
                            pictures.getRows() / 8, probes, hash_functions, M_points);

        hypercube.datasetInitialize(pictures);

        //distances
        //hypercube.displayHypercube();
        //cout << pictures_query.getRows() << " " << query_vectors.size() << endl;
        //hypercube.min_distances_HC(query_vectors,M_points);
        //hypercube.kNearestNeighbors(pictures_query[0], nearest_neighbors);

        /*hypercube.rangeSearch(query_vectors[0],search_radius);
        for (int i = 0; i < query_vectors.size(); i++){
            vector<int> in_range = hypercube.rangeSearch(query_vectors[i],search_radius);
            cout << "query: " << i+1 << " in range: " << endl;
            for (int j = 0; j < in_range.size(); j++){
                cout << in_range[j] << " ";
            }
            cout << endl;
        }*/

        if(output_file_c_str == NULL){
            cout << "Give the path of the output file: " << endl;
            cin  >> output_file_path;
        } else{
            output_file_path = (const char*) output_file_c_str;
        }

        ofstream output_file_stream(output_file_path);

        if(!output_file_stream.is_open()){
            cout << "Cannot open output file " << output_file_path << endl;
            return 1;
        }

        hypercube.evaluateQuerySet(pictures, query_file_stream,
                                output_file_stream, nearest_neighbors, search_radius);
        output_file_stream.close();
        query_file_stream.close();

        // THERE I SHOULD PRODUCE RESULTS AND PASS THE STREAM OBJECTS

        input_file_c_str  = NULL;
        query_file_c_str  = NULL;
        output_file_c_str = NULL;

        rerun_response = "";

        while((rerun_response.compare("y")) && (rerun_response.compare("n"))){
            cout << "Do you want to run the program again?(y/n)" << endl;
            cin  >> rerun_response;
        }
    }
}
//./cube -d ./Datasets/t10k-images.idx3-ubyte -q ./Datasets/train-images.idx3-ubyte -k 14 -M 10 -probes 2 -o output.txt -N 1 -R 10000
