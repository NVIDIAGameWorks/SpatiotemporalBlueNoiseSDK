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

#include "Utils/Dimensions3D.h"

struct VectorSTBNArgs
{
    Dimensions3D dimensions = { 0, 0, 0 };
    float coolingFactor = 0.0f;
    float swapCountFactor = 0.0f;
    float energySigma = 0.0f;
    float valueSigma = 0.0f;
    std::string baseOutputFilePath;
    std::string baseOutputFileName;
    std::string importanceMapPath;
    bool importanceMapIsRGB = false;
};
