#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/procgen/graph/Polygon.hh>

#include <algorithm>
#include <functional>

namespace cfl {
namespace procgen {
template<size_t PolygonOrder, typename... Args>
using Operation = std::function<PolygonStrip<PolygonOrder>(Polygon<PolygonOrder> &, Args...)>;

template<size_t PolygonOrder>
using Condition = std::function<bool(Polygon<PolygonOrder> const &, size_t)>;

template<size_t PolygonOrder, typename PolygonIteratorRight>
void MoveAll(PolygonStrip<PolygonOrder> &target, PolygonStrip<PolygonOrder> &source) {
    std::move(source.begin(), source.end(), std::back_inserter(target));
    source.erase(source.begin(), source.end());
};

template<size_t PolygonOrder, typename PolygonIterator, typename... OperationArgs>
std::vector<PolygonStrip<PolygonOrder>> AggregateIf(PolygonIterator begin, PolygonIterator end,
                                                    Operation<PolygonOrder, OperationArgs...> const &operation,
                                                    Condition<PolygonOrder> const &condition,
                                                    OperationArgs... args) {
    std::vector<PolygonStrip<PolygonOrder>> ret;
    size_t index = 0;

    while (begin != end) {
        auto &polygon = *begin;
        if (condition(polygon, index)) {
            auto strip = operation(polygon, std::forward<OperationArgs>(args)...);
            ret.push_back(std::move(strip));
        }

        ++index;
    }

    return ret;
};

template<size_t PolygonOrder, typename PolygonIterator, typename... OperationArgs>
std::vector<PolygonStrip<PolygonOrder>> Aggregate(PolygonIterator begin, PolygonIterator end,
                                                  Operation<PolygonOrder, OperationArgs...> const &operation,
                                                  OperationArgs... args) {
    static Condition<PolygonOrder> AlwaysTrue = [](Polygon<PolygonOrder> const &, size_t) {
        return true;
    };

    return AggregateIf<PolygonOrder, PolygonIterator, OperationArgs...>(
            begin, end, operation, AlwaysTrue, std::forward<OperationArgs>(args)...);
};

template<size_t PolygonOrder, typename PolygonStripIterator>
PolygonStrip<PolygonOrder> Reduce(std::vector<PolygonStrip<PolygonOrder>> &polygonStrips) {
    auto ret = std::move(polygonStrips[0]);
    for (size_t i = 1; i < polygonStrips.size(); ++i) {
        MoveAll(ret, polygonStrips[i]);
    }

    polygonStrips.clear();
    return ret;
};
} // namespace procgen
} // namespace cfl
