/*
* Copyright (c) 2019-2024, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

#include <filesystem>
#include <iostream>
#include <memory>

#include "cxxopts.hpp"

#include "STBNMaker.h"
#include "Kernel/BlueNoiseGaussianKernel.h"
#include "Reporting/ProgressReporter.h"
#include "VoidAndCluster/VoidAndCluster.h"

struct ProgramOptions
{
    std::filesystem::path outputDirectory;
    Dimensions dims;
    SigmaPerDimension sigmas;
    float initialBinaryPatternDensity;
    ScalarImplementation implementation;
};

cxxopts::Options BuildCmdOptions()
{
    cxxopts::Options cmdOptions("Spatiotemporal Blue Noise", "Generates spatiotemporal blue noise images");

    cmdOptions.add_options()
        ("o,output", "Output directory", cxxopts::value<std::string>()->default_value("output"))
        ("dimsX", "X texture dimension", cxxopts::value<int>()->default_value("128"))
        ("dimsY", "Y texture dimension", cxxopts::value<int>()->default_value("128"))
        ("dimsZ", "Z texture dimension", cxxopts::value<int>()->default_value("64"))
        ("dimsW", "W texture dimension", cxxopts::value<int>()->default_value("1"))
        ("sX", "Sigma X", cxxopts::value<float>()->default_value("1.9"))
        ("sY", "Sigma Y", cxxopts::value<float>()->default_value("1.9"))
        ("sZ", "Sigma Z", cxxopts::value<float>()->default_value("1.9"))
        ("sW", "Sigma W", cxxopts::value<float>()->default_value("1.9"))
        ("ibpd", "Initial binary pattern density", cxxopts::value<float>()->default_value("0.1"))
        ("i,implementation", "sc211 for slice cache 2Dx1Dx1D noise, sc22 for slice cache 2Dx2D noise, r211 for reference 2Dx1Dx1D, r22 reference 2Dx2D noise", cxxopts::value<std::string>()->default_value("sc211"))
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

ScalarImplementation ParseSplatBasis(const std::string& input)
{
    if (input == "r211")
        return ScalarImplementation::Reference_2Dx1Dx1D;
    if (input == "r22")
        return ScalarImplementation::Reference_2Dx2D;
    if (input == "sc211")
        return ScalarImplementation::SliceCache_2Dx1Dx1D;
    if (input == "sc22")
        return ScalarImplementation::SliceCache_2Dx2D;
    printf("Unrecognized splatBasis flag. Options are r22, r22, sc211, or sc22\n.");
    exit(-1);
}

ProgramOptions BuildProgramOptionsFromParsedArgs(cxxopts::ParseResult& parsedOptions)
{
    ProgramOptions programOptions;

    programOptions.outputDirectory = parsedOptions["output"].as<std::string>().c_str();
    programOptions.dims.x = parsedOptions["dimsX"].as<int>();
    programOptions.dims.y = parsedOptions["dimsY"].as<int>();
    programOptions.dims.z = parsedOptions["dimsZ"].as<int>();
    programOptions.dims.w = parsedOptions["dimsW"].as<int>();
    programOptions.sigmas.x = parsedOptions["sX"].as<float>();
    programOptions.sigmas.y = parsedOptions["sY"].as<float>();
    programOptions.sigmas.z = parsedOptions["sZ"].as<float>();
    programOptions.sigmas.w = parsedOptions["sW"].as<float>();
    programOptions.initialBinaryPatternDensity = parsedOptions["ibpd"].as<float>();
    programOptions.implementation = ParseSplatBasis(parsedOptions["implementation"].as<std::string>());

    return programOptions;
}

ProgramOptions ParseCmdArgs(int argc, char** argv)
{
    cxxopts::Options cmdOptions = BuildCmdOptions();
    auto parsedOptions = cmdOptions.parse(argc, argv);
    HandleHelpOption(cmdOptions, parsedOptions);
    return BuildProgramOptionsFromParsedArgs(parsedOptions);
}

std::vector<int> splatBasisToGroups(ScalarImplementation implementation)
{
    switch (implementation)
    {
        case ScalarImplementation::Reference_2Dx1Dx1D:
        case ScalarImplementation::SliceCache_2Dx1Dx1D:
            return std::vector<int>{ 0, 0, 1, 2 };
            break;
        case ScalarImplementation::Reference_2Dx2D:
            return std::vector<int>{0, 0, 1, 1};
            break;
    }
}

BlueNoiseTexturesND GetBlueNoiseTextures(STBNData& data, SigmaPerDimension sigmas, ScalarImplementation implementation)
{
    BlueNoiseTexturesND textures;
    auto groups = splatBasisToGroups(implementation);
    textures.Init({ static_cast<int>(data.dimensions.x), static_cast<int>(data.dimensions.y), static_cast<int>(data.dimensions.z), static_cast<int>(data.dimensions.w) }, { sigmas.x, sigmas.y, sigmas.z, sigmas.w }, groups);
    auto& pixels = textures.GetPixels();
    for (size_t pixelIndex = 0; pixelIndex < data.numPixels; ++pixelIndex)
    {
        Pixel& pixel = pixels[pixelIndex];
        pixel.energy = 0.0f;
        pixel.on = true;
        pixel.rank = static_cast<int>(data.pixelRank[pixelIndex]);
    }

    return textures;
}

std::string SplatBasisToString(ScalarImplementation implementation)
{
    switch (implementation)
    {
    case ScalarImplementation::Reference_2Dx1Dx1D:
        return "Reference_2Dx1Dx1D";
        break;
    case ScalarImplementation::Reference_2Dx2D:
        return "Reference_2Dx2D";
        break;
    case ScalarImplementation::SliceCache_2Dx1Dx1D:
        return "Splice_Cache_2Dx1Dx1D";
        break;
    case ScalarImplementation::SliceCache_2Dx2D:
        return "Splice_Cache_2Dx2D";
        break;
    }
}

void MakeMask(const ProgramOptions& programOptions)
{
    STBNMaker maker(programOptions.dims, programOptions.sigmas, programOptions.initialBinaryPatternDensity, programOptions.implementation);
    ProgressReporter pr(maker.GetVoidAndCluster(), 20);
    pr.LaunchCMDReporter();

    maker.Make();

    BlueNoiseTexturesND textures = maker.GetBlueNoiseTextures();

    std::filesystem::create_directory(programOptions.outputDirectory);

    // save it out as pngs
    std::string outputFileNamePrefix = "stbn_scalar_" + SplatBasisToString(programOptions.implementation) + "_" + std::to_string(programOptions.dims.x) + "x" + std::to_string(programOptions.dims.y) + "x" + std::to_string(programOptions.dims.z);
    std::string outputFileNameTemplate = outputFileNamePrefix + "_%i.png";
    std::string outputPath = programOptions.outputDirectory.string() + "/" + outputFileNameTemplate;
    SaveTextures(textures, outputPath.c_str());
}

int main(int argc, char** argv)
{
    ProgramOptions programOptions = ParseCmdArgs(argc, argv);

    MakeMask(programOptions);

    return 0;
}
