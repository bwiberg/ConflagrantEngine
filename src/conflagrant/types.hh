/**
 * Created by bwiberg on 9/11/17.
 */

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <json/json.h>

// common STL includes

#include <memory>
#include <string>
#include <array>
#include <vector>
#include <list>

#include <filesystem/path.h>
#include <filesystem/resolver.h>

#ifdef __linux__

#include <bits/stdint-uintn.h>
#include <bits/stdint-intn.h>

#endif

namespace cfl {
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;

typedef std::string string;

typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;

typedef glm::dvec2 dvec2;
typedef glm::dvec3 dvec3;
typedef glm::dvec4 dvec4;

typedef glm::uvec2 uvec2;
typedef glm::uvec3 uvec3;
typedef glm::uvec4 uvec4;

typedef glm::ivec2 ivec2;
typedef glm::ivec3 ivec3;
typedef glm::ivec4 ivec4;

typedef glm::bvec2 bvec2;
typedef glm::bvec3 bvec3;
typedef glm::bvec4 bvec4;

typedef glm::mat2 mat2;
typedef glm::mat3 mat3;
typedef glm::mat4 mat4;

// external/filesystem

typedef filesystem::path Path;
typedef filesystem::resolver PathResolver;

// jsoncpp

typedef Json::Value SceneDescription;
}
