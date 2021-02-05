#include "utils.hpp"

void checkShape(std::string const& arrayName, pybind11::array array, pybind11::ssize_t expectedLastDimSize, std::optional<pybind11::ssize_t> expectedFirstDimSize)
{
    if (array.ndim() != 2 || array.shape()[1] != expectedLastDimSize)
    {
        throw std::invalid_argument(arrayName + " array expected to be Nx" + std::to_string(expectedLastDimSize) + ".");
    }

    if (expectedFirstDimSize && (array.shape()[0] != *expectedFirstDimSize))
    {
        throw std::invalid_argument(arrayName + " has invalid number of elements in the first dimension (expected " + std::to_string(*expectedFirstDimSize) + ", got " + std::to_string(array.shape()[0]) + ")");
    }
}