/*
* Copyright (c) 2024, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

#include "RandomTypeGenerators/Float3RandomGenerators.h"

#include <cmath>
#include "STBNMath.h"
#include "STBNRandom.h"
#include "Types/Float3.h"

Float3 GenerateFloat3Uniform(pcg32_random_t& rng)
{
    Float3 f;
    f.x = RandomFloat01(rng) * 2.0f - 1.0f;
    f.y = RandomFloat01(rng) * 2.0f - 1.0f;
    f.z = RandomFloat01(rng) * 2.0f - 1.0f;
    return f;
}

Float3 GenerateFloat3Unit(pcg32_random_t& rng)
{
    Float3 f;
    f.z = RandomFloat01(rng) * 2.0f - 1.0f;
    float t = RandomFloat01(rng) * C_TWOPI;
    float r = (float)sqrtf(1.0f - f.z * f.z);
    f.x = r * (float)cos(t);
    f.y = r * (float)sin(t);
    return f;
}

Float3 GenerateFloat3PositiveHemisphere(pcg32_random_t& rng)
{
    Float3 f;

    float theta = RandomFloat01(rng) * 2.0f * C_PI;
    float radius = std::sqrtf(RandomFloat01(rng));

    f.x = cos(theta) * radius;
    f.y = sin(theta) * radius;
    f.z = std::sqrtf(1.0f - f.x * f.x - f.y * f.y);

    return f;
}