/*
* Copyright (c) 2024, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

#include "RandomTextureGenerators/ImportanceSampledUnitFloat3RandomTextureGenerator.h"

#include <algorithm>

#include "STBNMath.h"
#include "STBNRandom.h"
#include "RandomTextureGenerators/PDFStats.h"
#include "Utils/SphericalCoordinateMath.h"
#include "Types/Float2.h"
#include "Types/Float3.h"

ImportanceSampledUnitFloat3RandomTextureGenerator::ImportanceSampledUnitFloat3RandomTextureGenerator(ImportanceSamplingData& isData) :
    m_isData(isData)
{

}

PDFStats ImportanceSampledUnitFloat3RandomTextureGenerator::Generate(std::vector<Float3>& cells, std::vector<float>& pdf, pcg32_random_t& rng)
{
    PDFStats stats;
    for (size_t index = 0; index < cells.size(); ++index)
    {
        while (1)
        {
            float uvx = RandomFloat01(rng);
            float uvy = RandomFloat01(rng);

            int isx = Clamp(int(uvx * float(m_isData.isw)), 0, m_isData.isw - 1);
            int isy = Clamp(int(uvy * float(m_isData.ish)), 0, m_isData.ish - 1);

            float isvalue = m_isData.isf[isy * m_isData.isw + isx];
            float randValue = RandomFloat01(rng);
            if (randValue < isvalue)
            {
//#if MAKE_DEBUG_TEXTURES()
//                    Histogram2DInt[isy * isw + isx]++;
//#endif

                cells[index] = SphericalMapUVToDirection(Float2{ uvx, uvy });
                pdf[index] = isvalue / m_isData.isavg;
                stats.max = std::max(pdf[index], stats.max);
                stats.min = std::min(pdf[index], stats.min);
                break;
            }
        }
    }
    return stats;
}
