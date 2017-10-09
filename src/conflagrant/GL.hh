/**
 * Created by Benjamin Wiberg on 14/09/2017.
 */

#pragma once

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#include <GLUT/glut.h>
#elif __linux__
#include <GL/glew.h>
#include <GL/gl.h>
#elif __MINGW32__ || __MINGW64__
#include <GL/glew.h>
#endif

#include <iostream>

namespace cfl {
/**
 * @func _OpenGLErrorToString
 * @brief Converts an OpenGL error to an std::string.
 * @param err The OpenGL error returned by <a href="https://www.opengl.org/sdk/docs/man/html/glGetError.xhtml">glGetError</a>.
 * @returns A string that explain the OpenGL error. These are copied directly from the OpenGL documentation.
 *
 * This function is called by _DisplayOpenGLError to get a user-friendly message in addition to an errror code.
 */
std::string _OpenGLErrorToString(GLenum err);

/**
 * @func _DisplayOpenGLError
 * @brief Prints a user friendly message for the most recent OpenGL error (if any) to stdout.
 * @param command The command that was run.
 * @param file The file from which the command was run.
 * @param line The line in the file where the command was fun.
 *
 * This function is used by the OGL macro to provide debugging information in stdout when an OpenGL API call fails.
 */

inline void _DisplayOpenGLError(std::string command, std::string file, int line) {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << err << " -- " << _OpenGLErrorToString(err) << std::endl;
        std::cerr << "Relevant Command: " << command << std::endl;
        std::cerr << "Location: " << file << " at Line: " << line << std::endl;
    }
}


/**
 * @def OGL
 * @brief Provides an easy to use macro that wraps around OpenGL calls and checks their error.
 * @param x The OpenGL command.
 *
 * This is only defined in the Debug build when CFL_DEBUG is NOT defined. If you are unfamiliar with C++
 * preprocess macros, the __FILE__ macro, or the __LINE__ macro, you can read
 * <a href="http://www.cplusplus.com/doc/tutorial/preprocessor/">this</a> for more information.
 */
#if CFL_DEBUG == 1
#define OGL(x) x; _DisplayOpenGLError(#x, __FILE__, __LINE__);
#else
#define OGL(x) x;
#endif
}