#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/CL.hh>

namespace cfl {
namespace cl {
class Environment {
    Environment() = delete;

    std::unique_ptr<::cl::Device> device;
    std::unique_ptr<::cl::Platform> platform;
    std::unique_ptr<::cl::Context> context;
    std::unique_ptr<::cl::CommandQueue> queue;

public:
    static void InitEnvironment();
};
} // namespace cl
} // namespace cfl
