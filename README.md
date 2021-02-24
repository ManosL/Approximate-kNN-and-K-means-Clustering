Software Development for Algorithmic Problems
Project 1

Authors

    The project was written by the following:
        
        1) Emmanouil Lykos - 1115201600096
        2) Apostolos-Nikolaos Mponotis - 1115201600111

Compile Instructions:

    In order to compile the project write on the terminal the command "make".
    If you desire to delete the object and executable files write on the
    terminal the command "make clean". In order to run it check the Manual part
    of each section. 

Part 1: Nearest Neighbors & Range Search

Section 1: Locality Sensitive Hashing(LSH)

    1) Manual

        In order to run the LSH program in order to solve k-NN and Range
        queries you should write on terminal the command:

            ./lsh  [–d  <input  file>]  [–q  <query  file>]  [–k  <LSH functions>(def. 4)]
              [-L  <Hash Tables> (def. 5)]  [-ο  <output  file>]  [-Ν <number of nearest> (def. 1)] 
              [-R <radius> (def. 10000.0)]
        
        where:
            -> <input file> is the path to the training set file.

            -> <query file> is the path to the file that contains the vectors
            that we will solve queries upon them.

            -> <LSH functions> is a positive integer that denotes the hash functions 
            that will synthesize the amplified hash function.

            -> <Hash Tables> is a positive integer that denotes the number of the
            hash tables that LSH structure has.
            
            -> <output file> is the path of the output file.

            -> <number of nearest> is a positive integer that denotes how many nearest
            neighbors of query vector our program should find.

            -> <radius>: is a positive float that denotes the radius of the range 
            queries that will be conducted.
        
        All parameters are optional. If the value of the numerical ones is omitted than the
        defaults are used and if the paths are not provided then the user is asked for them in
        the runtime.

    2) Description

        In order to mention how Nearest Neighbors and Range Search problems are solved and generally
        how our program works we should describe firtsly our structure. The structure of the LSH is 
        simple. Our LSH structure contains <Hash Tables> hash tables that each one of them has some
        buckets(represented as linked list) and an Amplified Hash Function. This function has a set
        of hash functions of size <LSH functions> that are the same as the courses slides. 

        Our program firstly initializes the structures and reads the input file and saves them to
        a Dataset structure(which is like a 2D array) and the LSH structure reads the Dataset and
        inserts each vector. Afterwards, it reads the query file and for each query solves the 
        desired queries-with the same way mentioned in the slides- and writes the results into
        the output file.

    3) Hyperparameter tuning   

        The hyperparameters that should be tuned by us and not by the user
        are the window size(W), the exponential factor(m) and the power factor(M). 
        In terms of window size I saw that we have a tradeoff between performance 
        and Distance Precision in the NN query. This happens because as W increases 
        the vectors fall into little buckets, thus we have many buckets empty, thus
        the algorithm does not check less vectors than brute force hence it might give 
        precise neighbors but spend the same time as the brute force solution. After 
        experimentation, we concluded that the best window size is 10000, thus we set 
        window size 1500 because we define also a multiply factor that ranges between 
        2 and 10.

        The power factor M and exponential factor m were set initially to the number of 
        hash table buckets and max(a_i) + 1 respectively, as we did not saw too carefully 
        the slides. When we realized that should be set to 2^floor(32/hash tables) and the m
        will be set to a number that is not power of 2 (we set it to 257) we tested it and
        we saw that LSH performed worse than our initial values of M and m, thus we kept that
        values.

