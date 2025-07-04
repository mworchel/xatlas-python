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

namespace nb = nanobind;

void ChartOptions::bind(nb::module_& m)
{
    nb::class_<xatlas::ChartOptions>(m, "ChartOptions")
        .def(nb::init<>());
        //.def_prop_rw("max_chart_area", &xatlas::ChartOptions::maxChartArea, "Don't grow charts to be larger than this. 0 means no limit.")
        //.def_prop_rw("max_boundary_length", &xatlas::ChartOptions::maxBoundaryLength, "Don't grow charts to have a longer boundary than this. 0 means no limit.")
        //.def_prop_rw("normal_deviation_weight", &xatlas::ChartOptions::normalDeviationWeight, "Angle between face and average chart normal.")
        //.def_prop_rw("roundness_weight", &xatlas::ChartOptions::roundnessWeight, "")
        //.def_prop_rw("straightness_weight", &xatlas::ChartOptions::straightnessWeight, "")
        //.def_prop_rw("normal_seam_weight", &xatlas::ChartOptions::normalSeamWeight, "If > 1000, normal seams are fully respected.")
        //.def_prop_rw("texture_seam_weight", &xatlas::ChartOptions::textureSeamWeight, "")
        //.def_prop_rw("max_cost", &xatlas::ChartOptions::maxCost, "If total of all metrics * weights > maxCost, don't grow chart. Lower values result in more charts.")
        //.def_prop_rw("max_iterations", &xatlas::ChartOptions::maxIterations, "Number of iterations of the chart growing and seeding phases. Higher values result in better charts.")
        //.def_prop_rw("use_input_mesh_uvs", &xatlas::ChartOptions::useInputMeshUvs, "Use MeshDecl::vertexUvData for charts.")
        //.def_prop_rw("fix_winding", &xatlas::ChartOptions::fixWinding, "Enforce consistent texture coordinate winding.");
}

void PackOptions::bind(nb::module_& m)
{
    nb::class_<xatlas::PackOptions>(m, "PackOptions")
        .def(nb::init<>());
    //    .def_prop_rw("max_chart_size", &xatlas::PackOptions::maxChartSize, "Charts larger than this will be scaled down. 0 means no limit.")
    //    .def_prop_rw("padding", &xatlas::PackOptions::padding, "Number of pixels to pad charts with.")
    //    .def_prop_rw("texels_per_unit", &xatlas::PackOptions::texelsPerUnit, R"doc(Unit to texel scale. e.g. a 1x1 quad with texelsPerUnit of 32 will take up approximately 32x32 texels in the atlas.
    //If 0, an estimated value will be calculated to approximately match the given resolution.
    //If resolution is also 0, the estimated value will approximately match a 1024x1024 atlas.
	   // )doc")
    //    .def_prop_rw("resolution", &xatlas::PackOptions::resolution, R"doc(If 0, generate a single atlas with texelsPerUnit determining the final resolution. 
    //If not 0, and texelsPerUnit is not 0, generate one or more atlases with that exact resolution. 
    //If not 0, and texelsPerUnit is 0, texelsPerUnit is estimated to approximately match the resolution.)doc")
    //    .def_prop_rw("bilinear", &xatlas::PackOptions::bilinear, "Leave space around charts for texels that would be sampled by bilinear filtering.")
    //    .def_prop_rw("blockAlign", &xatlas::PackOptions::blockAlign, "Align charts to 4x4 blocks. Also improves packing speed, since there are fewer possible chart locations to consider.")
    //    .def_prop_rw("bruteForce", &xatlas::PackOptions::bruteForce, "Slower, but gives the best result. If false, use random chart placement.")
    //    .def_prop_rw("create_image", &xatlas::PackOptions::createImage, "Create Atlas::image.")
    //    .def_prop_rw("rotate_charts_to_axis", &xatlas::PackOptions::rotateChartsToAxis, "Rotate charts to the axis of their convex hull.")
    //    .def_prop_rw("rotate_charts", &xatlas::PackOptions::rotateCharts, "Rotate charts to improve packing.");
}
