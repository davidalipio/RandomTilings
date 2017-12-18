//
//  DominoLib.cpp
//  DominoLib
//
//  Created by David Keating on 11/11/17.
//  Copyright © 2017 David Keating. All rights reserved.
//
#include "DominoLib.h"
#include <climits>
#include "../EasyBMP/EasyBMP.h"

domain AztecRectangle(int N, int k, std::vector<int> &remove) {
	int M =2*N-k;
	std::vector<int> domain(M*M,0); // 2*N-k by 2*N-k domain with 0s everywhere

	for (int i = 0; i < 2*N-k; ++i) {
		for (int j = 0; j < 2*N-k; ++j) {
			if (i == 0 || i == 2*N-k-1) { // top and bottom row
				if (j == (N-k)+(i==2*N-k-1)*k - 1 || j == (N-k)+(i==2*N-k-1)*k) {
					domain[i*M+j] = 1;
				}
			} else {
				if (i<N-k) { // top section
					if (j >= N-k-i-1 && j <= N-k+i) {
						domain[i*M+j] = 1;
					}
				} else if (i > N-1) { // bottom section
					if (j >= i-N+k && j <= 3*N-k-i-1) {
						domain[i*M+j] = 1;
					}
				} else { // middle section
					if (j >= i-(N-k) && j <= i+(N-k)) {
						domain[i*M+j] = 1;
					}
				}
			}
		}
	}
	std::cout<<"Attempting to remove "<<k<<" points"<<std::endl;
	// note that if the wrong number of points to remove is given the program will inform you of this fact but still create the domain. Let's you create untileable domains if desired.
	for (int i = 0; i<k;++i) { // remove boundary points
		if (remove.empty()) {
			std::cout<<"No points removed, expecting "<<k<<std::endl;
		}
		else if (remove.size() != k) {
			std::cout<<remove.size()<<" points removed, expecting "<<k<<std::endl;
		} else {
			int i0 = N-k+remove[i];
			int j0 = remove[i];
			domain[i0*M+j0] = 0;
		}
	}
	std::cout<<"Done."<<std::endl;

	return domain;
}

struct incheightPriority
{
	std::vector<int> hf; int N;

	incheightPriority( std::vector<int>& hf0, int N0) { hf = hf0; N = N0; }

	bool operator()(std::pair<int,int> a, std::pair<int,int> b) const
	{
		return hf[(a.first)*N+a.second] < hf[(b.first)*N+b.second];
	}
};


struct decheightPriority
{
	std::vector<int> hf; int N;

	decheightPriority( std::vector<int>& hf0, int N0) { hf = hf0; N = N0; }

	bool operator()(std::pair<int,int> a, std::pair<int,int> b) const
	{
		return hf[(a.first)*N+a.second] > hf[(b.first)*N+b.second];
	}
};


