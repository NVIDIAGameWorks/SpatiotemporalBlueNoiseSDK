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

union Float3;

float L1(Float3 a, Float3 b);

float L2(Float3 a, Float3 b);

float LInfinity(Float3 a, Float3 b);

float Dot(Float3 a, Float3 b);

float NegativeDot(Float3 a, Float3 b);
