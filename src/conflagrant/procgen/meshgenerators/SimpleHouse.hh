#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/procgen/MeshGenerator.hh>
#include <conflagrant/procgen/graph/operations/Split.hh>
#include <conflagrant/procgen/graph/operations/Extrude.hh>
#include <conflagrant/procgen/graph/operations/Inset.hh>
#include <conflagrant/procgen/asset.hh>

#include <random>

namespace cfl {
namespace procgen {
struct SimpleHouse : public MeshGenerator {
    static constexpr auto Name = "SimpleHouse";

    static constexpr IntParameterKey PSeed = "Seed";

    static constexpr IntParameterKey PFloors = "Floors";

    static constexpr FloatParameterKey PBaseDimensions = "BaseDimensions";
    static constexpr FloatParameterKey PFloorHeight = "FloorHeight";

    static constexpr FloatParameterKey PWindowProbabilities = "WindowProbabilities";
    static constexpr FloatParameterKey PWindowWidth = "WindowWidth";
    static constexpr FloatParameterKey PWindowHeight = "WindowHeight";
    static constexpr FloatParameterKey PWindowPadding = "WindowPadding";
    static constexpr FloatParameterKey PWindowPaneOffset = "WindowPaneOffset";

    static constexpr FloatParameterKey PRoofHeight = "RoofHeight";
    static constexpr FloatParameterKey PRoofRidgeLength = "RoofRidgeLength";
    static constexpr FloatParameterKey PRoofOverhang = "RoofOverhang";

    Parameters GetDefaultParameters() const override {
        Parameters p;

        p[PSeed] = {0};

        p[PFloors] = {2};

        p[PBaseDimensions] = {5.f, 3.f};
        p[PFloorHeight] = {1.5f};

        p[PWindowProbabilities] = {0.f, 1.f, 0.f, 1.f};
        p[PWindowWidth] = {1.f};
        p[PWindowHeight] = {1.f};
        p[PWindowPadding] = {.5f};
        p[PWindowPaneOffset] = {.1f};

        p[PRoofHeight] = {1.f};
        p[PRoofRidgeLength] = {3.f};
        p[PRoofOverhang] = {0.25f};

        return p;
    }

