#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/GL.hh>

#include <ostream>

namespace cfl {
/**
 * @brief Output stream for serializing a Serializable.
 */
class WriteStream {
public:
    static bool constexpr IsWriting = true;
    static bool constexpr IsReading = false;

    explicit WriteStream(std::ostream &os) : os(os) {}

    template<typename TValue>
    bool SerializeValue(TValue const& value) {
        os << value;
        return !os.bad(); // todo verify this
    }

private:
    std::ostream &os;
};
} // namespace cfl
