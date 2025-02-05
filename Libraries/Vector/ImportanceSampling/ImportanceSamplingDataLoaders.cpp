/*
* Copyright (c) 2024, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

#include "ImportanceSampling/ImportanceSamplingDataLoaders.h"

#include "stb_image.h"

#include "STBNMath.h"
#include "ImportanceSampling/ImportanceSamplingData.h"

ImportanceSamplingData LoadISDataFromPNG(const std::string& importanceSamplingMapFilePath, bool isRGBMap)
{
    ImportanceSamplingData isData;

    if (!isRGBMap)
        stbi_hdr_to_ldr_gamma(1.0f);

    int isc;
    float* isfraw = stbi_loadf(importanceSamplingMapFilePath.c_str(), &isData.isw, &isData.ish, &isc, 0);

    if (!isRGBMap)
        stbi_hdr_to_ldr_gamma(2.2f);

    // Convert to greyscale and get the max value
    isData.isf.resize(isData.isw * isData.ish);
    for (int index = 0; index < isData.isw * isData.ish; ++index)
    {
        float lum;

        if (isc == 1)
            lum = isfraw[index * isc];
        else
        {
            lum =
                isfraw[index * isc + 0] * 0.3f +
                isfraw[index * isc + 1] * 0.59f +
                isfraw[index * isc + 2] * 0.11f;
        }
        isData.lummax = std::max(isData.lummax, lum);
        isData.isf[index] = lum;
    }

    // scale to 1.0 max and keep track of isavg
    for (int index = 0; index < isData.isw * isData.ish; ++index)
    {
        isData.isf[index] /= isData.lummax;
        isData.isavg = Lerp(isData.isavg, isData.isf[index], 1.0f / float(index + 1));
    }

    stbi_image_free(isfraw);

    return isData;
}
