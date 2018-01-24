#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/serialization/serialize.hh>
#include <conflagrant/assets/Mesh.hh>
#include <conflagrant/procgen/parameters/Parameters.hh>
#include <conflagrant/Factory.hh>

#include <unordered_map>

namespace cfl {
namespace procgen {
struct MeshGenerator {
    virtual Parameters GetDefaultParameters() const = 0;

    virtual std::shared_ptr<assets::Mesh> operator()(Parameters const& parameters) const = 0;
};

struct MeshGeneratorFactory : public Factory<MeshGenerator> {
};

template<typename T>
struct ConcreteMeshGeneratorFactory : public MeshGeneratorFactory {
    using GeneratedType = T;

    const string GetName() const override {
        return GeneratedType::Name;
    }

    std::shared_ptr<MeshGenerator> Create() const override {
        return std::make_shared<GeneratedType>();
    }
};

extern std::unordered_map<string, std::shared_ptr<MeshGeneratorFactory>> MeshGeneratorFactoriesByName;

#define REGISTER_MESH_GENERATOR(T) cfl::procgen::MeshGeneratorFactoriesByName[T::Name] = \
std::static_pointer_cast<cfl::procgen::MeshGeneratorFactory>(std::make_shared<cfl::procgen::ConcreteMeshGeneratorFactory<T>>())
} // namespace procgen
} // namespace cfl
