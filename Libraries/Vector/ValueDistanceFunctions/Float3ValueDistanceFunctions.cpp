/*
* Copyright (c) 2024, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

#include "ValueDistanceFunctions/Float3ValueDistanceFunctions.h"

#include <cmath>

#include "Types/Float3.h"

float L1(Float3 a, Float3 b)
{
	return std::fabs(a.x - b.x) + std::fabs(a.y - b.y) + std::fabs(a.z - b.z);
}

float L2(Float3 a, Float3 b)
{
	return std::sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
}

float LInfinity(Float3 a, Float3 b)
{
	return std::max(std::fabs(a.x - b.x), std::max(std::fabs(a.y - b.y), std::fabs(a.z - b.z)));
}

float Dot(Float3 a, Float3 b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

float NegativeDot(Float3 a, Float3 b)
{
    return -(a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}