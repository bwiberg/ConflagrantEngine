#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/assets/Asset.hh>
#include <conflagrant/gl/Texture.hh>

namespace cfl {
namespace assets {
struct Texture1D : public Asset {
    inline explicit Texture1D(gl::Texture1D &&texture) : texture(std::move(texture)) {}

    gl::Texture1D texture;
};

struct Texture2D : public Asset {
    inline explicit Texture2D(gl::Texture2D &&texture) : texture(std::move(texture)) {}

    gl::Texture2D texture;
};

struct Texture3D : public Asset {
    inline explicit Texture3D(gl::Texture3D &&texture) : texture(std::move(texture)) {}

    gl::Texture3D texture;
};
} // namespace assets
} // namespace cfl
