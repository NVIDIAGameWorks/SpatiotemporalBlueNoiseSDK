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

#include "VectorSTBNArgs.h"

enum class VECTOR_TYPE
{
    FLOAT,
    FLOAT2,
    FLOAT3
};

enum class FLOAT_GENERATOR_TYPE
{
    UNIFORM,
    UNIT
};

enum class FLOAT_VALUE_DISTANCE_FUNCTION
{
    ABSOLUTE_VALUE
};

enum class FLOAT2_GENERATOR_TYPE
{
    UNIFORM,
    UNIT
};

enum class FLOAT2_VALUE_DISTANCE_FUNCTION
{
    L1,
    L2,
    LInfinity
};

enum class FLOAT3_GENERATOR_TYPE
{
    UNIFORM,
    UNIT,
    UNIT_COSINE_WEIGHTED_HEMISPHERE
};

enum class FLOAT3_VALUE_DISTANCE_FUNCTION
{
    L1,
    L2,
    LInfinity,
    NegativeDot
};

struct VectorSTBNFloatArgs
{
    FLOAT_GENERATOR_TYPE generatorType;
    FLOAT_VALUE_DISTANCE_FUNCTION valueDistanceFunction;
};

struct VectorSTBNFloat2Args
{
    FLOAT2_GENERATOR_TYPE generatorType;
    FLOAT2_VALUE_DISTANCE_FUNCTION valueDistanceFunction;
};

struct VectorSTBNFloat3Args
{
    FLOAT3_GENERATOR_TYPE generatorType;
    FLOAT3_VALUE_DISTANCE_FUNCTION valueDistanceFunction;
};

struct VectorSTBNTypeArgs
{
    VECTOR_TYPE type;
    union
    {
        VectorSTBNFloatArgs floatArgs;
        VectorSTBNFloat2Args float2Args;
        VectorSTBNFloat3Args float3Args;
    };
};

class VectorSTBNImplSelector
{
public:
    VectorSTBNImplSelector(const VectorSTBNTypeArgs& typeArgs, const VectorSTBNArgs& args);

    void Run();
private:
    const VectorSTBNArgs m_args;
    VectorSTBNTypeArgs m_typeArgs;
};
