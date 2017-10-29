#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>

#include <istream>

namespace cfl {
/**
 * @brief Output stream for de-serializing a Serializable
 */
class ReadStream {
public:
    static constexpr bool IsWriting = false;
    static constexpr bool IsReading = true;

    explicit ReadStream(std::istream &is) : is(is) {}

    template<typename TValue>
    bool SerializeValue(TValue &value) {
        is >> value;
        return !is.bad(); // todo verify this
    }

private:
    std::istream &is;
};
} // namespace cfl
