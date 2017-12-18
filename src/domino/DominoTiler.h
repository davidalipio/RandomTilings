/*
 * dominoTiler.h
 *
 *  Created on: Dec 16, 2017
 *      Author: Ananth
 */

#ifndef DOMINO_DOMINOTILER_H_
#define DOMINO_DOMINOTILER_H_

#include "../common/common.h"


struct d_tiling {
	cl::Buffer d_vW;
	cl::Buffer d_vB;
	int N;
};

class DominoTiler {

private:
	cl::Context context;
	cl::CommandQueue queue;
	cl::Program program;
	cl::make_kernel<cl::Buffer, const int> InitTinyMT;
    cl::make_kernel<cl::Buffer, cl::Buffer,const int> RotateTiles;
    cl::make_kernel<cl::Buffer, cl::Buffer, const int, const int> UpdateTiles;
	cl::Buffer tinymtparams;

public:
    DominoTiler(cl::Context context0, cl::CommandQueue queue0, std::vector<cl::Device> devices, std::string source, cl_int &err) :
    	context(context0),
		queue(queue0),
    	program(LoadCLProgram(context,devices,source)),
		InitTinyMT(program, "InitTinyMT"),
		RotateTiles(program, "RotateTiles"),
		UpdateTiles(program, "UpdateTiles") { };

    void LoadTinyMT(std::string params, int size);
    d_tiling PutTiling(tiling &t);
    void GetTiling(d_tiling &dt, tiling &t);
    tiling GetTiling(d_tiling &dt);
    void Walk(d_tiling &dt, int samples, long seed);
};

#endif /* DOMINO_DOMINOTILER_H_ */
