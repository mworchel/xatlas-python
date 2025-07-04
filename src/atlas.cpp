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
#include <string>

namespace nb = nanobind;

Atlas::Atlas()
{
    m_atlas = xatlas::Create();
}

Atlas::~Atlas()
{
    xatlas::Destroy(m_atlas);
}

void Atlas::addMesh(ContiguousArray<float>                positions,
                    ContiguousArray<std::uint32_t>        indices,
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

void Atlas::addUvMesh(ContiguousArray<float>                   uvs,
                      ContiguousArray<std::uint32_t>           indices,
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
        nb::print("--- Generated Atlas ---");
        nb::print(("Utilization: " + std::to_string(m_atlas->utilization[0] * 100.f) + "%").c_str());
        nb::print(("Charts: " + std::to_string(m_atlas->chartCount)).c_str());
        nb::print(("Size: " + std::to_string(m_atlas->width) + "x" + std::to_string(m_atlas->height)).c_str());
        nb::print("");
    }
}

MeshResult Atlas::getMesh(std::uint32_t index) const
{
    if (index >= m_atlas->meshCount)
    {
        throw std::out_of_range("Mesh index " + std::to_string(index) + " out of bounds for atlas with " + std::to_string(m_atlas->meshCount) + " meshes.");
    }

    auto const& mesh = m_atlas->meshes[index];

    size_t                         shape1[] = {mesh.vertexCount};
    ContiguousArray<std::uint32_t> mapping(nullptr, 1, shape1);
    size_t                         shape2[] = {mesh.vertexCount, 2};
    ContiguousArray<float>         uvs(nullptr, 2, shape2);

    //auto mapping_ = mapping.mutable_unchecked<1>();
    //auto uvs_     = uvs.mutable_unchecked<2>();
    //for (size_t v = 0; v < static_cast<size_t>(mesh.vertexCount); ++v)
    //{
    //    auto const& vertex = mesh.vertexArray[v];

    //    mapping_(v) = vertex.xref;

    //    uvs_(v, 0) = vertex.uv[0] / m_atlas->width;
    //    uvs_(v, 1) = vertex.uv[1] / m_atlas->height;
    //}

    //py::array_t<std::uint32_t> indices(py::array::ShapeContainer{mesh.indexCount / 3, 3U});

    //auto indices_ = indices.mutable_unchecked<2>();
    //for (size_t f = 0; f < static_cast<size_t>(mesh.indexCount) / 3; ++f)
    //{
    //    indices_(f, 0) = mesh.indexArray[f*3 + 0];
    //    indices_(f, 1) = mesh.indexArray[f*3 + 1];
    //    indices_(f, 2) = mesh.indexArray[f*3 + 2];
    //}

    return std::make_tuple(mapping, mapping, uvs);
}

float Atlas::getUtilization(std::uint32_t index) const
{
    if (index >= m_atlas->atlasCount)
    {
        throw std::out_of_range("Atlas index " + std::to_string(index) + " out of bounds.");
    }

    return m_atlas->utilization[index];
}

ContiguousArray<std::uint8_t> Atlas::getChartImage(std::uint32_t index) const
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

    // Initialize image data
    size_t        numElements = m_atlas->height * m_atlas->width * 3U;
    std::uint8_t* imageData   = new std::uint8_t[numElements];
    memset(imageData, 0, sizeof(std::uint8_t) * numElements);

    uint32_t width  = m_atlas->width;
    uint32_t height = m_atlas->height;
    auto     pixel  = [&imageData, width](uint32_t y, uint32_t x, uint32_t c) -> std::uint8_t&
    {
        return imageData[(y * width + x) * 3U + c];
    };

    size_t offset = m_atlas->width * m_atlas->height * index;
    for (uint32_t y = 0; y < m_atlas->height; ++y)
    {
        for (uint32_t x = 0; x < m_atlas->width; ++x)
        {
            uint32_t const data = m_atlas->image[x + y * m_atlas->width + offset];

            if (data == 0)
            {
                pixel(y, x, 0) = pixel(y, x, 1) = pixel(y, x, 2) = 0;
                continue;
            }

            const uint32_t chartIndex = data & xatlas::kImageChartIndexMask;
            if (data & xatlas::kImageIsPaddingBit)
            {
                pixel(y, x, 0)  = 0;
                pixel(y, x, 1)  = 0;
                pixel(y, x, 2)  = 255;
            }
            else if (data & xatlas::kImageIsBilinearBit)
            {
                pixel(y, x, 0)  = 0;
                pixel(y, x, 1)  = 255;
                pixel(y, x, 2)  = 0;
            }
            else
            {
                auto color = chartColors[chartIndex];

                pixel(y, x, 0)  = color[0];
                pixel(y, x, 1)  = color[1];
                pixel(y, x, 2)  = color[2];
            }
        }
    }

    // Delete the image data when the 'owner' capsule expires
    nb::capsule owner(imageData, [](void* p) noexcept
                      { delete[] (std::uint8_t*)p; });

    ContiguousArray<std::uint8_t> image(
        /* data = */ imageData,
        /* shape = */ {height, width, 3},
        /* owner = */ owner
    );

    return image;
}

void Atlas::bind(nb::module_& m)
{
    nb::class_<Atlas>(m, "Atlas")
        .def(nb::init<>())
        .def("add_mesh", &Atlas::addMesh, nb::arg("positions"), nb::arg("indices"), nb::arg("normals") = std::nullopt, nb::arg("uvs") = std::nullopt)
        .def("add_uv_mesh", &Atlas::addUvMesh, nb::arg("uvs"), nb::arg("indices"), nb::arg("face_materials") = std::nullopt)
        .def("generate", &Atlas::generate, nb::arg("chart_options") = xatlas::ChartOptions(), nb::arg("pack_options") = xatlas::PackOptions(), nb::arg("verbose") = false)
        .def("get_mesh", &Atlas::getMesh, nb::arg("mesh_index"))
        .def("get_utilization", &Atlas::getUtilization, nb::arg("atlas_index"))
        .def("get_chart_image", &Atlas::getChartImage, nb::arg("atlas_index"))
        .def_prop_ro("atlas_count", [](Atlas const& self) { return self.m_atlas->atlasCount; })
        .def_prop_ro("mesh_count", [](Atlas const& self) { return self.m_atlas->meshCount; })
        .def_prop_ro("chart_count", [](Atlas const& self) { return self.m_atlas->chartCount; })
        .def_prop_ro("width", [](Atlas const& self) { return self.m_atlas->width; })
        .def_prop_ro("height", [](Atlas const& self) { return self.m_atlas->height; })
        .def_prop_ro("texels_per_unit", [](Atlas const& self) { return self.m_atlas->texelsPerUnit; })
        .def_prop_ro("utilization", [](Atlas const& self){ return self.getUtilization(0); })
        .def_prop_ro("chart_image", [](Atlas const& self){ return self.getChartImage(0); })

        // Convenience bindings
        .def("__len__", [](Atlas const& self) { return self.m_atlas->meshCount; })
        .def("__getitem__", &Atlas::getMesh);
}
