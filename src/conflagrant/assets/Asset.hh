#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>

namespace cfl {
namespace assets {
/**
 * @brief Marker interface for assets
 */
struct Asset {
    virtual ~Asset() {};
};
}
} // namespace cfl
