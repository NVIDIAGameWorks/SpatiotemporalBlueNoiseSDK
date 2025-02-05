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

#include <string>
#include <vector>

struct ImportanceSamplingData
{
    float isavg = 1.0f;
    std::vector<float> isf;
    int isw = 0;
    int ish = 0;
    float lummax = 0.0f;
};
