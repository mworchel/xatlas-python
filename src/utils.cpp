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

#include "utils.hpp"

void checkShape(std::string const& arrayName, pybind11::array array, pybind11::ssize_t expectedLastDimSize, std::optional<pybind11::ssize_t> expectedFirstDimSize)
{
    if (array.ndim() != 2 || array.shape(1) != expectedLastDimSize)
    {
        throw std::invalid_argument(arrayName + " array expected to be Nx" + std::to_string(expectedLastDimSize) + ".");
    }

    if (expectedFirstDimSize && (array.shape(0) != *expectedFirstDimSize))
    {
        throw std::invalid_argument(arrayName + " array has invalid number of elements in the first dimension (expected " + std::to_string(*expectedFirstDimSize) + ", got " + std::to_string(array.shape(0)) + ")");
    }
}