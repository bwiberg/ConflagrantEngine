#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/procgen/MeshGenerator.hh>

namespace cfl {
namespace procgen {
namespace {
static constexpr IntParameterKey Subdivisions = "Subdivisions";
static constexpr FloatParameterKey Dimensions = "Dimensions";
static constexpr FloatParameterKey Height = "Height";
}

struct Terrain : public MeshGenerator {
    static constexpr auto Name = "Terrain";


    Parameters GetDefaultParameters() const override {
        Parameters p;

        p[Subdivisions] = {10, 10};
        p[Dimensions] = {10.f, 10.f};
        p[Height] = {1.f};

        return p;
    }

    std::shared_ptr<assets::Mesh> operator()(Parameters const &parameters) const override {
        return nullptr;
    }
};
} // namespace procgen
} // namespace cfl
