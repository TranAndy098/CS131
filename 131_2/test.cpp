#include <omp.h>
#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <cmath>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
 
int chunk = 8;

int size = 64;

int outputImage[100][100];


int main(int argc, char* argv[]){
    int i, j;
    #pragma omp parallel private(i) 
{
    #pragma omp for schedule(static, chunk) private(j)
    for(i = 0; i < size; ++i ){
        for(j = 0; j < size;  ++j ){
            #pragma omp critical 
            {
            outputImage[i][j] = omp_get_thread_num();
            }
           
        }
    }
}

    std::ofstream ofile("test.pgm");
    if( ofile.is_open() )
    {
        ofile << "P2" << "\n" << size << " " << size << "\n";
        for( int i = 0; i < size; i++ )
        {
            for( int j = 0; j < size; j++ ){
                ofile << outputImage[i][j] << " ";
            }
            ofile << "\n";
        }
    } else {
        std::cout << "ERROR: Could not open output file " << argv[2] << std::endl;
        return 0;
    }

    return 0;
}