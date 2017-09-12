/**
 * Created by bwiberg on 9/12/17.
 */

#pragma once

#include <algorithm>

namespace utility {
template<typename TContainer>
bool contains(TContainer const& container, typename TContainer::value_type const& value) {
    return std::find(container.cbegin(), container.cend(), value) != container.cend();
}

template<typename TContainer1, typename TContainer2>
bool containsAtLeastOne(TContainer1 const& supercontainer, TContainer2 const& subcontainer) {
    for (auto const& value : subcontainer) {
        if (contains(supercontainer, value)) {
            return true;
        }
    }
    return false;
};
}
