/*
* Copyright (c) 2024, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

#include "CMDOptionsParser.h"

#include <iostream>

#include "cxxopts.hpp"

#include "VectorProgramOptions.h"

cxxopts::Options BuildCmdOptions()
{
    cxxopts::Options cmdOptions("Spatiotemporal Blue Noise", "Generates spatiotemporal blue noise images");

    cmdOptions.add_options()
        ("o,output", "Output directory", cxxopts::value<std::string>()->default_value("out"))
        ("x,dimsX", "Width in pixels to Generate.", cxxopts::value<int>()->default_value("128"))
        ("y,dimsY", "Height in pixels to Generate.", cxxopts::value<int>()->default_value("128"))
        ("z,dimsZ", "Depth in xy slices to Generate.", cxxopts::value<int>()->default_value("64"))
        ("energySigma", "Energy sigma", cxxopts::value<float>()->default_value("1.9"))
        ("valueSigma", "Value sigma", cxxopts::value<float>()->default_value("1.0"))
        ("swapCountFactor", "Swap count factor", cxxopts::value<float>()->default_value("0.001"))
        ("coolingFactor", "Cooling factor", cxxopts::value<float>()->default_value("0.00001"))
        ("t,type", "Type of mask to Make. Options are Float, Float2, or Float3", cxxopts::value<std::string>()->default_value("Float2"))
        ("g,generator", "Generator to use for initial random values. Options are Uniform or Unit. CosineWeightdHemisphere is available for Float3", cxxopts::value<std::string>()->default_value("Uniform"))
        ("d,valueDistance", "ValueDistance function to use in simulated annealing for Float2 and Float3. Options are L1, L2, or LInfinity. Float3 may also specify NegativeDot as well. Float always uses absolute value. ", cxxopts::value<std::string>()->default_value("L1"))
        ("h,help", "Print help")
        ;
    cmdOptions.allow_unrecognised_options();

    return cmdOptions;
}

void HandleHelpOption(const cxxopts::Options& cmdOptions, const cxxopts::ParseResult& parsedOptions)
{
    if (parsedOptions.count("help"))
    {
        std::cout << cmdOptions.help() << std::endl;
        exit(0);
    }
}

static std::string FloatStr = "Float";
static std::string Float2Str = "Float2";
static std::string Float3Str = "Float3";

static std::string UniformStr = "Uniform";
static std::string UnitStr = "Unit";

static std::string AbsStr = "Abs";
static std::string L1Str = "L1";
static std::string L2Str = "L2";
static std::string LInfinityStr = "LInfinity";
static std::string NegativeDotStr = "NegativeDot";

FLOAT_GENERATOR_TYPE FloatGeneratorStrToEnum(const std::string& generatorStr)
{
    if (generatorStr == UniformStr)
        return FLOAT_GENERATOR_TYPE::UNIFORM;
    else if (generatorStr == UnitStr)
        return FLOAT_GENERATOR_TYPE::UNIT;
    else
    {
        std::cout << "Error: Unrecognized generator type \'" << generatorStr << "\'. Exiting." << std::endl;
        exit(-1);
    }
}

FLOAT_VALUE_DISTANCE_FUNCTION FloatValueDistanceFunctionStrToEnum(const std::string& valueDistanceStr)
{
    if (valueDistanceStr == AbsStr)
        return FLOAT_VALUE_DISTANCE_FUNCTION::ABSOLUTE_VALUE;
    else
    {
        std::cout << "Error: Unrecognized float value distance function type \'" << valueDistanceStr << "\'. Exiting." << std::endl;
        exit(-1);
    }
}

FLOAT2_GENERATOR_TYPE Float2GeneratorStrToEnum(const std::string& generatorStr)
{
    if (generatorStr == UniformStr)
        return FLOAT2_GENERATOR_TYPE::UNIFORM;
    else if (generatorStr == UnitStr)
        return FLOAT2_GENERATOR_TYPE::UNIT;
    else
    {
        std::cout << "Error: Unrecognized generator type \'" << generatorStr << "\'. Exiting." << std::endl;
        exit(-1);
    }
}

FLOAT2_VALUE_DISTANCE_FUNCTION Float2ValueDistanceFunctionStrToEnum(const std::string& valueDistanceStr)
{
    if (valueDistanceStr == L1Str)
        return FLOAT2_VALUE_DISTANCE_FUNCTION::L1;
    else if (valueDistanceStr == L2Str)
        return FLOAT2_VALUE_DISTANCE_FUNCTION::L2;
    else if (valueDistanceStr == LInfinityStr)
        return FLOAT2_VALUE_DISTANCE_FUNCTION::LInfinity;
    else
    {
        std::cout << "Error: Unrecognized Float2 value distance function type \'" << valueDistanceStr << "\'. Exiting." << std::endl;
        exit(-1);
    }
}

FLOAT3_GENERATOR_TYPE Float3GeneratorStrToEnum(const std::string& generatorStr)
{
    if (generatorStr == UniformStr)
        return FLOAT3_GENERATOR_TYPE::UNIFORM;
    else if (generatorStr == UnitStr)
        return FLOAT3_GENERATOR_TYPE::UNIT;
    else
    {
        std::cout << "Error: Unrecognized generator type \'" << generatorStr << "\'. Exiting." << std::endl;
        exit(-1);
    }
}

FLOAT3_VALUE_DISTANCE_FUNCTION Float3ValueDistanceFunctionStrToEnum(const std::string& valueDistanceStr)
{
    if (valueDistanceStr == L1Str)
        return FLOAT3_VALUE_DISTANCE_FUNCTION::L1;
    else if (valueDistanceStr == L2Str)
        return FLOAT3_VALUE_DISTANCE_FUNCTION::L2;
    else if (valueDistanceStr == LInfinityStr)
        return FLOAT3_VALUE_DISTANCE_FUNCTION::LInfinity;
    else if (valueDistanceStr == NegativeDotStr)
        return FLOAT3_VALUE_DISTANCE_FUNCTION::NegativeDot;
    else
    {
        std::cout << "Error: Unrecognized Float3 value distance function type \'" << valueDistanceStr << "\'. Exiting." << std::endl;
        exit(-1);
    }
}

VectorSTBNTypeArgs ParsedOptionsToMakeType(cxxopts::ParseResult& parsedOptions)
{
    VectorSTBNTypeArgs typeArgs;
    auto typeStr = parsedOptions["type"].as<std::string>();
    auto generatorStr = parsedOptions["generator"].as<std::string>();
    auto valueDistanceStr = parsedOptions["valueDistance"].as<std::string>();

    if (typeStr == FloatStr)
    {
        typeArgs.type = VECTOR_TYPE::FLOAT;
        typeArgs.floatArgs.generatorType = FloatGeneratorStrToEnum(generatorStr);
        typeArgs.floatArgs.valueDistanceFunction = FloatValueDistanceFunctionStrToEnum(valueDistanceStr);
    }
    else if (typeStr == Float2Str)
    {
        typeArgs.type = VECTOR_TYPE::FLOAT2;
        typeArgs.float2Args.generatorType = Float2GeneratorStrToEnum(generatorStr);
        typeArgs.float2Args.valueDistanceFunction = Float2ValueDistanceFunctionStrToEnum(valueDistanceStr);
    }
    else if (typeStr == Float3Str)
    {
        typeArgs.type = VECTOR_TYPE::FLOAT3;
        typeArgs.float3Args.generatorType = Float3GeneratorStrToEnum(generatorStr);
        typeArgs.float3Args.valueDistanceFunction = Float3ValueDistanceFunctionStrToEnum(valueDistanceStr);
    }
    else
    {
        std::cout << "Error: Unrecognized type option \'" << typeStr << "\'. Exiting." << std::endl;
        exit(-1);
    }
    return typeArgs;
}

VectorProgramOptions BuildProgramOptionsFromParsedArgs(cxxopts::ParseResult& parsedOptions)
{
    VectorProgramOptions programOptions;

    // New args
    programOptions.args.dimensions.x = static_cast<size_t>(parsedOptions["dimsX"].as<int>());
    programOptions.args.dimensions.y = static_cast<size_t>(parsedOptions["dimsY"].as<int>());
    programOptions.args.dimensions.z = static_cast<size_t>(parsedOptions["dimsZ"].as<int>());
    programOptions.args.energySigma = parsedOptions["energySigma"].as<float>();
    programOptions.args.valueSigma = parsedOptions["valueSigma"].as<float>();
    programOptions.args.swapCountFactor = parsedOptions["swapCountFactor"].as<float>();
    programOptions.args.coolingFactor = parsedOptions["coolingFactor"].as<float>();
    programOptions.args.baseOutputFilePath = parsedOptions["output"].as<std::string>();
    programOptions.makeType = ParsedOptionsToMakeType(parsedOptions);

    return programOptions;
}

VectorProgramOptions ParseCmdArgs(int argc, char** argv)
{
    cxxopts::Options cmdOptions = BuildCmdOptions();
    auto parsedOptions = cmdOptions.parse(argc, argv);
    HandleHelpOption(cmdOptions, parsedOptions);
    return BuildProgramOptionsFromParsedArgs(parsedOptions);
}
