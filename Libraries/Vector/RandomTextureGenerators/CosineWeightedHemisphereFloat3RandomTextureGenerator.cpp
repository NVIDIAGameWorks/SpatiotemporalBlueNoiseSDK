/*
* Copyright (c) 2024, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

#include "RandomTextureGenerators/CosineWeightedHemisphereFloat3RandomTextureGenerator.h"

#include <algorithm>

#include "Types/Float3.h"
#include "RandomTypeGenerators/Float3RandomGenerators.h"
#include "RandomTextureGenerators/PDFStats.h"
#include "ValueDistanceFunctions/Float3ValueDistanceFunctions.h"

PDFStats CosineWeightedHemisphereFloat3RandomTextureGenerator::Generate(std::vector<Float3>& cells, std::vector<float>& pdf, pcg32_random_t& rng)
{
    PDFStats stats;
    for (size_t index = 0; index < cells.size(); ++index)
    {
        cells[index] = GenerateFloat3PositiveHemisphere(rng);
        pdf[index] = Dot(cells[index], Float3{ 0.0f, 0.0f, 1.0f });
        stats.max = std::max(pdf[index], stats.max);
        stats.min = std::min(pdf[index], stats.min);
    }
    return stats;
}