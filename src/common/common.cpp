#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include "common.h"

void PrintOpenCLInfo()
{
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);

	int platform_id = 0;
	int device_id = 0;

	std::cout << "Number of Platforms: " << platforms.size() << std::endl;

	for( std::vector<cl::Platform>::iterator it = platforms.begin(); it != platforms.end(); ++it) {
		cl::Platform platform(*it);

		std::cout << "Platform ID: " << platform_id++ << std::endl;
		std::cout << "Platform Name: " << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
		std::cout << "Platform Vendor: " << platform.getInfo<CL_PLATFORM_VENDOR>() << std::endl;

		std::vector<cl::Device> devices;
		platform.getDevices(CL_DEVICE_TYPE_GPU | CL_DEVICE_TYPE_CPU, &devices);

		for(std::vector<cl::Device>::iterator it2 = devices.begin(); it2 != devices.end(); ++it2){
			cl::Device device(*it2);

			std::cout << "\tDevice " << device_id++ << ": " << std::endl;
			std::cout << "\t\tDevice Name: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
			std::cout << "\t\tDevice Type: " << device.getInfo<CL_DEVICE_TYPE>();
			std::cout << " (GPU: " << CL_DEVICE_TYPE_GPU << ", CPU: " << CL_DEVICE_TYPE_CPU << ")" << std::endl;
			std::cout << "\t\tDevice Vendor: " << device.getInfo<CL_DEVICE_VENDOR>() << std::endl;
			std::cout << "\t\tDevice Max Compute Units: " << device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << std::endl;
			std::cout << "\t\tDevice Global Memory: " << device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>() << std::endl;
			std::cout << "\t\tDevice Max Clock Frequency: " << device.getInfo<CL_DEVICE_MAX_CLOCK_FREQUENCY>() << std::endl;
			std::cout << "\t\tDevice Max Allocateable Memory: " << device.getInfo<CL_DEVICE_MAX_MEM_ALLOC_SIZE>() << std::endl;
			std::cout << "\t\tDevice Local Memory: " << device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>() << std::endl;
			std::cout << "\t\tDevice Available: " << device.getInfo< CL_DEVICE_AVAILABLE>() << std::endl;
		}
		std::cout<< std::endl;
	}
}


void printMatrix(std::vector<int> &tiling, std::string filename) {
	int N = std::sqrt(tiling.size());
	std::ofstream outputFile(filename.c_str());


	// save the domain to text file
	for (int i=0; i<N; ++i){
		for(int j=0; j<N; ++j) {
			outputFile<<tiling[i*N+j]<<" ";
		}
	}
	outputFile.close();
}


domain LoadDomain(std::string filename) {
	std::vector<int> mat;
	std::ifstream is(filename.c_str());     // open file
	if(!is.is_open())
	{
		std::cout<<"File did not open."<<std::endl;
		exit(1);
	}

	char c;
	while (is.get(c)) {
		if ( c == '0')
			mat.push_back(0);
		if ( c == '1')
			mat.push_back(1);
	}
	is.close();                // close file

	return mat;
}


cl::Program LoadCLProgram(cl::Context context, std::vector<cl::Device> devices,std::string input) {

	std::ifstream sourceFile(input.c_str());

	if (!sourceFile.is_open()) {
		std::cout << "Cannot open file: " << input << std::endl;
		exit(1);
	}

	std::string sourceCode( std::istreambuf_iterator<char>(sourceFile), (std::istreambuf_iterator<char>()));
	cl::Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length()));

	cl_int err;

	cl::Program program = cl::Program(context, source, &err);

	try {
		err = program.build(devices);
	} catch (cl::Error &e) {
		if (e.err() != CL_SUCCESS) {
			if (e.err() == CL_BUILD_PROGRAM_FAILURE) {
				std::string str = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]);
				std::cout << "compile error:" << std::endl;
				std::cout << str << std::endl;
			} else {
				std::cout << "build error but not program failure err:" << e.err() << " " << e.what() << std::endl;
			}
		}
		throw e;
	}

	return program;
}


