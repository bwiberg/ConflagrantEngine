#pragma once

#include <glm/glm.hpp>
#include <assimp/types.h>

inline glm::vec2 operator=(aiVector2D const &vec) {
    return glm::vec2(vec.x, vec.y);
}

inline glm::vec3 operator=(aiVector3D const &vec) {
    return glm::vec3(vec.x, vec.y, vec.z);
}

inline glm::vec3 operator=(aiColor3D const &rgb) {
    return glm::vec3(rgb.r, rgb.g, rgb.b);
}

inline glm::vec4 operator=(aiColor4D const &rgba) {
    return glm::vec4(rgba.r, rgba.g, rgba.b, rgba.a);
}

inline glm::mat3 operator=(aiMatrix3x3 const &mat) {
    glm::mat3 m;

    m[0][0] = mat[0][0];
    m[0][1] = mat[1][0];
    m[0][2] = mat[2][0];

    m[1][0] = mat[0][1];
    m[1][1] = mat[1][1];
    m[1][2] = mat[2][1];

    m[2][0] = mat[0][2];
    m[2][1] = mat[1][2];
    m[2][2] = mat[2][2];

    m[3][0] = mat[0][3];
    m[3][1] = mat[1][3];
    m[3][2] = mat[2][3];

    return m;
}

inline glm::mat4 operator=(aiMatrix4x4 const &mat) {
    glm::mat4 m;

    m[0][0] = mat[0][0];
    m[0][1] = mat[1][0];
    m[0][2] = mat[2][0];
    m[0][3] = mat[3][0];


    m[1][0] = mat[0][1];
    m[1][1] = mat[1][1];
    m[1][2] = mat[2][1];
    m[1][3] = mat[3][1];

    m[2][0] = mat[0][2];
    m[2][1] = mat[1][2];
    m[2][2] = mat[2][2];
    m[2][3] = mat[3][2];

    m[3][0] = mat[0][3];
    m[3][1] = mat[1][3];
    m[3][2] = mat[2][3];
    m[3][3] = mat[3][3];

    return m;
}

inline std::string operator=(aiString const &str) {
    return std::string(str.C_Str());
}
