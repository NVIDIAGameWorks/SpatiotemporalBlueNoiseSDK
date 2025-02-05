/*
* Copyright (c) 2024, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

#include "Utils/PixelCoords3D.h"

#include "Utils/Dimensions3D.h"

size_t PixelCoords3DToPixelIndex(const PixelCoords3D& pixelCoords, const Dimensions3D& dims)
{
    size_t ret = 0;

    ret += pixelCoords[2];
    ret *= dims.y;
    ret += pixelCoords[1];
    ret *= dims.x;
    ret += pixelCoords[0];

    return ret;
}

PixelCoords3D PixelIndexToPixelCoords3D(size_t pixelIndex, const Dimensions3D& dims)
{
    PixelCoords3D ret;

    ret[0] = (pixelIndex % (dims.x)) / (1);
    ret[1] = (pixelIndex % (dims.x * dims.y)) / (dims.x);
    ret[2] = (pixelIndex % (dims.x * dims.y * dims.z)) / (dims.x * dims.y);

    return ret;
}
