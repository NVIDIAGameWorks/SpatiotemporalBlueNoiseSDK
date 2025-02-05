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

union Float3;

struct PDFStats;

class CosineWeightedHemisphereFloat3RandomTextureGenerator
{
public:
    PDFStats Generate(std::vector<Float3>& cells, std::vector<float>& pdf, pcg32_random_t& rng);

};