std::vector<int> MaxTiling(domain &d) {
	std::vector<int> vertices = DomainToVertices(d);

	int N = sqrt(vertices.size());
	std::vector<int> hf(N*N, INT_MAX); // creates a vector of ints called hf of size N*N with all elements having value INT_MAX

	/* We color even squares white, odd squares black. (Even as in i+j is even.)
	 * Black squares have black vertices NW and SE.
	 * White squares have black vertices NE and SW.
	 *
	 * The height function rules are:
	 * when crossing a black-on-left edge, +1
	 * when crossing a white-on-left edge, -1
	 *
	 * The rules above become:
	 * 		when leaving a black square towards north or south, +1, otherwise -1
	 * 		when leaving a white square towards north or south, -1, otherwise +1
	 */

	// We assume the domain is connected and simply connected.


	// First we find a starting vertex on the boundary of the domain
	std::queue<std::pair<int, int> > bfs;

	bool found = false;

	for (int i = 0; i < N && !found; i++) {
		for (int j = 0; j < N && !found; j++) {   // Search through each point in the domain.
			if ( vertices[i*N+j] == 2 ) {   // Check if it is a boundary point.
				found = true;   // If we found a boundary point stop searching,
				bfs.push(std::pair<int, int>(i,j));   // add it to the queue,
				hf[i*N+j] = 0;   // and set its height to 0.
			}
		}
	}

	// Then traverse the boundary, computing the height function along the boundary.
	// At the same time, add boundary points to the BFS queue.


	std::priority_queue< std::pair<int,int>, std::vector<std::pair<int, int> >, decheightPriority > bfs_pq( decheightPriority(hf,N) );   // Defines bfs_pq. This contains the boundary points ordered from least height to greatest. So we'll pop the lowest height first.

	// boundary stuff here
	while ( !bfs.empty() )
	{
		int i0 = bfs.front().first; int j0 = bfs.front().second;
		bfs.pop();

		// if (i0+j0)%2 == 0 then +1 for i change
		// if (i0+j0)%2 == 1 then +1 for j change
		for (int nb = 0; nb < 4; nb++) {
			int i = i0+neighbors[nb][0]; int j = j0+neighbors[nb][1];

			if ( i >= 0 && j >= 0 && i < N && j < N  && vertices[i*N+j] == 2) {
				if (((i0==0 || i0==N-1) && nb>1) || ((j0==0 || j0==N-1) && nb<=1) ) { // if we are on the boundary of our domain then we must be traversing boundary edges
					if ( hf[i*N+j] == INT_MAX ) {
						std::pair<int,int> np = std::pair<int, int>(i,j);
						hf[i*N+j] = hf[i0*N+j0] + (nb > 1 ? 1 : -1) * ((i0+j0)%2 == 0 ? 1 : -1);
						bfs.push(np);
						bfs_pq.push(np);
					} else {
						if (hf[i*N+j] != hf[i0*N+j0] + (nb > 1 ? 1 : -1) * ((i0+j0)%2 == 0 ? 1 : -1)) {
							std::cout<<"!!This region is NOT tileable!! Max, Boundary"<<std::endl;
							return hf;
						}
					}
				} else if (!(d[(i0 - (nb==1) - (nb>1))*(N-1)+j0 - (nb<=1)-(nb==3)] && d[(i0 - (nb==1))*(N-1)+j0-(nb==3)])) { // check to make sure we are traversing boundary edges
					if ( hf[i*N+j] == INT_MAX ) {
						std::pair<int,int> np = std::pair<int, int>(i,j);
						hf[i*N+j] = hf[i0*N+j0] + (nb > 1 ? 1 : -1) * ((i0+j0)%2 == 0 ? 1 : -1);
						bfs.push(np);
						bfs_pq.push(np);
					} else {
						if (hf[i*N+j] != hf[i0*N+j0] + (nb > 1 ? 1 : -1) * ((i0+j0)%2 == 0 ? 1 : -1)) {
							std::cout<<"!!This region is NOT tileable!! Max, Boundary"<<std::endl;
							return hf;
						}
					}
				}
			}
		}
	}

	// now fill in the height function in the bulk. Start at lowest know vertex and only move upward
	while (!bfs_pq.empty()) {
		int i0 = bfs_pq.top().first; int j0 = bfs_pq.top().second; bfs_pq.pop();

		for (int nb = 0; nb < 4; nb++) {
			int i = i0+neighbors[nb][0]; int j = j0+neighbors[nb][1];

			if ( i >= 0 && j >= 0 && i < N && j < N  && vertices[i*N+j] > 0  && ((i0+j0)%2 == 0 ? 1 : -1)*(nb > 1 ? 1 : -1) > 0) {   // Only move in directions that increase height.

				if ( vertices[i*N+j] == 2 && hf[i*N+j] > hf[i0*N+j0] + 1  && vertices[i0*N+j0] != 2) {   // Check height compatibility. Will the catch all cases?
					std::cout<<"!!This region is NOT tileable!! Max, Bulk"<<std::endl;
					for (int i=0;i<N;++i){
						for (int j=0;j<N;++j){
							std::cout<<hf[i*N+j]<<" ";
						}
						std::cout<<std::endl;
					}
					std::cout<<std::endl;
					return hf;
				} else if ( hf[i*N+j] == INT_MAX ) {
					hf[i*N+j] = hf[i0*N+j0] + 1;
					bfs_pq.push(std::pair<int, int>(i,j));
				}
			}
		}
	}


	for (int i = 0; i < N; i++) {   // Set height of all vertices outside domain to 0.
		for (int j = 0; j < N; j++) {
			if (vertices[i*N+j] == 0 )
				hf[i*N+j] = 0;
		}
	}


	return HeightfuncToTiling(hf,vertices);
}

