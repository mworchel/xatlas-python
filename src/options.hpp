#pragma once

#include <pybind11/pybind11.h>

class ChartOptions
{
public:
    static void bind(pybind11::module& m);
};

class PackOptions
{
public:
    static void bind(pybind11::module& m);
};