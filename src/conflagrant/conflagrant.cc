/**
 * Created by bwiberg on 9/11/17.
 */

#include "conflagrant.hh"

namespace cfl {
std::string const SayHelloFromExample(int exampleNumber) {
    std::stringstream ss;
    ss << "Hello, from example " << exampleNumber << "!";
    return ss.str();
}
}