Section 2: Hypercube
    1) Manual: 
        In order to run the Hypercube program in order to solve k-NN and Range
        queries you should write on terminal the command:
    
        ./cube  [–d  <input  file>]  [–q  <query  file>]  [–k  <LSH functions>(def. 4)]
        [-M  <pictures in buckets>] [-probes  <number of neighbors>] 
        [-ο  <output  file>]  [-Ν <number of nearest> (def. 1)]   [-R <radius> (def. 10000.0)]
        
        where:
            -> <input file> is the path to the training set file.

            -> <query file> is the path to the file that contains the vectors
                that we will solve queries upon them.

            -> <h functions> is a positive integer that denotes the hash functions. 
                that will concatenate.

            -> <M> is a positive integer that denotes the number of the
                pictures in buckets that we will search.

            -> <probes> is a positive integer that denotes the number of the
                            neighbors' buckets that we will search.           

            -> <output file> is the path of the output file.

            -> <number of nearest> is a positive integer that denotes how many nearest
                neighbors of query vector our program should find.

            -> <radius>: is a positive float that denotes the radius of the range 
                queries that will be conducted.

        All parameters are optional. If the value of the numerical ones is omitted than the
        defaults are used and if the paths are not provided then the user is asked for them in
        the runtime.

    2) Description: 

    The table is a vector <vector <Node>> where the external vector represents the buckets
    and the inside of the images of each bucket.Idex of the outside vector is decimal
    value of binaryTag.The neighbors function finds the adjacent vertices of the query to
    make the appropriate search based on the probes and M values.
      
    3) Hypercube functions:
    
         • Constructor: Initialized with the following arguments. The size of the table (2d). The distance
                        function and the appropriate sizes for calculating hi as well
                         and M_points (images to be checked in buckets), probes and
                         number k.

                         Saves an array M * k (hash_array), where f_i are stored for each
                         value of h_i .Because h_i is derived from mod M, h_i < M is valid, hence one
                         array M * k is enough. The index of hash_array is h_i (p), where
                         the f_i are stored.
                        
                         The exp_power_factors are stored for the calculation of h. The table
                         size 2d is a vector with Nodes.Node is a struct with pointer to
                         image and id the id of the image we set depending on the order
                         reading pictures.

                        Vector <HashFunction>: saves the h_i for their recalculation.
      
         • Insert: Initializes the uniform distribution for the production of binaryTag.
                        Finds the value h_i (p) and if it exists in the hash_array table, then it enters
                        in concat_result the value of hash_array [hi (p)] [i] otherwise called distribution
                        and the generated value is stored in hash_array, then enters in
                        concat_result.
                      
                        In addition, the index in the table is the decimal value of binrayTag
                        the Node is inserted in the appropriate position after the binaryTag is converted to
                        decimal value of. (binarytoDec)

      
         • get_querytag: Creates the binaryTag based on hash_array. If f (h_i (p)) exists in
                         table then gets its value otherwise calls the distribution.

                         
         • Neighbors: Makes all the neighbors binaryTags and returns a table N with
                        the decimal values ​​of the neighbors binaryTag.

                        
         • DatasetInitialize: Creates Nodes and inserts them into the hypercube taking the
                               their values ​​from the dataset and calling the insert.
     
     
         • Knearestneighbors: Checks on -probes neighboring vertices and puts NNQueryReturn
                              (id, distance, pointer to the vector with which to compare)
                              in a vector. Sort the vector and resize to keep them
                              -N nearest. The counter counts the images checked in each bucket
                              and when M_points are done we go to another neighboring vertex.

                              
         • RangeSearch: Calls the neighbors and -probes peaks checks M images and if
                          within a radius, are entered in the in_range vector (their id is entered).
                          Classify in_range and return it.

                          
         • EvaluateQuerySet: Gets the ifstream of the query_file and the ofstream of the output. Creates
                             the appropriate output and calculates the execution times of
                             hypercube functions.

Part 2: K-Medians Clustering

Section 1: Manual

    In order to run the K-Medians clustering program you should write on terminal the
    following command:

        ./cluster  –i  <input  file>  –c  <configuration  file>  –o  <output file>
                   [-complete]  -m <method: Classic or LSH or Hypercube>
    
    where:

        -> <input file> is the path to the file that contains the vectors that will be
        clustered.

        -> <configuration file> is the path to the configration file that contains the
        number of clusters and other parameters that are needed to LSH and Hypercube(if
        their use is necessary).

        -> <output file> is the path to the output file, where the results will be printed.

        -> -complete is an optional parameter that tells if the user wants to be printed
        full results that show for each cluster the centroid and the points that contains.

        -> <method> is a parameter that denotes the method of clustering. The user can 
        give "Classic" in order to run Lloyd's algorithm, "LSH" to run Reverse Assignment
        algorithm with the use of an LSH Structure and "Hypercube" to run Reverse Assignment
        algorithm with the use of a Hypercube Structure.
    
    The initialization of centers is done with the k-means++ algorithm that it is defined
    in the lecture slides. After the clustering is done we find the silhouettes of each 
    cluster.

Section 2: Lloyd's Algorithm

    Lloyd's algorithms is like K-Medians Classic algorithm and does the following steps:

        1) Initializes the centers

        2) Each point is inducted into the cluster that belongs its closest center.

        3) Every coordinate of the new centroids is updated by taking the medians of its 
        points along that coordinate.

        We run the algorithm for 10 clusters (because the labels of handwritten digits are
        10) and we saw that all the clusters had average Silhouette around 0 and 0.1.

Section 3: Reverse Assignment

    Reverse assignment algorithm is virtually the same as Lloyd's algorithm with only difference 
    that between steps 1 and 2 it happens an approximate assignment to some points by increasing
    the Range Query radius and step 2 is applied only to points that were not assigned in the
    previous steps.

    Note that, that we did not do the change to the lists in the range query structures but we 
    kept a set of assigned points at each loop, in order to keep our code simpler.

    In Reverse Assignment algorithm we got almost the same results as the Lloyd's ones,
    and Reverse Assignment covers a satisfactory number of points.