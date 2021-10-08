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

#include "options.hpp"

#include <xatlas.h>

namespace py = pybind11;

void ChartOptions::bind(py::module& m)
{
    py::class_<xatlas::ChartOptions>(m, "ChartOptions")
        .def(py::init<>())
        .def_readwrite("max_chart_area", &xatlas::ChartOptions::maxChartArea)
        .def_readwrite("max_boundary_length", &xatlas::ChartOptions::maxBoundaryLength)
        .def_readwrite("normal_deviation_weight", &xatlas::ChartOptions::normalDeviationWeight)
        .def_readwrite("roundness_weight", &xatlas::ChartOptions::roundnessWeight)
        .def_readwrite("straightness_weight", &xatlas::ChartOptions::straightnessWeight)
        .def_readwrite("normal_seam_weight", &xatlas::ChartOptions::normalSeamWeight)
        .def_readwrite("texture_seam_weight", &xatlas::ChartOptions::textureSeamWeight)
        .def_readwrite("max_cost", &xatlas::ChartOptions::maxCost)
        .def_readwrite("max_iterations", &xatlas::ChartOptions::maxIterations)
        .def_readwrite("use_input_mesh_uvs", &xatlas::ChartOptions::useInputMeshUvs)
        .def_readwrite("fix_winding", &xatlas::ChartOptions::fixWinding);
}

void PackOptions::bind(pybind11::module& m)
{
    py::class_<xatlas::PackOptions>(m, "PackOptions")
        .def(py::init<>())
        .def_readwrite("max_chart_size", &xatlas::PackOptions::maxChartSize, "Charts larger than this will be scaled down. 0 means no limit.")
        .def_readwrite("padding", &xatlas::PackOptions::padding, "Number of pixels to pad charts with.")
        .def_readwrite("texels_per_unit", &xatlas::PackOptions::texelsPerUnit, R"doc(Unit to texel scale. e.g. a 1x1 quad with texelsPerUnit of 32 will take up approximately 32x32 texels in the atlas.
    If 0, an estimated value will be calculated to approximately match the given resolution.
    If resolution is also 0, the estimated value will approximately match a 1024x1024 atlas.
	    )doc")
        .def_readwrite("resolution", &xatlas::PackOptions::resolution, R"doc(If 0, generate a single atlas with texelsPerUnit determining the final resolution. 
    If not 0, and texelsPerUnit is not 0, generate one or more atlases with that exact resolution. 
    If not 0, and texelsPerUnit is 0, texelsPerUnit is estimated to approximately match the resolution.)doc")
        .def_readwrite("bilinear", &xatlas::PackOptions::bilinear, "Leave space around charts for texels that would be sampled by bilinear filtering.")
        .def_readwrite("blockAlign", &xatlas::PackOptions::blockAlign, "Align charts to 4x4 blocks. Also improves packing speed, since there are fewer possible chart locations to consider.")
        .def_readwrite("bruteForce", &xatlas::PackOptions::bruteForce, "Slower, but gives the best result. If false, use random chart placement.")
        .def_readwrite("create_image", &xatlas::PackOptions::createImage, "Create Atlas::image.")
        .def_readwrite("rotate_charts_to_axis", &xatlas::PackOptions::rotateChartsToAxis, "Rotate charts to the axis of their convex hull.")
        .def_readwrite("rotate_charts", &xatlas::PackOptions::rotateCharts, "Rotate charts to improve packing.");
}
