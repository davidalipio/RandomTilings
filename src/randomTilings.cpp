//============================================================================
// Name        : randomTilings.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#define __CL_ENABLE_EXCEPTIONS
#include <cl/cl.hpp>
#include <sstream>
#include <vector>
#include <iostream>
#include "./domino/DominoLib.h"
#include "./domino/DominoTiler.h"
#include "./common/err_code.h"
#include <cmath>

// This example generates a random domino tiling
// of the domain specified in the file ex1/domain.txt.
void dominoExample1() {
	try {
		std::cout<<"Running Example 1."<<std::endl;
		// Setup OpenCL
		cl::Context context(CL_DEVICE_TYPE_DEFAULT);
		std::string sinfo;
		std::vector<cl::Device> devices;
		context.getInfo(CL_CONTEXT_DEVICES, &devices);
		devices[0].getInfo(CL_DEVICE_NAME, &sinfo);
		cl::CommandQueue queue(context);

		cl_int err = 0;

		//Load domain from text and draw it.
		domain d = LoadDomain("./examples/ex1/domain.txt");
		DomainToSVG(d, "./examples/ex1/domain.svg");

		//Compute the maximum tiling and draw it.
		tiling max = MaxTiling(d);
		TilingToSVG(max,"./examples/ex1/MaxTiling.svg");

		//Setup the mcmc.
		DominoTiler D(context, queue, devices, "./src/domino/dominokernel.cl", err);

		//Load the maximum tiling as the initial state.
		d_tiling dtiling = D.PutTiling(max);

		//Do a random walk of 100 steps, with seed 203.
		D.Walk(dtiling, 100, 203);

		//Get the resulting tiling, and draw it.
		tiling randTiling = D.GetTiling(dtiling);
		TilingToSVG(max,"./examples/ex1/RandTiling.svg");

		std::cout<<"Done!"<<std::endl;

	} catch (cl::Error &err) {
		std::cout << "Exception\n";
		std::cerr << "ERROR: " << err.what() << "(" << err_code(err.err()) << ")" << std::endl;
	}

}

int main() {
	dominoExample1();
}
