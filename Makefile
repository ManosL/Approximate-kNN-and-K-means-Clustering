COMPILER     = g++
FLAGS        = -g -g3 -Wpedantic
LSH_OBJS     = AmplifiedHashFunction.o HashFunction.o HashTable.o LinkedList.o LSH_Structure.o Dataset.o DistanceMetrics.o MathUtils.o lsh.o 
CUBE_OBJS	= AmplifiedHashFunction.o HashFunction.o HashTable.o LinkedList.o LSH_Structure.o Dataset.o DistanceMetrics.o MathUtils.o hypercube.o main.o
CLUSTER_OBJS = cluster.o kMeans.o AmplifiedHashFunction.o HashFunction.o HashTable.o LinkedList.o LSH_Structure.o Dataset.o DistanceMetrics.o MathUtils.o hypercube.o

all: cube lsh cluster

cube: $(CUBE_OBJS)
	$(COMPILER) $(FLAGS) $(CUBE_OBJS) -o cube

lsh: $(LSH_OBJS)
	$(COMPILER) $(FLAGS) $(LSH_OBJS) -o lsh

cluster: $(CLUSTER_OBJS)
	$(COMPILER) $(FLAGS) $(CLUSTER_OBJS) -o cluster

kMeans.o: ./Cluster/kMeans/kMeans.cpp
	$(COMPILER) $(FLAGS) -c ./Cluster/kMeans/kMeans.cpp

AmplifiedHashFunction.o: ./LSH/HashFunctions/AmplifiedHashFunction/AmplifiedHashFunction.cpp
	$(COMPILER) $(FLAGS) -c ./LSH/HashFunctions/AmplifiedHashFunction/AmplifiedHashFunction.cpp

HashFunction.o: ./LSH/HashFunctions/HashFunction/HashFunction.cpp
	$(COMPILER) $(FLAGS) -c ./LSH/HashFunctions/HashFunction/HashFunction.cpp

HashTable.o: ./LSH/HashTables/HashTable/HashTable.cpp
	$(COMPILER) $(FLAGS) -c ./LSH/HashTables/HashTable/HashTable.cpp

LinkedList.o: ./LSH/HashTables/LinkedList/LinkedList.cpp
	$(COMPILER) $(FLAGS) -c ./LSH/HashTables/LinkedList/LinkedList.cpp

LSH_Structure.o: ./LSH/HashTables/LSH_Structure/LSH_Structure.cpp
	$(COMPILER) $(FLAGS) -c ./LSH/HashTables/LSH_Structure/LSH_Structure.cpp

Dataset.o: ./Dataset/Dataset.cpp
	$(COMPILER) $(FLAGS) -c ./Dataset/Dataset.cpp

DistanceMetrics.o: ./DistanceMetrics/DistanceMetrics.cpp
	$(COMPILER) $(FLAGS) -c ./DistanceMetrics/DistanceMetrics.cpp

MathUtils.o: ./LSH/MathUtils/MathUtils.cpp
	$(COMPILER) $(FLAGS) -c ./LSH/MathUtils/MathUtils.cpp

lsh.o: ./LSH/lsh.cpp
	$(COMPILER) $(FLAGS) -c ./LSH/lsh.cpp

cluster.o: ./Cluster/cluster.cpp
	$(COMPILER) $(FLAGS) -c ./Cluster/cluster.cpp
	
hypercube.o: ./hyperCube/hypercube.cpp	
	$(COMPILER) $(FLAGS) -c ./hyperCube/hypercube.cpp
	
main.o: ./hyperCube/main.cpp
	$(COMPILER) $(FLAGS) -c ./hyperCube/main.cpp
	
.PHONY: clean

clean:
	rm -rf $(LSH_OBJS) $(CLUSTER_OBJS) $(CUBE_OBJS) cube cluster lsh
