/**
 * Created by bwiberg on 9/13/17.
 */

#pragma once

#include <conflagrant/GL.hh>
#include <conflagrant/gl/Mesh.hh>

namespace cfl {
namespace comp {
/**
 * @brief Component that references a Mesh in OpenGL memory.
 */
struct Mesh {
    std::shared_ptr<gl::Mesh> glMesh;

    inline static string const GetName() {
        return "Mesh";
    }

    template<typename TSerializer>
    static bool Serialize(Json::Value &json, Mesh &mesh) {
        // SERIALIZE(json, name.value);
        return true;
    }

    static bool DrawWithImGui(Mesh &mesh) {
        return true;
    }
};
} // namespace comp
} // namespace cfl
