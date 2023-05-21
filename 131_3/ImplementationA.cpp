#include <omp.h>
#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <cmath>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <mpi.h>

int fill_arrays(int argc, char* argv[]);
void output_result(int argc, char* argv[]);
 
/* Global variables, Look at their usage in main() */
int image_height;
int image_width;
int image_maxShades;
int adj_height;
int adj_width;
int inputImage[5000*5000];
int adjMatrix[5000*5000];
int output[256];

//mpic++ -std=c++11 ImplementationA.cpp -o ImplementationA
//mpirun -np 4 ./ImplementationA test1.pgm adj1.txt output1.txt

/* ****************Change and add functions below ***************** */
void compute_histogram()
{

}
/* **************** Change the function below if you need to ***************** */

int main(int argc, char* argv[])
{
    int process_Rank, size_Of_Cluster;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size_Of_Cluster);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_Rank);


    int size;
    int adj_size;
    if (process_Rank == 0)
    {
        if (fill_arrays(argc, argv) == -1)
        {
            MPI_Finalize();
            return 0;
        }

        size = (image_height*image_width)/size_Of_Cluster;
        adj_size = adj_width;
        for (int i = 1; i < size_Of_Cluster; ++i) {
            MPI_Send(&size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&adj_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }
    else {
        MPI_Recv(&size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&adj_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    //printf("size=%d\n", size);
    int buf[size];
    int adjBuf[adj_size];
    MPI_Scatter(inputImage, size, MPI_INT, buf, size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(adjMatrix, adj_size, MPI_INT, adjBuf, adj_size, MPI_INT, 0, MPI_COMM_WORLD);

    /*
        Every node has the partial data they need
    */


    int parent = process_Rank;
    int p_parent = process_Rank;
    int counts[256];
    for (int i=0; i < 256; ++i) {
        counts[i] = 0;
    }
    int totalCounts[256];
    for (int i=0; i < 256; ++i) {
        totalCounts[i] = 0;
    }

    if (process_Rank != 1) {
        // wait
        printf("Process %d is waiting\n", process_Rank);
        MPI_Recv(&p_parent, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(counts, 256, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (parent == process_Rank){
            parent = p_parent;
            adjBuf[parent] = 0;
        }
        printf("Process %d is before loop\n", process_Rank);
        // you are initiator
        for (int i = 0; i < adj_size; ++i) {
            if (adjBuf[i] == 1) {
                // go down path
                printf("Process %d is sending to Process %d\n", process_Rank, i);
                MPI_Send(&process_Rank, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Send(counts, 256, MPI_INT, i, 0, MPI_COMM_WORLD);
                printf("Process %d is waiting\n", process_Rank);
                MPI_Recv(&p_parent, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(counts, 256, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                for (int i=0; i < 256; ++i) {
                    totalCounts[i] += counts[i];
                    counts[i] = 0;
                }
                // recv  histogram
            }
        }
        printf("Process %d has parent %d\n", process_Rank, parent);
        for (int i=0; i < size; ++i) {
                totalCounts[buf[i]] += 1;
                }
        MPI_Send(&process_Rank, 1, MPI_INT, parent, 0, MPI_COMM_WORLD);
        MPI_Send(totalCounts, 256, MPI_INT, parent, 0, MPI_COMM_WORLD);

        /*
            calculate and send back to parent
        */
    }
    else {
        printf("Process %d is before loop\n", process_Rank);
        // you are initiator and u r your own parent
        for (int i = 0; i < adj_size; ++i) {
            if (adjBuf[i] == 1) {
                // go down path
                // send
                printf("Process %d is sending to Process %d\n", process_Rank, i);
                MPI_Send(&process_Rank, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Send(counts, 256, MPI_INT, i, 0, MPI_COMM_WORLD);
                printf("Process %d is waiting\n", process_Rank);
                MPI_Recv(&p_parent, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(counts, 256, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                for (int i=0; i < 256; ++i) {
                    totalCounts[i] += counts[i];
                    counts[i] = 0;
                }
                // recv
            }
        }
        printf("Process %d has parent %d\n", process_Rank, parent);
        for (int i=0; i < size; ++i) {
                totalCounts[buf[i]] += 1;
        }
        
        for (int i=0; i < 256; ++i) {
                output[i] = totalCounts[i];
        }
        /*
            Calculate initiator and add to final with others,, no sending back to parent
            store in global output variable
        */
    }
    

    /*
    for(int i = 0; i < adj_size; ++i)
        printf("%d ", adjBuf[i]);
    printf("process %d\n", process_Rank);
    */

    if (process_Rank == 1) { // changed to 1 because the global output of 1 will have the results
        output_result(argc, argv);
    }

    MPI_Finalize();
    return 0;
}


int fill_arrays(int argc, char* argv[])
{
    if(argc != 4)
    {
        std::cout << "ERROR: Incorrect number of arguments. Format is: <Input image filename> <Adjacency Matrix filename> <Output filename>" << std::endl;
        return -1;
    }
 
    std::ifstream file(argv[1]);
    if(!file.is_open())
    {
        std::cout << "ERROR: Could not open file " << argv[1] << std::endl;
        return -1;
    }

    /* ******Reading image into 2-D array below******** */

    std::string workString;
    /* Remove comments '#' and check image format */ 
    while(std::getline(file,workString))
    {
        if( workString.at(0) != '#' ){
            if( workString.at(1) != '2' ){
                std::cout << "Input image is not a valid PGM image" << std::endl;
                return -1;
            } else {
                break;
            }       
        } else {
            continue;
        }
    }
    /* Check image size */ 
    while(std::getline(file,workString))
    {
        if( workString.at(0) != '#' ){
            std::stringstream stream(workString);
            int n;
            stream >> n;
            image_width = n;
            stream >> n;
            image_height = n;
            break;
        } else {
            continue;
        }
    }

    /* Check image max shades */ 
    while(std::getline(file,workString))
    {
        if( workString.at(0) != '#' ){
            std::stringstream stream(workString);
            stream >> image_maxShades;
            break;
        } else {
            continue;
        }
    }
    /* Fill input image matrix */ 
    int pixel_val;
    for( int i = 0; i < image_height; i++ )
    {
        if( std::getline(file,workString) && workString.at(0) != '#' ){
            std::stringstream stream(workString);
            for( int j = 0; j < image_width; j++ ){
                if( !stream )
                    break;
                stream >> pixel_val;
                inputImage[i*image_width + j] = pixel_val;
            }
        } else {
            continue;
        }
    }

    /*********** Get Adjacency Matrix ************/
    std::ifstream adjFile(argv[2]);
    if(!adjFile.is_open())
    {
        std::cout << "ERROR: Could not open adjacency file " << argv[2] << std::endl;
        return -1;
    }

    std::string adj_workString;
    /* Check image size */ 
    while(std::getline(adjFile,adj_workString))
    {
        if( adj_workString.at(0) != '#' ){
            std::stringstream stream(adj_workString);
            int n;
            stream >> n;
            adj_width = n;
            stream >> n;
            adj_height = n;
            break;
        } else {
            continue;
        }
    }
    
    /* Fill adjacency matrix */ 
    int val;
    for( int i = 0; i < adj_height; i++ )
    {
        if( std::getline(adjFile,adj_workString) && adj_workString.at(0) != '#' ){
            std::stringstream stream(adj_workString);
            for( int j = 0; j < adj_width; j++ ){
                if( !stream )
                    break;
                stream >> val;
                adjMatrix[i*adj_width + j] = val;
            }
        } else {
            continue;
        }
    }

    /*
    for (int i = 0; i < adj_height; ++i) {
        for (int j = 0; j < adj_width; ++j)
            std::cout << adjMatrix[i*adj_width + j] << " ";
        std::cout << std::endl;
    }
    */

    return 0;
}

void output_result(int argc, char* argv[])
{
    /* ********Start writing output to your file************ */
    std::ofstream ofile(argv[3]);
    if( ofile.is_open() )
    {
        for( int i = 0; i < 256; i++ )
        {
            ofile << output[i] << "\n";
        }
    } else {
        std::cout << "ERROR: Could not open output file " << argv[3] << std::endl;
    }
}