tiling MinTiling(domain &d) {
	std::vector<int> vertices = DomainToVertices(d);

	int N = sqrt(vertices.size());
	std::vector<int> hf(N*N,INT_MAX); // creates a vector of ints called hf of size N*N with all elements having value INT_MAX

	/* We color even squares black, odd squares white. (Even as in i+j is even.)
	 * Black squares have black vertices NW and SE.
	 * White squares have black vertices NE and SW.
	 *
	 * The height function rules are:
	 * when crossing a black-on-left edge, +1
	 * when crossing a white-on-left edge, -1
	 *
	 * The rules above become:
	 * 		when leaving a black square towards north or south, +1, otherwise -1
	 * 		when leaving a white square towards north or south, -1, otherwise +1
	 */

	// We assume the domain is connected and simply connected.


	// First we find a starting vertex on the boundary of the domain
	std::queue<std::pair<int, int> > bfs;

	bool found = false;

	for (int i = 0; i < N && !found; i++) {
		for (int j = 0; j < N && !found; j++) {   // Search through each point in the domain.
			if ( vertices[i*N+j] == 2 ) {   // Check if it is a boundary point.
				found = true;   // If we found a boundary point stop searching,
				bfs.push(std::pair<int, int>(i,j));   // add it to the queue,
				hf[i*N+j] = 0;   // and set its height to 0.
			}
		}
	}


	std::priority_queue< std::pair<int,int>, std::vector<std::pair<int, int> >, incheightPriority > bfs_pq( incheightPriority(hf,N) );   // Defines bfs_pq. This contains the boundary points ordered from greatest height to least (?). So we'll pop the greatest height first.


	// boundary stuff here
	while ( !bfs.empty() )
	{
		int i0 = bfs.front().first; int j0 = bfs.front().second;
		bfs.pop();

		// if (i0+j0)%2 == 0 then +1 for i change
		// if (i0+j0)%2 == 1 then +1 for j change
		for (int nb = 0; nb < 4; nb++) {
			int i = i0+neighbors[nb][0]; int j = j0+neighbors[nb][1];

			if ( i >= 0 && j >= 0 && i < N && j < N  && vertices[i*N+j] == 2) {
				if (((i0==0 || i0==N-1) && nb>1) || ((j0==0 || j0==N-1) && nb<=1)) { // if we are on the boundary of our domain then we must be traversing boundary edges
					if ( hf[i*N+j] == INT_MAX ) {
						std::pair<int,int> np = std::pair<int, int>(i,j);
						hf[i*N+j] = hf[i0*N+j0] + (nb > 1 ? 1 : -1) * ((i0+j0)%2 == 0 ? 1 : -1);
						bfs.push(np);
						bfs_pq.push(np);
					} else {
						if (hf[i*N+j] != hf[i0*N+j0] + (nb > 1 ? 1 : -1) * ((i0+j0)%2 == 0 ? 1 : -1)) {
							std::cout<<"!!This region is NOT tileable!! Min, Boundary"<<std::endl;
							return hf;
						}
					}
				} else if (!(d[(i0 - (nb==1) - (nb>1))*(N-1)+j0 - (nb<=1)-(nb==3)] && d[(i0 - (nb==1))*(N-1)+j0-(nb==3)])) { // check that we are traversing a boundary edge
					if ( hf[i*N+j] == INT_MAX ) {
						std::pair<int,int> np = std::pair<int, int>(i,j);
						hf[i*N+j] = hf[i0*N+j0] + (nb > 1 ? 1 : -1) * ((i0+j0)%2 == 0 ? 1 : -1);
						bfs.push(np);
						bfs_pq.push(np);
					} else {
						if (hf[i*N+j] != hf[i0*N+j0] + (nb > 1 ? 1 : -1) * ((i0+j0)%2 == 0 ? 1 : -1)) {
							std::cout<<"!!This region is NOT tileable!! Min, Boundary"<<std::endl;
							return hf;
						}
					}
				}
			}
		}
	}


	// now fill in the height function in the bulk. Start at heighest vertex and only move down.
	while (!bfs_pq.empty()) {
		int i0 = bfs_pq.top().first; int j0 = bfs_pq.top().second; bfs_pq.pop();

		for (int nb = 0; nb < 4; nb++) {
			int i = i0+neighbors[nb][0]; int j = j0+neighbors[nb][1];

			if ( i >= 0 && j >= 0 && i < N && j < N  && vertices[i*N+j] > 0  && ((i0+j0)%2 == 0 ? 1 : -1)*(nb > 1 ? 1 : -1) < 0) {   // Only move in directions that decrease height.

				if ( vertices[i*N+j] == 2 && hf[i*N+j] < hf[i0*N+j0] - 1 && hf[i0*N+j0]!=2) {   // Check height compatibility. Will the catch all cases?
					std::cout<<"!!This region is NOT tileable!! Min, Bulk"<<std::endl;  // what about checking with already visted site not on boundary?
					return hf;
				} else if ( hf[i*N+j] == INT_MAX ) {
					hf[i*N+j] = hf[i0*N+j0] - 1;
					bfs_pq.push(std::pair<int, int>(i,j));
				}
			}
		}
	}

	for (int i = 0; i < N; i++) {   // Set height of all vertices outside domain to 0.
		for (int j = 0; j < N; j++) {
			if ( vertices[i*N+j] == 0 )
				hf[i*N+j] = 0;
		}
	}

	return HeightfuncToTiling(hf,vertices);
}

