#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>
#include <entityx/Entity.h>

namespace cfl {
namespace event {
struct CameraChanged {
    entityx::Entity oldCamera, newCamera;
};

struct Quit {};
}
} // namespace cfl
