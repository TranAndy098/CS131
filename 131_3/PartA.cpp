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
 
int input(int argc, char* argv[]);
int output(int argc, char* argv[]);

/* Global variables, Look at their usage in main() */
int image_height;
int image_width;
int adj_height;
int adj_width;
int image_maxShades;
int inputImage[250000];
int adjMatrix[5000][5000];
int outputImage[256];
int numChunks;
int process_Rank;
int size_Of_Cluster;
int size;

/* **************** Change the function below if you need to ***************** */

int main(int argc, char* argv[])
{   
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size_Of_Cluster);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_Rank);
    
    
    if (process_Rank == 0 && input(argc,argv) == 0) {
        MPI_Finalize();
        return 0;
        }
    
    
    int buf[size];
    MPI_Scatter(inputImage, size, MPI_INT, buf, size, MPI_INT, 0, MPI_COMM_WORLD);
    /************ Call functions to process image *********/
    printf("After Scatter\n");
    int test[256];

    printf("In process %d has buf[0] = %d\n", process_Rank, buf[0]);

    int sub_output[256];

    printf("Gather\n");

    MPI_Gather(&test, 256, MPI_INT, sub_output, 256, MPI_INT, 0, MPI_COMM_WORLD);


    if (process_Rank == 0) {
        printf("After Gather\n");
    }

    if (process_Rank == 0 && output(argc,argv) == 0) {
        MPI_Finalize();
        return 0;
    }

    MPI_Finalize();
    return 0;
    
}


int input(int argc, char* argv[]){
    if(argc != 4)
    {
        std::cout << "ERROR: Incorrect number of arguments. Format is: <Grayscale PGM image> <path to text file with the adjacency matrix> <output file name>" << std::endl;
        return 0;
    }
 
    std::ifstream file(argv[1]);
    if(!file.is_open())
    {
        std::cout << "ERROR: Could not open file " << argv[1] << std::endl;
        return 0;
    }


    /* ******Reading image into 2-D array below******** */

    std::string workString;
    /* Remove comments '#' and check image format */ 
    while(std::getline(file,workString))
    {
        if( workString.at(0) != '#' ){
            if( workString.at(1) != '2' ){
                std::cout << "Input image is not a valid PGM image" << std::endl;
                return 0;
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
                
                inputImage[(i*image_width) + j] = pixel_val;
            }
        } else {
            continue;
        }
    }

    /******Getting Adjacent Matrix******/
    std::ifstream file2(argv[2]);
    if(!file2.is_open())
    {
        std::cout << "ERROR: Could not open file " << argv[2] << std::endl;
        return 0;
    }


    /* ******Reading adjacent matric into 2-D array below******** */

    /* Check image size */ 
    while(std::getline(file2,workString))
    {
        if( workString.at(0) != '#' ){
            std::stringstream stream(workString);
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

    /* Fill input image matrix */ 
    for( int i = 0; i < adj_height; i++ )
    {
        if( std::getline(file2,workString) && workString.at(0) != '#' ){
            std::stringstream stream(workString);
            for( int j = 0; j < adj_width; j++ ){
                if( !stream )
                    break;
                stream >> pixel_val;
                adjMatrix[i][j] = pixel_val;
            }
        } else {
            continue;
        }
    }
    size = (image_height*image_width)/size_Of_Cluster;
    return 1;
}

int output(int argc, char* argv[]){
    /* ********Start writing output to your file************ */
    std::ofstream ofile(argv[3]);
    if( ofile.is_open() )
    {
        for( int i = 0; i < 256; i++ )
        {
            ofile << outputImage[i] << "\n";
        }
    } else {
        std::cout << "ERROR: Could not open output file " << argv[3] << std::endl;
        return 0;
    }
    return 1;
}