tiling HeightfuncToTiling(heightfunc &hf, std::vector<int> &verts) {
	int N = sqrt(hf.size());

	std::vector<int> tiling(N*N,0);
	int eU=0,eD=0,eL=0,eR=0,state=0;
	for(int i=0; i<N; ++i) {
		for(int j=0; j<N; ++j) {
			if (i>0) {if (verts[(i-1)*N+j]!=0) {eU = std::abs((hf[i*N+j]-hf[(i-1)*N+j])/2);} }
			if (i<N-1) {if (verts[(i+1)*N+j]!=0) {eD = std::abs((hf[i*N+j]-hf[(i+1)*N+j])/2);} }
			if (j>0) {if (verts[i*N+j-1]!=0) {eL = std::abs((hf[i*N+j]-hf[i*N+j-1])/2);} }
			if (j<N-1) {if (verts[i*N+j+1]!=0) {eR = std::abs((hf[i*N+j]-hf[i*N+j+1])/2);} }
			state = (verts[i*N+j]!=0)*(eU + 2*eD + 4*eL + 8*eR);

			tiling[i*N+j] = state;
			eU=0,eD=0,eL=0,eR=0;
		}
	}
	return tiling;
}

heightfunc TilingToHeightFunc(tiling &t, domain& d) {
	return std::vector<int>(0,0);
}

std::vector<int> DomainToVertices(domain &d) {
	int M = sqrt(d.size());
	int N = M+1;
	std::vector<int> vertices(N*N,0);
	for (int i=0; i<N; ++i) {
		for (int j=0; j<N; ++j) {
			if (i == 0 || j == 0 || i == M || j == M) { // deal with the edge of the domain
				if (d[(i - (i==M))*M + j - (j==M)] || d[(i-1+(i==0))*M + j-(j==M)] || d[(i-(i==M))*M + j-1+(j==0)] || d[((i-1+(i==0))*M + j-1+(j==0))]) {
					vertices[i*N+j] = 2;
				}
			} else {
				if (d[(i-1)*M + j-1] && d[(i-1)*M + j] && d[(i)*M + j-1] && d[(i)*M + j]) { // check if interior vertex
					vertices[i*N+j] = 1;
				} else if (d[(i-1)*M + j-1] || d[(i-1)*M + j] || d[(i)*M + j-1] || d[(i)*M + j]) { // if not check if boundary vertex
					vertices[i*N+j] = 2;
				}
			}
		}
	}

	return vertices;
}


void DomainToSVG(domain &d, std::string filename) {
	std::ofstream outputFile(filename.c_str());

	int N = sqrt(d.size());

	double s = 20.0;

	std::string BlackStyle = "\" style=\"fill:darkgrey;stroke:black;stroke-width:1\" />\n";
	std::string WhiteStyle = "\" style=\"fill:dodgerblue;stroke:black;stroke-width:1\" />\n";

	std::string OutStyle = "\" style=\"fill:aliceblue;stroke:aquamarine;stroke-width:1\" />\n";

	outputFile<< "<svg xmlns='http://www.w3.org/2000/svg' height=\"" << (s)*(N) << "\" width=\""<<s*(N)<<"\">\n";

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {

			if ( d[i*N+j] == 1) {
			double x = s * j;
			double y = s*(N-i-1);

				outputFile<< "<polygon points=\"";
				outputFile<<x<<","<<y<<" "
						<<x+s<<","<<y<<" "
						<<x+s<<","<<y+s<<" "
						<<x<<","<<y+s;

						if ( (i+j)% 2 == 0)
							outputFile<<BlackStyle;
						else
							outputFile<<WhiteStyle;
						}
		}
	}
	outputFile<< "</svg>";
	outputFile.close();
}


