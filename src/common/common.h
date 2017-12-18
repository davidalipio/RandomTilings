/*
 * util.h
 *
 *  Created on: Dec 15, 2017
 *      Author: Ananth
 */

#ifndef SRC_COMMON_UTIL_H_
#define SRC_COMMON_UTIL_H_

#define __CL_ENABLE_EXCEPTIONS
#ifdef __APPLE__
//#include <OpenCL/opencl.h>
#include <OpenCL/cl.hpp> /* read cpp_wrapper_fix.txt */
#else
#include <CL/cl.hpp>
#endif

#include <string>
#include <vector>

typedef std::vector<int> domain;
typedef std::vector<int> heightfunc;
typedef std::vector<int> tiling;

void PrintOpenCLInfo();
void PrintMatrix(std::vector<int> &mat, std::string filename);
domain LoadDomain(std::string filename);
cl::Program LoadCLProgram(cl::Context context, std::vector<cl::Device> devices, std::string input);

#endif /* SRC_COMMON_UTIL_H_ */
