#pragma once

#include "utils.hpp"

#include <pybind11/pybind11.h>

#include <xatlas.h>

#include <cstdint>
#include <optional>
#include <tuple>

using MeshResult = std::tuple<ContiguousArray<std::uint32_t>, ContiguousArray<std::uint32_t>, ContiguousArray<float>>;

class Atlas
{
public:
    Atlas();

    virtual ~Atlas();

    void addMesh(ContiguousArray<float> const& positions,
                 ContiguousArray<std::uint32_t> const& indices,
                 std::optional<ContiguousArray<float>> normals = std::nullopt,
                 std::optional<ContiguousArray<float>> uvs = std::nullopt);

    void generate(xatlas::ChartOptions const& chartOptions = xatlas::ChartOptions(), xatlas::PackOptions const& packOptions = xatlas::PackOptions());

    MeshResult getMesh(std::uint32_t index);

    auto getAtlasCount() const
    {
        return m_atlas->atlasCount;
    }

    auto getMeshCount() const
    {
        return m_atlas->meshCount;
    }

    auto getChartCount() const
    {
        return m_atlas->chartCount;
    }

    auto getWidth() const
    {
        return m_atlas->width;
    }

    auto getHeight() const
    {
        return m_atlas->height;
    }

    auto getTexelsPerUnit() const
    {
        return m_atlas->texelsPerUnit;
    }

    auto getUtilization() const
    {
        return m_atlas->utilization;
    }

    static void bind(pybind11::module& m);

private:
    xatlas::Atlas* m_atlas;
};