void TilingToSVG(tiling &t, std::string filename) {
	std::ofstream outputFile(filename.c_str());

	int N = sqrt(t.size());

	double s = 20.0;

	std::string style = "\" style=\"fill:deepskyblue;stroke:black;stroke-width:1\" />\n";

	outputFile<< "<svg xmlns='http://www.w3.org/2000/svg' height=\"" << s*N << "\" width=\""<<s*N<<"\">\n";

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			double x = s * j;
			double y = s*(N-i);
			if ( (t[i*N+j] ) & 1 == 1) {

				outputFile<< "<polygon points=\"";
				outputFile<<x-s/2<<","<<y+s/2<<" "
						<<x+3*s/2<<","<<y+s/2<<" "
						<<x+3*s/2<<","<<y-s/2<<" "
						<<x-s/2<<","<<y-s/2;
				outputFile<<style;
			}


			if ( (t[i*N+j]  & 4) == 4) {
				outputFile<< "<polygon points=\"";
				outputFile<<x-s/2<<","<<y+s/2<<" "
						<<x+s/2<<","<<y+s/2<<" "
						<<x+s/2<<","<<y-3*s/2<<" "
						<<x-s/2<<","<<y-3*s/2;
				outputFile<<style;
			}
		}
	}
	outputFile<< "</svg>";
	outputFile.close();
}

void TilingToBMP(tiling &t, std::string filename) {
	int N = sqrt(t.size());

	BMP tile[8];
	tile[0].ReadFromFile("./IMGS/tile0d.bmp");
	tile[1].ReadFromFile("./IMGS/tile1d.bmp");
	tile[2].ReadFromFile("./IMGS/tile2d.bmp");
	tile[3].ReadFromFile("./IMGS/tile3d.bmp");
	tile[4].ReadFromFile("./IMGS/tile4d.bmp");
	tile[5].ReadFromFile("./IMGS/tile5d.bmp");
	tile[6].ReadFromFile("./IMGS/tile6d.bmp");
	tile[7].ReadFromFile("./IMGS/tile7d.bmp");
	int S = tile[0].TellWidth();

	BMP output;

	output.SetSize(N*(S/2-1) + S/2, N*(S/2-1) + S/2);
	output.SetBitDepth(32);

	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			switch(t[i*N+j]) {
			case 0:
				RangedPixelToPixelCopyTransparent( tile[0], 0, S, S, 0, output, j*(S/2-1), i*(S/2-1), *tile[0](1,0) );
				break;
			case 1:
				RangedPixelToPixelCopyTransparent( tile[2], 0, S, S, 0, output, j*(S/2-1), i*(S/2-1), *tile[0](1,0) );
				break;
			case 2:
				RangedPixelToPixelCopyTransparent( tile[4], 0, S, S, 0, output, j*(S/2-1), i*(S/2-1), *tile[0](1,0) );
				break;
			case 3:
				RangedPixelToPixelCopyTransparent( tile[5], 0, S, S, 0, output, j*(S/2-1), i*(S/2-1), *tile[0](1,0) );
				break;
			case 4:
				RangedPixelToPixelCopyTransparent( tile[1], 0, S, S, 0, output, j*(S/2-1), i*(S/2-1), *tile[0](1,0) );
				break;
			case 8:
				RangedPixelToPixelCopyTransparent( tile[3], 0, S, S, 0, output, j*(S/2-1), i*(S/2-1), *tile[0](1,0) );
				break;
			case 12:
				RangedPixelToPixelCopyTransparent( tile[6], 0, S, S, 0, output, j*(S/2-1), i*(S/2-1), *tile[0](1,0) );
				break;
			}
		}
	}

	output.WriteToFile(filename.c_str());
}
