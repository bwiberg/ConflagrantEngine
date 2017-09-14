/**
 * Created by Benjamin Wiberg on 14/09/2017.
 */

#include <conflagrant/GL.hh>

std::string cfl::_OpenGLErrorToString(GLenum err) {
// Error messages copied from the glGetError documentation: https://www.opengl.org/sdk/docs/man/docbook4/xhtml/glGetError.xml
    switch (err) {
        case GL_NO_ERROR:
            return "No error has been recorded. The value of this symbolic constant is guaranteed to be 0.";
        case GL_INVALID_ENUM:
            return "An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.";
        case GL_INVALID_VALUE:
            return "A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.";
        case GL_INVALID_OPERATION:
            return "The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag.";
        case GL_OUT_OF_MEMORY:
            return "There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.";
        case GL_STACK_UNDERFLOW:
            return "An attempt has been made to perform an operation that would cause an internal stack to underflow.";
        case GL_STACK_OVERFLOW:
            return "An attempt has been made to perform an operation that would cause an internal stack to overflow.";
        default:
            break;
    }
    return "Unspecified error";
}
