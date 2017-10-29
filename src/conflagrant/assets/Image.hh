#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>

namespace cfl {
namespace assets {
template<size_t NumChannels>
struct Image2D {
    /**
     * The raw pixels of the image
     */
    std::vector<std::array<char, NumChannels>> pixels;

    /**
     * Resolution of the texture
     */
    uint width, height;
};
} // namespace assets
} // namespace cfl
