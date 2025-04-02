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
#include "options.hpp"
#include "utils.hpp"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <xatlas.h>

namespace py = pybind11;

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

auto parametrize(ContiguousArray<float> const&         positions,
                 ContiguousArray<std::uint32_t> const& indices,
                 std::optional<ContiguousArray<float>> normals = std::nullopt,
                 std::optional<ContiguousArray<float>> uvs     = std::nullopt)
{
    Atlas atlas;
    atlas.addMesh(positions, indices, normals, uvs);
    atlas.generate();
    return atlas.getMesh(0);
}

void exportObj(std::string const&                            path,
               ContiguousArray<float> const&                 positions,
               std::optional<ContiguousArray<std::uint32_t>> indices = std::nullopt,
               std::optional<ContiguousArray<float>>         uvs     = std::nullopt,
               std::optional<ContiguousArray<float>>         normals = std::nullopt)
{
    // Perform sanity checks on the inputs
    checkShape("Position", positions, 3);
    if (indices)
    {
        checkShape("Index", *indices, 3);
    }
    if (normals)
    {
        checkShape("Normal", *normals, 3, positions.shape(0));
    }
    if (uvs)
    {
        checkShape("Texture coordinates", *uvs, 2, positions.shape(0));
    }

    std::ofstream file(path);

    if (!file.is_open())
    {
        throw std::invalid_argument("Cannot open path " + path);
    }

    // Write the vertex positions
    for (py::ssize_t v = 0; v < positions.shape(0); ++v)
    {
        float const* position = positions.data(v, 0);
        file << "v " << position[0] << " " << position[1] << " " << position[2] << std::endl;
    }

    // Write the vertex normals
    if (normals)
    {
        for (py::ssize_t v = 0; v < (*normals).shape(0); ++v)
        {
            float const* normal = (*normals).data(v, 0);
            file << "vn " << normal[0] << " " << normal[1] << " " << normal[2] << std::endl;
        }
    }

    // Write the vertex uv coordinates
    if (uvs)
    {
        for (py::ssize_t v = 0; v < (*uvs).shape(0); ++v)
        {
            float const* uv = (*uvs).data(v, 0);
            file << "vt " << uv[0] << " " << uv[1] << std::endl;
        }
    }

    if (indices)
    {
        std::function<std::string(size_t)> formatFace = [](size_t index) { return std::to_string(index); };

        if (normals && uvs)
        {
            formatFace = [](size_t index) { return std::to_string(index) + "/" + std::to_string(index) + "/" + std::to_string(index); };
        }
        else if (normals)
        {
            formatFace = [](size_t index) { return std::to_string(index) + "//" + std::to_string(index); };
        }
        else if (uvs)
        {
            formatFace = [](size_t index) { return std::to_string(index) + "/" + std::to_string(index); };
        }

        // Write the faces
        for (py::ssize_t f = 0; f < (*indices).shape(0); ++f)
        {
            std::uint32_t const* face = (*indices).data(f, 0);

            file << "f " << formatFace(static_cast<size_t>(face[0]) + 1) << " " << formatFace(static_cast<size_t>(face[1]) + 1) << " " << formatFace(static_cast<size_t>(face[2]) + 1) << std::endl;
        }
    }
}

PYBIND11_MODULE(xatlas, m)
{
    // Bindings
    ChartOptions::bind(m);
    PackOptions::bind(m);
    Atlas::bind(m);

    // Convenience functions
    m.def("parametrize", &parametrize, py::arg("positions"), py::arg("indices"), py::arg("normals") = std::nullopt, py::arg("uvs") = std::nullopt);

    // I/O functions
    m.def("export", &exportObj, py::arg("path"), py::arg("positions"), py::arg("indices") = std::nullopt, py::arg("uvs") = std::nullopt, py::arg("normals") = std::nullopt);

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}