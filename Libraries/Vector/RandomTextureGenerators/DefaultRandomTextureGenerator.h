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

#include <algorithm>
#include <vector>

#include "pcg_basic.h"

#include "RandomTextureGenerators/PDFStats.h"

template<typename T, T(*RandomTGenerator)(pcg32_random_t& rng)>
class DefaultRandomTextureGenerator
{
public:
    PDFStats Generate(std::vector<T>& cells, std::vector<float>& pdf, pcg32_random_t& rng)
    {
        PDFStats stats;
        for (size_t index = 0; index < cells.size(); ++index)
        {
            cells[index] = RandomTGenerator(rng);
            pdf[index] = 1.0f;
            stats.max = std::max(pdf[index], stats.max);
            stats.min = std::min(pdf[index], stats.min);
        }
        return stats;
    }
};