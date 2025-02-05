/*
* Copyright (c) 2024, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

#include "VectorSTBNImplSelector.h"

#include "VectorSTBNMaker.h"
#include "ImportanceSampling/ImportanceSamplingDataLoaders.h"
#include "RandomTypeGenerators/FloatRandomGenerators.h"
#include "RandomTypeGenerators/Float2RandomGenerators.h"
#include "RandomTypeGenerators/Float3RandomGenerators.h"
#include "RandomTextureGenerators/CosineWeightedHemisphereFloat3RandomTextureGenerator.h"
#include "RandomTextureGenerators/DefaultRandomTextureGenerator.h"
#include "RandomTextureGenerators/ImportanceSampledUnitFloat3RandomTextureGenerator.h"
#include "RandomTextureGenerators/ImportanceSampledRandomTextureGenerator.h"
#include "Types/Float.h"
#include "Types/Float2.h"
#include "Types/Float3.h"
#include "ValueDistanceFunctions/FloatValueDistanceFunctions.h"
#include "ValueDistanceFunctions/Float2ValueDistanceFunctions.h"
#include "ValueDistanceFunctions/Float3ValueDistanceFunctions.h"


VectorSTBNImplSelector::VectorSTBNImplSelector(const VectorSTBNTypeArgs& typeArgs, const VectorSTBNArgs& args) :
    m_args(args),
    m_typeArgs(typeArgs)
{

}

template<float(*ValueDistanceFunction)(Float, Float), Float(*RandomFloatGenerator)(pcg32_random_t&)>
void MakeFloat(const VectorSTBNFloatArgs& typeArgs, const VectorSTBNArgs& args)
{
    if (args.importanceMapPath == "")
    {
        DefaultRandomTextureGenerator<Float, RandomFloatGenerator> randomFloatTextureGenerator;
        VectorSTBNMaker<Float, ValueDistanceFunction, decltype(randomFloatTextureGenerator)> maker(args, randomFloatTextureGenerator);
        maker.Make();
    }
    else
    {
        ImportanceSamplingData isData = LoadISDataFromPNG(args.importanceMapPath, args.importanceMapIsRGB);
        ImportanceSampledRandomTextureGenerator<Float, RandomFloatGenerator> randomISFloatTextureGenerator(isData);
        VectorSTBNMaker<Float, ValueDistanceFunction, decltype(randomISFloatTextureGenerator)> maker(args, randomISFloatTextureGenerator);
        maker.Make();
    }
}

template<float(*ValueDistanceFunction)(Float, Float)>
void MakeFloat(const VectorSTBNFloatArgs& typeArgs, const VectorSTBNArgs& args)
{
    switch (typeArgs.generatorType)
    {
    case FLOAT_GENERATOR_TYPE::UNIFORM:
        MakeFloat<ValueDistanceFunction, GenerateFloatUniform>(typeArgs, args);
    break;
    case FLOAT_GENERATOR_TYPE::UNIT:
        MakeFloat<ValueDistanceFunction, GenerateFloatUnit>(typeArgs, args);
    break;
    }
}

void MakeFloat(const VectorSTBNFloatArgs& typeArgs, const VectorSTBNArgs& args)
{
    switch (typeArgs.valueDistanceFunction)
    {
    case FLOAT_VALUE_DISTANCE_FUNCTION::ABSOLUTE_VALUE:
        MakeFloat<&AbsDistance>(typeArgs, args);
        break;
    }
}

template<float(*ValueDistanceFunction)(Float2, Float2), Float2(*RandomFloat2Generator)(pcg32_random_t&)>
void MakeFloat2(const VectorSTBNFloat2Args& typeArgs, const VectorSTBNArgs& args)
{
    if (args.importanceMapPath == "")
    {
        DefaultRandomTextureGenerator<Float2, RandomFloat2Generator> randomFloat2TextureGenerator;
        VectorSTBNMaker<Float2, ValueDistanceFunction, decltype(randomFloat2TextureGenerator)> maker(args, randomFloat2TextureGenerator);
        maker.Make();
    }
    else
    {
        ImportanceSamplingData isData = LoadISDataFromPNG(args.importanceMapPath, args.importanceMapIsRGB);
        ImportanceSampledRandomTextureGenerator<Float2, RandomFloat2Generator> randomISFloat2TextureGenerator(isData);
        VectorSTBNMaker<Float2, ValueDistanceFunction, decltype(randomISFloat2TextureGenerator)> maker(args, randomISFloat2TextureGenerator);
        maker.Make();
    }
}

template<float(*ValueDistanceFunction)(Float2, Float2)>
void MakeFloat2(const VectorSTBNFloat2Args& typeArgs, const VectorSTBNArgs& args)
{
    switch (typeArgs.generatorType)
    {
    case FLOAT2_GENERATOR_TYPE::UNIFORM:
        MakeFloat2<ValueDistanceFunction, GenerateFloat2Uniform>(typeArgs, args);
        break;
    case FLOAT2_GENERATOR_TYPE::UNIT:
        MakeFloat2<ValueDistanceFunction, GenerateFloat2Unit>(typeArgs, args);
        break;
    }
}

void MakeFloat2(const VectorSTBNFloat2Args& typeArgs, const VectorSTBNArgs& args)
{
    switch (typeArgs.valueDistanceFunction)
    {
    case FLOAT2_VALUE_DISTANCE_FUNCTION::L1:
        MakeFloat2<&L1>(typeArgs, args);
        break;
    case FLOAT2_VALUE_DISTANCE_FUNCTION::L2:
        MakeFloat2<&L2>(typeArgs, args);
        break;
    case FLOAT2_VALUE_DISTANCE_FUNCTION::LInfinity:
        MakeFloat2<&LInfinity>(typeArgs, args);
        break;
    }
}

template<float(*ValueDistanceFunction)(Float3, Float3), Float3(*RandomFloat3Generator)(pcg32_random_t&)>
void MakeFloat3(const VectorSTBNFloat3Args& typeArgs, const VectorSTBNArgs& args)
{
    if (args.importanceMapPath == "")
    {
        DefaultRandomTextureGenerator<Float3, RandomFloat3Generator> randomFloat3TextureGenerator;
        VectorSTBNMaker<Float3, ValueDistanceFunction, decltype(randomFloat3TextureGenerator)> maker(args, randomFloat3TextureGenerator);
        maker.Make();
    }
    else
    {
        ImportanceSamplingData isData = LoadISDataFromPNG(args.importanceMapPath, args.importanceMapIsRGB);
        ImportanceSampledRandomTextureGenerator<Float3, RandomFloat3Generator> randomISFloat3TextureGenerator(isData);
        VectorSTBNMaker<Float3, ValueDistanceFunction, decltype(randomISFloat3TextureGenerator)> maker(args, randomISFloat3TextureGenerator);
        maker.Make();
    }
}

template<float(*ValueDistanceFunction)(Float3, Float3)>
void MakeFloat3Unit(const VectorSTBNFloat3Args& typeArgs, const VectorSTBNArgs& args)
{
    if (args.importanceMapPath == "")
    {
        DefaultRandomTextureGenerator<Float3, GenerateFloatUnit> randomFloat3TextureGenerator;
        VectorSTBNMaker<Float3, ValueDistanceFunction, decltype(randomFloat3TextureGenerator)> maker(args, randomFloat3TextureGenerator);
        maker.Make();
    }
    else
    {
        ImportanceSamplingData isData = LoadISDataFromPNG(args.importanceMapPath, args.importanceMapIsRGB);
        ImportanceSampledUnitFloat3RandomTextureGenerator randomISUnitFloat3TextureGenerator(isData);
        VectorSTBNMaker<Float3, ValueDistanceFunction, ImportanceSampledUnitFloat3RandomTextureGenerator> maker(args, randomISUnitFloat3TextureGenerator);
        maker.Make();
    }
}

template<float(*ValueDistanceFunction)(Float3, Float3)>
void MakeFloat3UnitWeightedCosineHemisphere(const VectorSTBNFloat3Args& typeArgs, const VectorSTBNArgs& args)
{
    CosineWeightedHemisphereFloat3RandomTextureGenerator randomISCosineWeightedHemisphereFloat3TextureGenerator;
    VectorSTBNMaker<Float3, ValueDistanceFunction, CosineWeightedHemisphereFloat3RandomTextureGenerator> maker(args, randomISCosineWeightedHemisphereFloat3TextureGenerator);
    maker.Make();
}

template<float(*ValueDistanceFunction)(Float3, Float3)>
void MakeFloat3(const VectorSTBNFloat3Args& typeArgs, const VectorSTBNArgs& args)
{
    switch (typeArgs.generatorType)
    {
    case FLOAT3_GENERATOR_TYPE::UNIFORM:
        MakeFloat3<ValueDistanceFunction, GenerateFloat3Uniform>(typeArgs, args);
        break;
    case FLOAT3_GENERATOR_TYPE::UNIT:
        MakeFloat3<ValueDistanceFunction, GenerateFloat3Unit>(typeArgs, args);
        break;
    case FLOAT3_GENERATOR_TYPE::UNIT_COSINE_WEIGHTED_HEMISPHERE:
        MakeFloat3UnitWeightedCosineHemisphere<ValueDistanceFunction>(typeArgs, args);
    }
}

void MakeFloat3(const VectorSTBNFloat3Args& typeArgs, const VectorSTBNArgs& args)
{
    switch (typeArgs.valueDistanceFunction)
    {
    case FLOAT3_VALUE_DISTANCE_FUNCTION::L1:
        MakeFloat3<&L1>(typeArgs, args);
        break;
    case FLOAT3_VALUE_DISTANCE_FUNCTION::L2:
        MakeFloat3<&L2>(typeArgs, args);
        break;
    case FLOAT3_VALUE_DISTANCE_FUNCTION::LInfinity:
        MakeFloat3<&LInfinity>(typeArgs, args);
        break;
    case FLOAT3_VALUE_DISTANCE_FUNCTION::NegativeDot:
        MakeFloat3<&NegativeDot>(typeArgs, args);
        break;
    }
}

void VectorSTBNImplSelector::Run()
{
    switch (m_typeArgs.type)
    {
    case VECTOR_TYPE::FLOAT:
        MakeFloat(m_typeArgs.floatArgs, m_args);
        break;
    case VECTOR_TYPE::FLOAT2:
        MakeFloat2(m_typeArgs.float2Args, m_args);
        break;
    case VECTOR_TYPE::FLOAT3:
        MakeFloat3(m_typeArgs.float3Args, m_args);
        break;
    }
}
