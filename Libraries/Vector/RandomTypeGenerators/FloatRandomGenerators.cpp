/*
* Copyright (c) 2024, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

#include "RandomTypeGenerators/FloatRandomGenerators.h"

#include <cmath>
#include "STBNRandom.h"
#include "Types/Float.h"

Float GenerateFloatUniform(pcg32_random_t& rng)
{
    Float f;
    f.x = RandomFloat01(rng) * 2.0f - 1.0f;
    return f;
}

Float GenerateFloatUnit(pcg32_random_t& rng)
{
    Float f;
    f.x = (RandomFloat01(rng) <= 0.5f) ? -1.0f : 1.0f;
    return f;
}
