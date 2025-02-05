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

union Dimensions3D;

union PixelCoords3D
{
    struct
    {
        size_t x;
        size_t y;
        size_t z;
    };
    size_t coords[3];
    inline size_t operator[](int index) const { return coords[index]; }
    inline size_t& operator[](int index) { return coords[index]; }
};

size_t PixelCoords3DToPixelIndex(const PixelCoords3D& pixelCoords, const Dimensions3D& dims);

PixelCoords3D PixelIndexToPixelCoords3D(size_t pixelIndex, const Dimensions3D& dims);
