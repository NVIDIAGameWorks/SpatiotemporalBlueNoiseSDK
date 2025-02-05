/*
* Copyright (c) 2024, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

#pragma once

#include <vector>

#include "Utils/Dimensions3D.h"

template<typename T>
struct SAData
{
public:
    SAData(const Dimensions3D& _dims) :
        dims(_dims),
        numPixels(dims.x * dims.y * dims.z),
        cells(numPixels),
        pdf(numPixels),
        energy(numPixels, 0.0f)
    {

    }
    const Dimensions3D dims;
    const size_t numPixels;
    std::vector<T> cells;
    std::vector<float> pdf;
    std::vector<float> energy;
};
