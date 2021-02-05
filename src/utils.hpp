#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include <optional>
#include <stdexcept>

template<typename T>
using ContiguousArray = pybind11::array_t<T, pybind11::array::c_style | pybind11::array::forcecast>;


void checkShape(std::string const& arrayName, pybind11::array array, pybind11::ssize_t expectedLastDimSize, std::optional<pybind11::ssize_t> expectedFirstDimSize = std::nullopt);