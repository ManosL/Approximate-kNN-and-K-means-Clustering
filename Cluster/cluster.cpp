#include <cstdbool>
#include <cstdlib>
#include <cstring>

#include <fstream>
#include <istream>
#include <ostream>
#include <iostream>

#include <string>
#include <vector>

#include "./kMeans/kMeans.hpp"
#include "../DistanceMetrics/DistanceMetrics.hpp"
                         
using namespace std;

int main(int argc, char* argv[]){
    char*  input_file_c_str  = NULL;
    char*  conf_file_c_str   = NULL;
    char*  output_file_c_str = NULL;
    char*  assign_method_str = NULL;
    bool   complete_set      = false;

    //////////////////////// Checking the arguments ///////////////////////////

    if((argc != 10) && (argc != 9)){
        cout << "Usage: ./cluster  [–i  <input  file>]  [–c  <configuration  file>]  [–o  <output file>]" << endl;
        cout << "                  [-complete  <optional>]  [-m <method: Classic or LSH or Hypercube]>" << endl;
        return 1;
    }

    // For convenience I will handle argv
    // elements as char* instead of STL string
    for(int i = 1; i < argc; i=i+2){
        if(!strcmp(argv[i], "-i")){
            if(input_file_c_str != NULL){
                cout << "Cannot give same argument twice" << endl;
                cout << "Usage: ./cluster  [–i  <input  file>]  [–c  <configuration  file>]  [–o  <output file>]" << endl;
                cout << "                  [-complete  <optional>]  [-m <method: Classic or LSH or Hypercube]>" << endl;
        
                return 1;
            }

            input_file_c_str = argv[i+1];
        } else if(!strcmp(argv[i], "-c")){
            if(conf_file_c_str != NULL){
                cout << "Cannot give same argument twice" << endl;
                cout << "Usage: ./cluster  [–i  <input  file>]  [–c  <configuration  file>]  [–o  <output file>]" << endl;
                cout << "                  [-complete  <optional>]  [-m <method: Classic or LSH or Hypercube]>" << endl;

                return 1;
            }

            conf_file_c_str = argv[i+1];
        } else if(!strcmp(argv[i], "-o")){
            if(output_file_c_str != NULL){
                cout << "Cannot give same argument twice" << endl;
                cout << "Usage: ./cluster  [–i  <input  file>]  [–c  <configuration  file>]  [–o  <output file>]" << endl;
                cout << "                  [-complete  <optional>]  [-m <method: Classic or LSH or Hypercube]>" << endl;

                return 1;
            }

            output_file_c_str = argv[i+1];            
        } else if(!strcmp(argv[i], "-complete")){
            if(complete_set){
                cout << "Cannot give same argument twice" << endl;
                cout << "Usage: ./cluster  [–i  <input  file>]  [–c  <configuration  file>]  [–o  <output file>]" << endl;
                cout << "                  [-complete  <optional>]  [-m <method: Classic or LSH or Hypercube]>" << endl;

                return 1;                  
            }

            complete_set = true;
        } else if(!strcmp(argv[i], "-m")){
            if(assign_method_str != NULL){
                cout << "Cannot give same argument twice" << endl;
                cout << "Usage: ./cluster  [–i  <input  file>]  [–c  <configuration  file>]  [–o  <output file>]" << endl;
                cout << "                  [-complete  <optional>]  [-m <method: Classic or LSH or Hypercube]>" << endl;

                return 1;                 
            }

            if(strcmp(argv[i+1], "Classic") && strcmp(argv[i+1], "LSH") && strcmp(argv[i+1], "Hypercube")){
                cout << "Wrong assign method" << endl;
                cout << "Usage: ./cluster  [–i  <input  file>]  [–c  <configuration  file>]  [–o  <output file>]" << endl;
                cout << "                  [-complete  <optional>]  [-m <method: Classic or LSH or Hypercube]>" << endl;

                return 1;                  
            }

            assign_method_str = argv[i+1];
        }else{
                cout << "Usage: ./cluster  [–i  <input  file>]  [–c  <configuration  file>]  [–o  <output file>]" << endl;
                cout << "                  [-complete  <optional>]  [-m <method: Classic or LSH or Hypercube]>" << endl;

            return 1;
        }
    }

    AssignMethod assign_method;

    if(!strcmp(assign_method_str, "Classic")){
        assign_method = CLASSIC;
    } else if(!strcmp(assign_method_str, "LSH")){
        assign_method = LSH;
    } else if(!strcmp(assign_method_str, "Hypercube")){
        assign_method = HYPERCUBE;
    }
    
    string input_file_path;
    string conf_file_path;
    string output_file_path;

    // Checking the files
    input_file_path = (const char*) input_file_c_str;

    ifstream input_file_stream(input_file_path, std::ios::binary);

    if(!input_file_stream.is_open()){
        cout << "Cannot open input file " << input_file_path << endl;
        return 1;
    }

    conf_file_path = (const char*) conf_file_c_str;

    ifstream conf_file_stream(conf_file_path, std::ios::binary);

    if(!conf_file_stream.is_open()){
        cout << "Cannot open configuration file " << conf_file_path << endl;
        return 1;
    }


    output_file_path = (const char*) output_file_c_str;

    ofstream output_file_stream(output_file_path);

    if(!output_file_stream.is_open()){
        cout << "Cannot open output file " << output_file_path << endl;
        return 1;
    }

    /* Reading Configuration file */

    int  clusters_num             = -1;
    bool clusters_num_given       = false;

    int  no_of_vector_hash_tables = 3;
    bool no_of_hash_tables_given  = false;

    int  no_of_vector_hash_fns    = 4;
    bool no_of_hash_fns_given     = false;

    int  max_number_M_hypercube   = 10;
    bool max_no_M_hypercube_given = false;

    int  no_of_hypercube_dims     = 3;
    bool hypercube_dims_given     = false;

    int  no_of_probes             = 2;
    bool no_of_probes_given       = false;

    while(!conf_file_stream.eof()){
        string curr_line;

        getline(conf_file_stream, curr_line);

        if(curr_line.size() == 0) continue;

        string meaning = curr_line.substr(0, curr_line.find(':'));
        int    value   = stoi(curr_line.substr(curr_line.find(':') + 2));

        if(meaning == "number_of_clusters"){
            if(clusters_num_given){
                cout << "Cannot give twice " << meaning << endl;

                return 1;
            }

            if(value <= 0){
                cout << "Cannot give negative value " << endl;

                return 1;
            }

            clusters_num       = value;
            clusters_num_given = true;
        } else if(meaning == "number_of_vector_hash_tables"){
            if(no_of_hash_tables_given){
                cout << "Cannot give twice " << meaning << endl;

                return 1;
            }

            if(value <= 0){
                cout << "Cannot give negative value " << endl;

                return 1;
            }

            no_of_hash_tables_given  = true;
            no_of_vector_hash_tables = value;
        } else if(meaning == "number_of_vector_hash_functions"){
            if(no_of_hash_fns_given){
                cout << "Cannot give twice " << meaning << endl;

                return 1;
            }

            if(value <= 0){
                cout << "Cannot give negative value " << endl;

                return 1;
            }

            no_of_vector_hash_fns = value;
            no_of_hash_fns_given  = true;
        }else if(meaning == "max_number_M_hypercube"){
            if(max_no_M_hypercube_given){
                cout << "Cannot give twice " << meaning << endl;

                return 1;
            }

            if(value <= 0){
                cout << "Cannot give negative value " << endl;

                return 1;
            }

            max_no_M_hypercube_given = true;
            max_number_M_hypercube   = value;
        }else if(meaning == "number_of_hypercube_dimensions"){
            if(hypercube_dims_given){
                cout << "Cannot give twice " << meaning << endl;

                return 1;
            }

            if(value <= 0){
                cout << "Cannot give negative value " << endl;

                return 1;
            }

            no_of_hypercube_dims = value;
            hypercube_dims_given = true;
        }else if(meaning == "number_of_probes"){
            if(no_of_probes_given){
                cout << "Cannot give twice " << meaning << endl;

                return 1;
            }

            if(value <= 0){
                cout << "Cannot give negative value " << endl;

                return 1;
            }

            no_of_probes       = value;
            no_of_probes_given = true;
        }else{
            cout << "Wrong parameter " << meaning << " given" << endl;

            input_file_stream.close();
            output_file_stream.close();
            conf_file_stream.close();

            return 1;
        }
    }

    if(clusters_num <= 0){
        input_file_stream.close();
        output_file_stream.close();
        conf_file_stream.close();

        cout << "Give a valid number of clusters to be created" << endl;
        return 1;
    }

    cout << clusters_num << endl;
    cout << no_of_vector_hash_tables << endl;
    cout << no_of_vector_hash_fns << endl;
    cout << max_number_M_hypercube << endl;
    cout << no_of_hypercube_dims << endl;
    cout << no_of_probes << endl;

    kMeans kmeans_obj(input_file_stream, clusters_num, ManhattanDistance,
                no_of_vector_hash_tables, no_of_vector_hash_fns,
                max_number_M_hypercube, no_of_hypercube_dims,
                no_of_probes, assign_method);
    
    kmeans_obj.solve(complete_set, output_file_stream);

    input_file_stream.close();
    output_file_stream.close();
    conf_file_stream.close();
}