/*
* Copyright (c) 2024, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

#include "ValueDistanceFunctions/Float2ValueDistanceFunctions.h"

#include <cmath>
#include "Types/Float2.h"

float L1(Float2 a, Float2 b)
{
	return std::fabs(a.x - b.x) + std::fabs(a.y - b.y);
}

float L2(Float2 a, Float2 b)
{
	return std::sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

float LInfinity(Float2 a, Float2 b)
{
	return std::max(std::fabs(a.x - b.x), std::fabs(a.y - b.y));
}
