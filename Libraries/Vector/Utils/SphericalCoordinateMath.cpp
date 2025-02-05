/*
* Copyright (c) 2024, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

#include "Utils/SphericalCoordinateMath.h"

#include <cmath>

#include "Types/Float2.h"
#include "Types/Float3.h"

Float3 SphericalMapUVToDirection(Float2 uv)
{
    uv.x -= 0.5f;
    uv.y -= 0.5f;
    uv.x /= 0.1591f;
    uv.y /= 0.3183f;

    float yaw = uv[0];
    float pitch = uv[1];

    Float3 ret;
    ret[1] = -(sin(pitch));
    ret[0] = cos(yaw) * cos(pitch);
    ret[2] = sin(yaw) * cos(pitch);

    return ret;
}

Float2 SphericalMapDirectionToUV(Float3 direction)
{
    float pitch = asin(-direction[1]);
    float yaw = atan2(direction[2], direction[0]);

    Float2 uv = Float2{ yaw, pitch };
    uv.x *= 0.1591f;
    uv.y *= 0.3183f;
    uv.x += 0.5f;
    uv.y += 0.5f;

    return uv;
}
