#pragma once

#include <conflagrant/types.hh>

namespace cfl {
namespace procgen {
template<typename T>
struct Factory {
    virtual string const GetName() const = 0;

    virtual std::shared_ptr<T> Create() const = 0;
};
} // namespace procgen
} // namespace cfl
