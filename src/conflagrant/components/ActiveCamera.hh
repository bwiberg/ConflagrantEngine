#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <conflagrant/serialization/serialize.hh>
#include <conflagrant/InputManager.hh>

#include <imgui.h>

namespace cfl {
namespace comp {
struct ActiveCamera {
    static constexpr auto ComponentName = "ActiveCamera";
};
} // namespace comp
} // namespace cfl
