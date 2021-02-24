#include <cstdbool>
#include <cstdlib>
#include <cstring>

#include <fstream>
#include <istream>
#include <ostream>
#include <iostream>

#include <string>
#include <vector>

#include "../Dataset/Dataset.hpp"
#include "../DistanceMetrics/DistanceMetrics.hpp"
#include "./HashTables/LSH_Structure/LSH_Structure.hpp"

#define WINDOW_SIZE 1500 //7500 works good 10000 works better in terms of error, 15000 has also good times
                         // 20000 the same but does time thus the w will be 1500
#define MULTIPLY_FACTOR 6

using namespace std;

int main(int argc, char* argv[]){
    int    lsh_functions     = 4;   // -k
    bool   lsh_funs_given    = false;

    int    hash_tables       = 5;   // -L
    bool   hash_tables_given = false;

    int    nearest_neighbors = 1;   // -N
    bool   nns_given         = false;

    double search_radius     = 10000.0; // -R
    bool   search_rad_given  = false;

    char*  input_file_c_str  = NULL;
    char*  query_file_c_str  = NULL;
    char*  output_file_c_str = NULL;

    //////////////////////// Checking the arguments ///////////////////////////

    if((argc - 1) % 2 == 1){
        cout << "Usage: ./lsh  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
        cout << "              [-L  <int>]  [-ο  <output  file>]  [-Ν <number of nearest>] [-R <radius>]" << endl;
        return 1;
    }

    // For convenience I will handle argv
    // elements as char* instead of STL string
    for(int i = 1; i < argc; i=i+2){
        if(!strcmp(argv[i], "-d")){
            if(input_file_c_str != NULL){
                cout << "Cannot give same argument twice" << endl;
                cout << "Usage: ./lsh  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
                cout << "              [-L  <int>]  [-ο  <output  file>]  [-Ν <number of nearest>] [-R <radius>]" << endl;
                
                return 1;
            }

            input_file_c_str = argv[i+1];
        } else if(!strcmp(argv[i], "-q")){
            if(query_file_c_str != NULL){
                cout << "Cannot give same argument twice" << endl;
                cout << "Usage: ./lsh  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
                cout << "              [-L  <int>]  [-ο  <output  file>]  [-Ν <number of nearest>] [-R <radius>]" << endl;

                return 1;
            }

            query_file_c_str = argv[i+1];
        } else if(!strcmp(argv[i], "-o")){
            if(output_file_c_str != NULL){
                cout << "Cannot give same argument twice" << endl;
                cout << "Usage: ./lsh  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
                cout << "              [-L  <int>]  [-ο  <output  file>]  [-Ν <number of nearest>] [-R <radius>]" << endl;

                return 1;
            }

            output_file_c_str = argv[i+1];            
        } else if(!strcmp(argv[i], "-k")){
            if(lsh_funs_given){
                cout << "Cannot give same argument twice" << endl;
                cout << "Usage: ./lsh  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
                cout << "              [-L  <int>]  [-ο  <output  file>]  [-Ν <number of nearest>] [-R <radius>]" << endl;

                return 1;               
            }

            lsh_funs_given = true;
            lsh_functions  = atoi(argv[i+1]);
        } else if(!strcmp(argv[i], "-L")){
            if(hash_tables_given){
                cout << "Cannot give same argument twice" << endl;
                cout << "Usage: ./lsh  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
                cout << "              [-L  <int>]  [-ο  <output  file>]  [-Ν <number of nearest>] [-R <radius>]" << endl;

                return 1;                 
            }

            hash_tables_given = true;
            hash_tables       = atoi(argv[i+1]);
        } else if(!strcmp(argv[i], "-N")){
            if(nns_given){
                cout << "Cannot give same argument twice" << endl;
                cout << "Usage: ./lsh  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
                cout << "              [-L  <int>]  [-ο  <output  file>]  [-Ν <number of nearest>] [-R <radius>]" << endl;

                return 1;  
            }

            nns_given         = true;
            nearest_neighbors = atoi(argv[i+1]);
        } else if(!strcmp(argv[i], "-R")){
            if(search_rad_given){
                cout << "Cannot give same argument twice" << endl;
                cout << "Usage: ./lsh  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
                cout << "              [-L  <int>]  [-ο  <output  file>]  [-Ν <number of nearest>] [-R <radius>]" << endl;

                return 1; 
            }

            search_rad_given = true;
            search_radius    = atof(argv[i+1]);
        } else{
                cout << "Usage: ./lsh  [–d  <input  file>]  [–q  <query  file>]  [–k  <int>]" << endl;
                cout << "              [-L  <int>]  [-ο  <output  file>]  [-Ν <number of nearest>] [-R <radius>]" << endl;

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

        // I SHOULD THERE CREATE THE SEARCH STRUCTURE AND I WILL PASS
        // AS ARGUMENT THE ISTREAM OBJECT
        Dataset pictures(input_file_stream);
        double  power_factor = pictures.LSH_findPowerFactor();
        double  window_size  = MULTIPLY_FACTOR * WINDOW_SIZE ;
        int     dimensions   = pictures.getColumns();

        cout << "Power factor " << power_factor << endl;
        cout << "Window Size "  << window_size << endl;
        
        LSH_Structure lsh_struct(hash_tables, ManhattanDistance, pictures.getRows() / 8,
                        lsh_functions, window_size, dimensions, power_factor  );

        lsh_struct.datasetInitialize(pictures);

        input_file_stream.close();

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

        if(output_file_c_str == NULL){
            cout << "Give the path of the output file: " << endl;
            cin  >> output_file_path;
        } else{
            output_file_path = (const char*) output_file_c_str;
        }

        ofstream output_file_stream(output_file_path);

        if(!output_file_stream.is_open()){
            query_file_stream.close();
            cout << "Cannot open output file " << output_file_path << endl;
            return 1;
        }

        lsh_struct.evaluateQuerySet(pictures, query_file_stream, output_file_stream, nearest_neighbors, search_radius);

        output_file_stream.close();
        query_file_stream.close();

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