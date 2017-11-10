#pragma once

#include <glm/glm.hpp>
#include <assimp/types.h>

inline glm::vec2 &Apply(glm::vec2 &target, aiVector2D const &vec) {
    target.x = vec.x;
    target.y = vec.y;
    return target;
}

inline glm::vec2 Convert(aiVector2D const &vec) {
    glm::vec2 target;
    Apply(target, vec);
    return target;
}

inline glm::vec3 &Apply(glm::vec3 &target, aiVector3D const &vec) {
    target.x = vec.x;
    target.y = vec.y;
    target.z = vec.z;
    return target;
}

inline glm::vec3 Convert(aiVector3D const &vec) {
    glm::vec3 target;
    Apply(target, vec);
    return target;
}

inline glm::vec3 &Apply(glm::vec3 &target, aiColor3D const &rgb) {
    target.r = rgb.r;
    target.g = rgb.g;
    target.b = rgb.b;
    return target;
}

inline glm::vec3 Convert(aiColor3D const &rgb) {
    glm::vec3 target;
    Apply(target, rgb);
    return target;
}

inline glm::vec4 &Apply(glm::vec4 &target, aiColor4D const &rgba) {
    target.r = rgba.r;
    target.g = rgba.g;
    target.b = rgba.b;
    target.a = rgba.a;
    return target;
}

inline glm::vec4 Convert(aiColor4D const &rgba) {
    glm::vec4 target;
    Apply(target, rgba);
    return target;
}

inline glm::mat3 &Apply(glm::mat3 &target, aiMatrix3x3 const &mat) {
    target[0][0] = mat[0][0];
    target[0][1] = mat[1][0];
    target[0][2] = mat[2][0];

    target[1][0] = mat[0][1];
    target[1][1] = mat[1][1];
    target[1][2] = mat[2][1];

    target[2][0] = mat[0][2];
    target[2][1] = mat[1][2];
    target[2][2] = mat[2][2];

    target[3][0] = mat[0][3];
    target[3][1] = mat[1][3];
    target[3][2] = mat[2][3];

    return target;
}

inline glm::mat3 Convert(aiMatrix3x3 const &mat) {
    glm::mat3 target;
    Apply(target, mat);
    return target;
}

inline glm::mat4 &Apply(glm::mat4 &target, aiMatrix4x4 const &mat) {
    target[0][0] = mat[0][0];
    target[0][1] = mat[1][0];
    target[0][2] = mat[2][0];
    target[0][3] = mat[3][0];


    target[1][0] = mat[0][1];
    target[1][1] = mat[1][1];
    target[1][2] = mat[2][1];
    target[1][3] = mat[3][1];

    target[2][0] = mat[0][2];
    target[2][1] = mat[1][2];
    target[2][2] = mat[2][2];
    target[2][3] = mat[3][2];

    target[3][0] = mat[0][3];
    target[3][1] = mat[1][3];
    target[3][2] = mat[2][3];
    target[3][3] = mat[3][3];

    return target;
}

inline glm::mat4 Convert(aiMatrix4x4 const &mat) {
    glm::mat4 target;
    Apply(target, mat);
    return target;
}

inline std::string &Apply(std::string &target, aiString const &str) {
    target = std::string(str.C_Str());
    return target;
}

inline std::string Convert(aiString const &str) {
    std::string target;
    Apply(target, str);
    return target;
}
