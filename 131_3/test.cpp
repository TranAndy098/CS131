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

// mpic++ -std=c++11 test.cpp -o test
// ./test test1.pgm testresults1.txt


 
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
    input(argc,argv);
    int n;
    for( int i = 0; i < image_height; i++ )
    {
        for( int j = 0; j < image_width; j++ ){
            
                n = inputImage[(i*image_width) + j];
                //printf("%d ", n);
                outputImage[n] +=1;
            }
        } 
    

    output(argc,argv);
    return 0;
    
}


int input(int argc, char* argv[]){
    if(argc != 3)
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

    return 1;
}

int output(int argc, char* argv[]){
    /* ********Start writing output to your file************ */
    std::ofstream ofile(argv[2]);
    if( ofile.is_open() )
    {
        for( int i = 0; i < 256; i++ )
        {
            ofile << outputImage[i] << "\n";
        }
    } else {
        std::cout << "ERROR: Could not open output file " << argv[2] << std::endl;
        return 0;
    }
    return 1;
}