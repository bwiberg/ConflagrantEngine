#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>

namespace cfl {
template<typename T>
class Singleton {
    Singleton(Singleton const &) = default;

    Singleton &operator=(Singleton const &) {};

public:
    static std::shared_ptr<T> GetInstance();

    static void Destroy();

protected:
    static std::shared_ptr<T> Instance;

    Singleton() = default;

    ~Singleton() = default;
};

template<typename T>
typename std::shared_ptr<T> Singleton<T>::Instance = 0;

template<typename T>
std::shared_ptr<T> Singleton<T>::GetInstance() {
    if (!Instance) {
        Singleton<T>::Instance = std::make_shared<T>();
    }

    return Singleton<T>::Instance;
}

template<typename T>
void Singleton<T>::Destroy() {
    Singleton<T>::Instance = nullptr;
}
} // namespace cfl
