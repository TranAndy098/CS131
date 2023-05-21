#include <omp.h>
#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <cmath>
#include <sstream>
#include <fstream>
#include <iostream>


// mpic++ -std=c++11 correctness.cpp -o correctness
// ./correctness output1.txt testresults1.txt


 
int mpi_results[256];
int brute_results[256];


int main(int argc, char* argv[])
{   
    
    int pixel_val;
    std::string workString;
    std::ifstream file(argv[1]);
    for( int i = 0; i < 256; i++ )
    {
        if( std::getline(file,workString) && workString.at(0) != '#' ){
            std::stringstream stream(workString);
            if( !stream )
                    break;
            stream >> pixel_val;
            mpi_results[i] = pixel_val;
            
            //printf("%d ", mpi_results[i]);
            }
        
    }
    //printf("\n");
    int pixel_val2;
    std::string workString2;
    std::ifstream file2(argv[2]);
    for( int i = 0; i < 256; i++ )
    {
        if( std::getline(file2,workString2) && workString2.at(0) != '#' ){
            std::stringstream stream(workString2);
            if( !stream )
                    break;
            stream >> pixel_val2;
            brute_results[i] = pixel_val2;
            //printf("%d ", brute_results[i]);
            }
        
    }
    //printf("\n");
    for( int i = 0; i < 256; i++ )
    {
        if( brute_results[i] != mpi_results[i]){
            printf("False for %d \n", i);
        }
        
        
        
    }
    

    return 0;
    
}
