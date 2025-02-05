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

#include "STBNRandom.h"

union Float3;

Float3 GenerateFloat3Uniform(pcg32_random_t& rng);

Float3 GenerateFloat3Unit(pcg32_random_t& rng);

Float3 GenerateFloat3PositiveHemisphere(pcg32_random_t& rng);