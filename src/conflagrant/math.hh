#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>

#include <cmath>

namespace cfl {
namespace math {

#define PI_MANY_DECIMALS    3.14159265358979323846264338327
#define IPI_MANY_DECIMALS   0.31830988618379067153776752674

constexpr double PI_D = static_cast<double>(PI_MANY_DECIMALS);
constexpr float PI_F = static_cast<float>(PI_MANY_DECIMALS);

constexpr double IPI_D = static_cast<double>(PI_MANY_DECIMALS);
constexpr float IPI_F = static_cast<float>(IPI_MANY_DECIMALS);

constexpr double RAD2DEG_D = static_cast<double>(180 * IPI_MANY_DECIMALS);
constexpr float RAD2DEG_F = static_cast<float>(180 * IPI_MANY_DECIMALS);

constexpr double DEG2RAD_D = static_cast<double>(PI_MANY_DECIMALS / 180);
constexpr float DEG2RAD_F = static_cast<float>(PI_MANY_DECIMALS / 180);

template<typename T>
inline T Clamp(T const &value, T const &minimum, T const &maximum) {
    if (value < minimum)
        return minimum;
    if (value > maximum)
        return maximum;
    return value;
}

template<typename TValue, typename TMin, typename TMax>
inline TValue Clamp(TValue const &value, TMin const &minimum, TMax const &maximum) {
    return Clamp(value, static_cast<TValue>(minimum), static_cast<TValue>(maximum));
}

template<typename TBase, typename TExponent>
inline TBase Pow(TBase base, TExponent exponent) {
    return static_cast<TBase>(pow(static_cast<double>(base), static_cast<double>(exponent)));
};

template<>
inline int Pow(int base, int exponent) {
    int result = 1;
    while (exponent)
    {
        if (exponent & 1)
            result *= base;
        exponent >>= 1;
        base *= base;
    }
    return result;
}

template<typename TFov, typename TSize>
inline TFov FocalLengthFromFovx(TFov fovxDegrees, TSize width) {
    auto dfovxRadians = DEG2RAD_D * static_cast<double>(fovxDegrees);
    auto dwidth = static_cast<double>(width);

    return static_cast<TFov>(0.5 * dwidth / tan(0.5 * dfovxRadians));
}

template<typename TFov, typename TSize>
inline TFov FocalLengthFromFovy(TFov fovyDegrees, TSize height) {
    auto dfovyRadians = DEG2RAD_D * static_cast<double>(fovyDegrees);
    auto dheight = static_cast<double>(height);

    return static_cast<TFov>(0.5 * dheight / tan(0.5 * dfovyRadians));
}

template<typename TFov, typename TSize>
inline TFov Fovx(TFov fovyDegrees, TFov width, TSize height) {
    auto dfovyDegrees = static_cast<double>(fovyDegrees);
    auto df = FocalLengthFromFovy(dfovyDegrees, height);
    auto dwidth = static_cast<double>(width);

    auto dfovx = RAD2DEG_D * 2 * atan2(0.5 * dwidth, df);

    return static_cast<TFov>(dfovx);
}

template<typename TFov, typename TSize>
inline TFov Fovy(TFov fovxDegrees, TSize width, TSize height) {
    auto dfovxDegrees = static_cast<double>(fovxDegrees);
    auto df = FocalLengthFromFovx(dfovxDegrees, width);
    auto dheight = static_cast<double>(height);

    auto dfovy = RAD2DEG_D * 2 * atan2(0.5 * dheight, df);

    return static_cast<TFov>(dfovy);
}

template<typename T>
inline T Repeat(T const& value, T const& min, T const& max) {
    T copy = value;
    T const range = 1 + max - min;

    while (copy < min) {
        copy += range;
    }

    while (copy > max) {
        copy -= range;
    }

    return copy;
}

template<typename TValue, typename TMin, typename TMax>
inline TValue Repeat(TValue const& value, TMin const& min, TMax const& max) {
    return Repeat(value, static_cast<TValue>(min), static_cast<TValue>(max));
}
} // namespace math
} // namespace cfl
