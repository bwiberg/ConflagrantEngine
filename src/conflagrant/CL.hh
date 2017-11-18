/**
 * Created by Benjamin Wiberg on 14/09/2017.
 */

#pragma once

#include <cl/cl.hpp>
#include <string>
#include <conflagrant/logging.hh>

namespace cfl {
/**
 * @func _OpenCLErrorToString
 * @brief Converts an OpenCL error to an std::string.
 * @param err The OpenCL error returned by an operation.
 * @returns A string that explain the OpenCL error.
 *
 * This function is called by _DisplayOpenCLError to get the error code as a string.
 */
std::string _OpenCLErrorToString(cl_int error);

/**
 * @func _DisplayOpenCLError
 * @brief Prints a user friendly message for the most recent OpenCL error (if any) to stdout.
 * @param command The command that was run.
 * @param file The file from which the command was run.
 * @param line The line in the file where the command was fun.
 *
 * This function is used by the OCL macro to provide debugging information in stdout when an OpenCL API call fails.
 */

inline void _DisplayOpenCLError(cl_int error, std::string const& command, std::string const& file, int line) {
    if (error != CL_SUCCESS) {
        LOG_ERROR(OpenCL) << error << " -- " << _OpenCLErrorToString(error) << std::endl
                          << "Relevant Command: " << command << std::endl
                          << "Location: " << file << " at Line: " << line << std::endl;
    }
}


/**
 * @def OCL
 * @brief Provides an easy to use macro that wraps around OpenCL calls and checks their error.
 * @param x The OpenCL command.
 */
#if CFL_DEBUG == 1
#define OCL(x) _DisplayOpenCLError(x, #x, __FILE__, __LINE__);
#else
#define OCL(x) x;
#endif

#if CFL_DEBUG == 1
#define OCL_CHECK(x) cl_int *OCL_ERROR = new cl_int(); \
x; \
_DisplayOpenCLError(*OCL_ERROR, #x, __FILE__, __LINE__); \
delete OCL_ERROR;
#else
#define OCL_CHECK(x) x;
#endif
}
