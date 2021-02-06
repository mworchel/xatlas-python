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
        checkShape("Normal", *normals, 3, positions.shape()[0]);
    }
    if (uvs)
    {
        checkShape("Texture coordinates", *uvs, 2, positions.shape()[0]);
    }

    // Fill the mesh declaration
    xatlas::MeshDecl meshDecl;

    meshDecl.vertexCount          = static_cast<std::uint32_t>(positions.shape()[0]);
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

    xatlas::AddMesh(m_atlas, meshDecl);
}

void Atlas::generate(xatlas::ChartOptions const& chartOptions, xatlas::PackOptions const& packOptions)
{
    xatlas::Generate(m_atlas, chartOptions, packOptions);
    py::print("--- Generated Atlas ---");
    py::print("Utilization: " + std::to_string(m_atlas->utilization[0] * 100.f) + "%");
    py::print("Charts: " + std::to_string(m_atlas->chartCount));
    py::print("Size: " + std::to_string(m_atlas->width) + "x" + std::to_string(m_atlas->height));
    py::print("");
}

MeshResult Atlas::getMesh(std::uint32_t index)
{
    if (index >= m_atlas->meshCount)
    {
        throw std::out_of_range("Mesh index " + std::to_string(index) + " out of bounds for atlas with " + std::to_string(m_atlas->meshCount) + " meshes.");
    }

    // Collect the mesh
    auto const& mesh = m_atlas->meshes[index];

    std::vector<std::uint32_t> vertexMappingOut(mesh.vertexCount);
    std::vector<float>         uvsOut(static_cast<size_t>(mesh.vertexCount) * 2);
    std::vector<std::uint32_t> indicesOut(mesh.indexCount);

    for (size_t v = 0; v < static_cast<size_t>(mesh.vertexCount); ++v)
    {
        auto const& vertex = mesh.vertexArray[v];

        vertexMappingOut[v] = vertex.xref;

        uvsOut[v * 2 + 0] = vertex.uv[0] / m_atlas->width;
        uvsOut[v * 2 + 1] = vertex.uv[1] / m_atlas->height;
    }

    for (size_t f = 0; f < static_cast<size_t>(mesh.indexCount); ++f)
    {
        indicesOut[f] = mesh.indexArray[f];
    }

    return std::make_tuple(
        ContiguousArray<std::uint32_t>(std::vector<py::ssize_t>{mesh.vertexCount}, vertexMappingOut.data()),
        ContiguousArray<std::uint32_t>(std::vector<py::ssize_t>{mesh.indexCount / 3, 3}, indicesOut.data()),
        ContiguousArray<float>(std::vector<py::ssize_t>{mesh.vertexCount, 2}, uvsOut.data()));
}

void Atlas::bind(py::module& m)
{
    py::class_<Atlas>(m, "Atlas")
        .def(py::init<>())
        .def("add_mesh", &Atlas::addMesh, py::arg("positions"), py::arg("indices"), py::arg("uvs") = std::nullopt, py::arg("normals") = std::nullopt)
        .def("generate", &Atlas::generate, py::arg("chart_options") = xatlas::ChartOptions(), py::arg("pack_options") = xatlas::PackOptions())
        .def("get_mesh", &Atlas::getMesh)
        .def_property_readonly("atlas_count", [](Atlas const& self) { return self.m_atlas->atlasCount; })
        .def_property_readonly("mesh_count", [](Atlas const& self) { return self.m_atlas->meshCount; })
        .def_property_readonly("chart_count", [](Atlas const& self) { return self.m_atlas->chartCount; })
        .def_property_readonly("width", [](Atlas const& self) { return self.m_atlas->width; })
        .def_property_readonly("height", [](Atlas const& self) { return self.m_atlas->height; })
        .def_property_readonly("texels_per_unit", [](Atlas const& self) { return self.m_atlas->texelsPerUnit; })
        .def_property_readonly("utilization", [](Atlas const& self) { return self.m_atlas->utilization; })

        // Convenience bindings
        .def("__len__", [](Atlas const& self) { return self.m_atlas->meshCount; })
        .def("__getitem__", &Atlas::getMesh);
}
