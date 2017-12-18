#include <cmath>
#include "DominoTiler.h"
#include "../TinyMT/file_reader.h"

d_tiling DominoTiler::PutTiling(tiling &t) {
	int N = std::sqrt(t.size());
	std::vector<int> h_vW((N/2)*N,0); // initialize vectors
	std::vector<int> h_vB((N/2)*N,0);

	for(int i=0; i < N ; ++i) {
		for(int j=0; j < N; ++j) {
			if ((i+j)%2==0) {
				h_vB[i*(N/2)+(j/2)] = t[i*N+j];
			} else {
				h_vW[i*(N/2)+(j/2)] = t[i*N+j];
			}
		}
	}

	d_tiling nt;
	nt.d_vW = cl::Buffer(context, h_vW.begin(), h_vW.end(), CL_FALSE);
	nt.d_vB = cl::Buffer(context, h_vB.begin(), h_vB.end(), CL_FALSE);
	nt.N = N;

	return nt;
}


void DominoTiler::LoadTinyMT(std::string params, int size) {
	tinymtparams = get_params_buffer("tinymt32dc.0.1048576.txt", context, queue, size);
}

void DominoTiler::Walk(d_tiling &dt, int numSamples, long seed) {
	srand(seed); // seed rand()

	cl::Buffer d_vB = dt.d_vB; cl::Buffer d_vW = dt.d_vW; int N = dt.N;

	InitTinyMT( cl::EnqueueArgs(queue, cl::NDRange(N*N/2)), tinymtparams, seed );

	for(int i=0; i < numSamples; ++i) {
		if (rand()%2==0) {
			RotateTiles( cl::EnqueueArgs( queue, cl::NDRange(N,N/2)), tinymtparams, d_vB, N);
			UpdateTiles( cl::EnqueueArgs(queue, cl::NDRange(N,N/2)), d_vW, d_vB, N,1);
		} else {
			RotateTiles( cl::EnqueueArgs( queue, cl::NDRange(N,N/2)), tinymtparams, d_vW, N);
			UpdateTiles( cl::EnqueueArgs(queue, cl::NDRange(N,N/2)), d_vB, d_vW, N,0);
		}
	}
}


void DominoTiler::GetTiling(d_tiling &dt, tiling& tilestate) {
	std::vector<int> h_vW((dt.N/2)*dt.N,0); // initialize vectors
	std::vector<int> h_vB((dt.N/2)*dt.N,0);

	cl::copy(queue, dt.d_vW, h_vW.begin(), h_vW.end());
	cl::copy(queue, dt.d_vB, h_vB.begin(), h_vB.end());

	for(int i=0; i<dt.N; ++i) { // combine back into tile state
		for(int j=0; j<dt.N; ++j) {
			if ((i+j)%2==0) {
				tilestate[i*dt.N+j] = h_vB[i*(dt.N/2)+(j/2)];
			} else {
				tilestate[i*dt.N+j] = h_vW[i*(dt.N/2)+(j/2)];
			}
		}
	}
}

tiling DominoTiler::GetTiling(d_tiling &dt) {

	std::vector<int> tilestate(dt.N*dt.N,0);

	std::vector<int> h_vW((dt.N/2)*dt.N,0); // initialize vectors
	std::vector<int> h_vB((dt.N/2)*dt.N,0);

	cl::copy(queue, dt.d_vW, h_vW.begin(), h_vW.end());
	cl::copy(queue, dt.d_vB, h_vB.begin(), h_vB.end());

	for(int i=0; i<dt.N; ++i) { // combine back into tile state
		for(int j=0; j<dt.N; ++j) {
			if ((i+j)%2==0) {
				tilestate[i*dt.N+j] = h_vB[i*(dt.N/2)+(j/2)];
			} else {
				tilestate[i*dt.N+j] = h_vW[i*(dt.N/2)+(j/2)];
			}
		}
	}

	return tilestate;
}
