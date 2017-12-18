//
//  main.cpp
//  DominoTiler
//
//  Given a connected, simply connected checkerboard region, cut out of some square checkerboard, this will produce the max and min (wrt the height function on vertices) tilings of the region if it is tileable. Uses Thurston's algorithm.


/*
#include "DominoLib.h"
#include "../Common/util.h"

#define DEBUG 0

int main2(int argc, const char * argv[])
{
    int M = atoi(argv[1]);  // size of the aztec diamond
    int k = atoi(argv[2]);  // number of diagonal slices to remove
    int N = 2*M-k+1; // size for vertice array
    std::vector<int> remove(k); // k distinct points in 0,...,N-1. Order does not matter. Nchoosek possibilities.
    
    if (argc<k+4) {
        std::cout<<"Not enough arguments."<<std::endl;
        return 1;
    }
    for(int i = 3; i<argc-1; ++i) {
        remove[i-3]=atoi(argv[i]);
    }
    std::string filename = argv[argc-1];

    std::vector<int> domain = AztecRectangle(M,k,remove);   // faces of checkerboard domain
    std::vector<int> vertices = verticesFromDomain(domain); // vertices of checkerboard domain

    std::vector<int> maxhf = maxTiling(vertices,domain);   // Max height function on faces of the dimer model (ie vertices of the checkerboard domain).
    std::vector<int> minhf = minTiling(vertices,domain);   // Min height function on faces of the dimer model (ie vertices of the checkerboard domain)..

    std::vector<int> maxtiling = heightfunctionToTileState(maxhf,vertices);   // Domino tiling corresponding to the dimer height function.
    std::vector<int> mintiling = heightfunctionToTileState(minhf,vertices);   // Domino tiling corresponding to the dimer height function.
    
    tilingToBMP(maxtiling,"./output/"+filename+"_max.bmp");
    tilingToBMP(mintiling,"./output/"+filename+"_min.bmp");
    
    std::cout<<"Drew out tilings"<<std::endl;
    
    printMatrix(maxtiling,"./tilestates/"+filename+"_maxts.txt");
    printMatrix(mintiling,"./tilestates/"+filename+"_mints.txt");
    
    std::cout<<"Saved tile states."<<std::endl;
    
    printMatrix(maxhf,"./heightfunctions/"+filename+"_maxhf.txt");
    printMatrix(minhf,"./heightfunctions/"+filename+"_minhf.txt");
    
    std::cout<<"Saved height functions."<<std::endl;
    
    return 0;
}







*/


