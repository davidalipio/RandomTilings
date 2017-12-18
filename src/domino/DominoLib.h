//
//  DominoLib.h
//  
//
//  Created by David Keating on 11/11/17.
//
//

#ifndef DominoLib_h
#define DominoLib_h

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <queue>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

#include "../common/common.h"

const int neighbors[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};

// N is length TR and BL edge, N-k is length of BL and TR.
// remove gives points to remove along the bottom left edge, must be in [0,N-1]
// Total size of domain is 2*N-k by 2*N-k (note this is the domain of the height function)
domain AztecRectangle(int N, int k, std::vector<int> &remove);

// This will spit out the max tiling (in the partial order given by the height function).
// does not throw error if not all boundary vertices are reached (presumably this will result in a bulk error but it seems like this should throw a boundary error)
tiling MaxTiling(domain &d);
tiling MinTiling(domain &d);

tiling HeightfuncToTiling(heightfunc &hf, std::vector<int> &domain);
heightfunc TilingToHeightfunc(tiling &t, std::vector<int> &domain);

std::vector<int> DomainToVertices(domain &d);

void DomainToSVG(domain &d, std::string filename);

void TilingToSVG(tiling &t, std::string filename);
void TilingToBMP(tiling &t, std::string filename);

#endif /* DominoLib_h */
