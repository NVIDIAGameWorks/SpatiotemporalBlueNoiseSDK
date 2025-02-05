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

union Float
{
    float x;
    static const size_t N = 1;
    float data[N];
    float operator[](int i) const { return data[i]; }
    float& operator[](int i) { return data[i]; }
};

inline bool operator==(Float a, Float b) { return a.x == b.x; }
