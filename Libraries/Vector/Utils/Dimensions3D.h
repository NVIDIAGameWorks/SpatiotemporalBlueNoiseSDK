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

union Dimensions3D
{
    struct
    {
        size_t x;
        size_t y;
        size_t z;
    };
    size_t dim[3];
    size_t NumPixels() const;
};

bool operator==(const Dimensions3D& a, const Dimensions3D& b);
