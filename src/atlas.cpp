/**
 * MIT License
 * 
 * Copyright (c) 2021 Markus Worchel
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "atlas.hpp"

#include <array>
#include <random>

#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

Atlas::Atlas()
{
    m_atlas = xatlas::Create();
}

Atlas::~Atlas()
{
    xatlas::Destroy(m_atlas);
}

void Atlas::addMesh(ContiguousArray<float> const&         positions,
                    ContiguousArray<std::uint32_t> const& indices,
                    std::optional<ContiguousArray<float>> normals,
                    std::optional<ContiguousArray<float>> uvs)
{
    // Perform sanity checks on the inputs
    checkShape("Position", positions, 3);
    checkShape("Index", indices, 3);
    if (normals)
    {
        checkShape("Normal", *normals, 3, positions.shape(0));
    }
    if (uvs)
    {
        checkShape("Texture coordinate", *uvs, 2, positions.shape(0));
    }

    // Fill the mesh declaration
    xatlas::MeshDecl meshDecl;

    meshDecl.vertexCount          = static_cast<std::uint32_t>(positions.shape(0));
    meshDecl.vertexPositionData   = positions.data();
    meshDecl.vertexPositionStride = sizeof(float) * 3; // X, Y, Z

    meshDecl.indexCount  = static_cast<std::uint32_t>(indices.size());
    meshDecl.indexData   = indices.data();
    meshDecl.indexFormat = xatlas::IndexFormat::UInt32;

    if (normals)
    {
        meshDecl.vertexNormalData   = normals->data();
        meshDecl.vertexNormalStride = sizeof(float) * 3;
    }

    if (uvs)
    {
        meshDecl.vertexUvData   = uvs->data();
        meshDecl.vertexUvStride = sizeof(float) * 2;
    }

    xatlas::AddMeshError error = xatlas::AddMesh(m_atlas, meshDecl);

    if (error != xatlas::AddMeshError::Success) {
        throw std::runtime_error("Adding mesh failed: " + std::string(xatlas::StringForEnum(error)));
    }
}

void Atlas::addUvMesh(ContiguousArray<float> const&            uvs,
                      ContiguousArray<std::uint32_t> const&    indices,
                      std::optional<ContiguousArray<uint32_t>> faceMaterials)
{
    // Perform sanity checks on the inputs
    checkShape("Texture coordinate", uvs, 2);
    checkShape("Index", indices, 3);
    if (faceMaterials)
    {
        checkShape("Face material ID", *faceMaterials, 1, indices.shape(0));
    }

    // Fill the mesh declaration
    xatlas::UvMeshDecl meshDecl;

    meshDecl.vertexCount  = static_cast<std::uint32_t>(uvs.shape(0));
    meshDecl.vertexUvData = uvs.data();
    meshDecl.vertexStride = sizeof(float) * 2; // U, V

    meshDecl.indexCount  = static_cast<std::uint32_t>(indices.size());
    meshDecl.indexData   = indices.data();
    meshDecl.indexFormat = xatlas::IndexFormat::UInt32;

    if (faceMaterials)
    {
        meshDecl.faceMaterialData = faceMaterials->data();
    }

    xatlas::AddMeshError error = xatlas::AddUvMesh(m_atlas, meshDecl);

    if (error != xatlas::AddMeshError::Success)
    {
        throw std::runtime_error("Adding mesh failed: " + std::string(xatlas::StringForEnum(error)));
    }
}

void Atlas::generate(xatlas::ChartOptions const& chartOptions, xatlas::PackOptions const& packOptions, bool verbose)
{
    xatlas::Generate(m_atlas, chartOptions, packOptions);

    if (verbose)
    {
        py::print("--- Generated Atlas ---");
        py::print("Utilization: " + std::to_string(m_atlas->utilization[0] * 100.f) + "%");
        py::print("Charts: " + std::to_string(m_atlas->chartCount));
        py::print("Size: " + std::to_string(m_atlas->width) + "x" + std::to_string(m_atlas->height));
        py::print("");
    }
}

MeshResult Atlas::getMesh(std::uint32_t index) const
{
    if (index >= m_atlas->meshCount)
    {
        throw std::out_of_range("Mesh index " + std::to_string(index) + " out of bounds for atlas with " + std::to_string(m_atlas->meshCount) + " meshes.");
    }

    auto const& mesh = m_atlas->meshes[index];

    py::array_t<std::uint32_t> mapping(py::array::ShapeContainer{mesh.vertexCount});
    py::array_t<float>         uvs(py::array::ShapeContainer{mesh.vertexCount, 2U});

    auto mapping_ = mapping.mutable_unchecked<1>();
    auto uvs_     = uvs.mutable_unchecked<2>();
    for (size_t v = 0; v < static_cast<size_t>(mesh.vertexCount); ++v)
    {
        auto const& vertex = mesh.vertexArray[v];

        mapping_(v) = vertex.xref;

        uvs_(v, 0) = vertex.uv[0] / m_atlas->width;
        uvs_(v, 1) = vertex.uv[1] / m_atlas->height;
    }

    py::array_t<std::uint32_t> indices(py::array::ShapeContainer{mesh.indexCount / 3, 3U});

    auto indices_ = indices.mutable_unchecked<2>();
    for (size_t f = 0; f < static_cast<size_t>(mesh.indexCount) / 3; ++f)
    {
        indices_(f, 0) = mesh.indexArray[f*3 + 0];
        indices_(f, 1) = mesh.indexArray[f*3 + 1];
        indices_(f, 2) = mesh.indexArray[f*3 + 2];
    }

    return std::make_tuple(mapping, indices, uvs);
}

float Atlas::getUtilization(std::uint32_t index) const
{
    if (index >= m_atlas->atlasCount)
    {
        throw std::out_of_range("Atlas index " + std::to_string(index) + " out of bounds.");
    }

    return m_atlas->utilization[index];
}

py::array_t<std::uint8_t> Atlas::getChartImage(std::uint32_t index) const
{
    // Code inspired by xatlas::writeTga

    if (index >= m_atlas->atlasCount)
    {
        throw std::out_of_range("Atlas index " + std::to_string(index) + " out of bounds.");
    }

    if (!m_atlas->image || m_atlas->width == 0 || m_atlas->height == 0)
    {
        throw std::runtime_error("The atlas does not have an image.");
    }

    // Generate a color for each chart
    std::vector<std::array<uint8_t, 3>>         chartColors(m_atlas->chartCount);
    std::uniform_int_distribution<unsigned int> distribution(0, 254); // Original code uses `% 255`, which excludes 255
    constexpr unsigned int const                mix        = 192U;
    size_t                                      chartIndex = 0U;
    for (auto& color : chartColors)
    {
        std::default_random_engine engine(static_cast<unsigned int>(chartIndex++));
        color[0] = static_cast<std::uint8_t>((distribution(engine) + mix) * 0.5f) ;
        color[1] = static_cast<std::uint8_t>((distribution(engine) + mix) * 0.5f);
        color[2] = static_cast<std::uint8_t>((distribution(engine) + mix) * 0.5f);
    }

    // Fill an image with the chart colors
    py::array_t<std::uint8_t> image(py::array::ShapeContainer{ m_atlas->height, m_atlas->width, 3U });

    size_t offset = m_atlas->width * m_atlas->height * index;
    auto image_ = image.mutable_unchecked<3>();
    for (uint32_t y = 0; y < m_atlas->height; ++y)
    {
        for (uint32_t x = 0; x < m_atlas->width; ++x)
        {
            uint32_t const data = m_atlas->image[x + y * m_atlas->width + offset];

            if (data == 0)
            {
                image_(y, x, 0) = image_(y, x, 1) = image_(y, x, 2) = 0;
                continue;
            }

            const uint32_t chartIndex = data & xatlas::kImageChartIndexMask;
            if (data & xatlas::kImageIsPaddingBit)
            {
                image_(y, x, 0) = 0;
                image_(y, x, 1) = 0;
                image_(y, x, 2) = 255;
            }
            else if (data & xatlas::kImageIsBilinearBit)
            {
                image_(y, x, 0) = 0;
                image_(y, x, 1) = 255;
                image_(y, x, 2) = 0;
            }
            else
            {
                auto color = chartColors[chartIndex];

                image_(y, x, 0) = color[0];
                image_(y, x, 1) = color[1];
                image_(y, x, 2) = color[2];
            }
        }
    }

    return image;
}

void Atlas::bind(py::module& m)
{
    py::class_<Atlas>(m, "Atlas")
        .def(py::init<>())
        .def("add_mesh", &Atlas::addMesh, py::arg("positions"), py::arg("indices"), py::arg("normals") = std::nullopt, py::arg("uvs") = std::nullopt)
        .def("add_uv_mesh", &Atlas::addUvMesh, py::arg("uvs"), py::arg("indices"), py::arg("face_materials") = std::nullopt)
        .def("generate", &Atlas::generate, py::arg("chart_options") = xatlas::ChartOptions(), py::arg("pack_options") = xatlas::PackOptions(), py::arg("verbose") = false)
        .def("get_mesh", &Atlas::getMesh, py::arg("mesh_index"))
        .def("get_utilization", &Atlas::getUtilization, py::arg("atlas_index"))
        .def("get_chart_image", &Atlas::getChartImage, py::arg("atlas_index"))
        .def_property_readonly("atlas_count", [](Atlas const& self) { return self.m_atlas->atlasCount; })
        .def_property_readonly("mesh_count", [](Atlas const& self) { return self.m_atlas->meshCount; })
        .def_property_readonly("chart_count", [](Atlas const& self) { return self.m_atlas->chartCount; })
        .def_property_readonly("width", [](Atlas const& self) { return self.m_atlas->width; })
        .def_property_readonly("height", [](Atlas const& self) { return self.m_atlas->height; })
        .def_property_readonly("texels_per_unit", [](Atlas const& self) { return self.m_atlas->texelsPerUnit; })
        .def_property_readonly("utilization", [](Atlas const& self){ return self.getUtilization(0); })
        .def_property_readonly("chart_image", [](Atlas const& self){ return self.getChartImage(0); })

        // Convenience bindings
        .def("__len__", [](Atlas const& self) { return self.m_atlas->meshCount; })
        .def("__getitem__", &Atlas::getMesh);
}