    std::shared_ptr<assets::Mesh> operator()(Parameters const &p) const override {
        using namespace procgen;

#define ADD(polygon) result.push_back(polygon)
#define ADD_ALL(strip) result.insert(result.end(), (strip).begin(), (strip).end())

        PolygonStrip<4> result;
        std::mt19937 gen(static_cast<unsigned long>(p[PSeed][0]));
        std::uniform_real_distribution<float> distribution;

        {
            float const Width = p[PBaseDimensions][0];
            float const Length = p[PBaseDimensions][1];

            float const HalfWidth = .5f * Width;
            float const HalfLength = .5f * Length;

            float const FloorHeight = p[PFloorHeight][0];

            Polygon<4> base = {vec3(-HalfWidth, 0.f, -HalfLength),
                               vec3(HalfWidth, 0.f, -HalfLength),
                               vec3(HalfWidth, 0.f, HalfLength),
                               vec3(-HalfWidth, 0.f, HalfLength)};

            auto walls = Extrude(base, p[PFloors][0] * p[PFloorHeight][0]);

            uint iwall = 0;
            for (auto &wall : walls) {
                auto const WallSize = ((iwall + 3) % 2 == 0) ? Width : Length;

                auto const NumWindows = std::max(static_cast<uint>(WallSize / p[PWindowWidth][0]), 1u);

                float const WindowProbability = math::Clamp(p[PWindowProbabilities][iwall], 0, 1);

                float const WindowWidth = WallSize / NumWindows;
                float const WindowHorizontalPadding = 0.5f * std::min(2 * p[PWindowPadding][0], WindowWidth);

                float const WindowHeight = std::min(p[PWindowHeight][0], FloorHeight);
                float const WindowVerticalPadding = 0.5f * (FloorHeight - WindowHeight);

                std::array<float, 4> const WindowInsetDistances = {WindowHorizontalPadding, WindowVerticalPadding,
                                                                   WindowHorizontalPadding, WindowVerticalPadding};

                auto levels = Split<SplitAxis::VERTICAL>(wall, p[PFloors][0]);

                uint ilevel = 0;
                for (auto &level : levels) {
                    auto windows = Split<SplitAxis::HORIZONTAL>(level, NumWindows);

                    uint iwindow = 0;
                    for (auto &window : windows) {
                        if (distribution(gen) > WindowProbability) {
                            continue;
                        }

                        auto windowInsets = Inset(window, WindowInsetDistances);
                        auto windowExtrudeds = Extrude(window, p[PWindowPaneOffset][0]);

                        ++iwindow;
                        ADD_ALL(windowInsets);
                        ADD_ALL(windowExtrudeds);
                    }

                    ++ilevel;
                    ADD_ALL(windows);
                }

                ++iwall;
            }

            {
                auto const RoofHeight = p[PRoofHeight][0];
                auto const RoofOverhang = p[PRoofOverhang][0];
                auto const RoofRidgeLength = p[PRoofRidgeLength][0];

                base[0].z -= RoofOverhang;
                base[1].z -= RoofOverhang;

                base[1].x += RoofOverhang;
                base[2].x += RoofOverhang;

                base[2].z += RoofOverhang;
                base[3].z += RoofOverhang;

                base[3].x -= RoofOverhang;
                base[0].x -= RoofOverhang;

                auto roofUnderside = base;
                Flip(roofUnderside);

                float const InsetA = RoofOverhang + 0.5f * Length;
                float const InsetB = RoofOverhang + 0.5f * Width - 0.5f * RoofRidgeLength;
                auto roofSides = Inset(base, {InsetA, InsetB, InsetA, InsetB});

                for (auto &roofSide : roofSides) {
                    roofSide[2].y -= RoofHeight;
                    roofSide[3].y -= RoofHeight;
                }

                ADD_ALL(roofSides);
                ADD(roofUnderside);
            }
        }

#undef ADD
#undef ADD_ALL

//        {
//            auto segments = Split(quad, 4, SplitAxis::HORIZONTAL);
//
//            auto extrudedSides = Extrude(segments[1], 1.f);
//            for (auto &extrudedSide : extrudedSides) {
//                auto insets = Inset(extrudedSide, .05f);
//                auto extrudeds = Extrude(extrudedSide, -.01f);
//
//                result.insert(result.end(), insets.begin(), insets.end());
//                result.insert(result.end(), extrudeds.begin(), extrudeds.end());
//            }
//
//            result.insert(result.end(), extrudedSides.begin(), extrudedSides.end());
//            result.insert(result.end(), segments.begin(), segments.end());
//        }

        return ExportMeshAsset(ToMeshDraft<4>(result));
    }
};

constexpr IntParameterKey SimpleHouse::PSeed;

constexpr IntParameterKey SimpleHouse::PFloors;

constexpr FloatParameterKey SimpleHouse::PBaseDimensions;
constexpr FloatParameterKey SimpleHouse::PFloorHeight;

constexpr FloatParameterKey SimpleHouse::PWindowProbabilities;
constexpr FloatParameterKey SimpleHouse::PWindowWidth;
constexpr FloatParameterKey SimpleHouse::PWindowHeight;
constexpr FloatParameterKey SimpleHouse::PWindowPadding;
constexpr FloatParameterKey SimpleHouse::PWindowPaneOffset;

constexpr FloatParameterKey SimpleHouse::PRoofHeight;
constexpr FloatParameterKey SimpleHouse::PRoofRidgeLength;
constexpr FloatParameterKey SimpleHouse::PRoofOverhang;
} // namespace procgen
} // namespace cfl
