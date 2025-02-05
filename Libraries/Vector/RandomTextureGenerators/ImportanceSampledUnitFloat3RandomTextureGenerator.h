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
#include <cmath>
#include <vector>

#include "pcg_basic.h"

#include "ImportanceSampling/ImportanceSamplingData.h"
#include "Types/Float3.h"

struct PDFStats;
union Float2;

class ImportanceSampledUnitFloat3RandomTextureGenerator
{
public:
    ImportanceSampledUnitFloat3RandomTextureGenerator(ImportanceSamplingData& isData);

    PDFStats Generate(std::vector<Float3>& cells, std::vector<float>& pdf, pcg32_random_t& rng);

private:
    ImportanceSamplingData m_isData;
};