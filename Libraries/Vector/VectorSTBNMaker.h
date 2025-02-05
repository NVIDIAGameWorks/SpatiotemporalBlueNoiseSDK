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

#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "STBNRandom.h"
#include "VectorSTBNArgs.h"
#include "Debug/DebugFileExport.h"
#include "Exporters/RGBAExporters.h"
#include "Kernel/VectorBlueNoiseGaussianKernel.h"
#include "ImportanceSampling/ImportanceSamplingData.h"
#include "RandomTextureGenerators/PDFStats.h"
#include "Reporting/ProgressReporter.h"
#include "SimulatedAnnealing/SimulatedAnnealing.h"
#include "Utils/Dimensions3D.h"
#include "VectorSTBNProgressData.h"

std::string GenerateBaseOutputFileName(const VectorSTBNArgs& args)
{
    std::stringstream ss;
    ss << args.baseOutputFilePath;
    ss << "/";
    ss << "VectorSTBN_" << args.dimensions.x << "x" << args.dimensions.y << "x" << args.dimensions.z;
    return ss.str();
}

template<typename T, float(*ValueDistance)(T, T), typename RandomTextureGenerator>
class VectorSTBNMaker
{
public:
    VectorSTBNMaker(const VectorSTBNArgs& args, RandomTextureGenerator& randomTextureGenerator) :
        m_dims(args.dimensions),
        m_baseOutputFilePath(args.baseOutputFilePath),
        m_baseOutputFileName(GenerateBaseOutputFileName(args)),
        m_randomTextureGenerator(randomTextureGenerator),
        m_rng(GetRNG()),
        m_kernelX(VectorBlueNoiseGaussianKernel(args.energySigma, m_dims.x)),
        m_kernelY(VectorBlueNoiseGaussianKernel(args.energySigma, m_dims.y)),
        m_kernelZ(VectorBlueNoiseGaussianKernel(args.energySigma, m_dims.z)),
        m_saData(m_dims),
        m_saDataController(m_saData, m_kernelX, m_kernelY, m_kernelZ, args.valueSigma),
        m_sa(m_dims, m_saDataController, args.swapCountFactor, args.coolingFactor),
        m_reporter(m_progress, m_sa.GetProgressData(), m_dims, 10)
    {
    }

    void Make()
    {
        m_reporter.LaunchCMDReporter();

        m_rng = GetRNG();

        GenerateStartingTexture();

        RunSimulatedAnnealing();

        WriteOutFinalFiles();
    }

    const SAData<T>& GetData() const
    {
        return m_saData;
    }

private:

    Dimensions3D m_dims;
    std::string m_baseOutputFilePath;
    std::string m_baseOutputFileName;

    RandomTextureGenerator& m_randomTextureGenerator;

    PDFStats m_pdfStats;

    pcg32_random_t m_rng;

    SymmetricKernel m_kernelX;
    SymmetricKernel m_kernelY;
    SymmetricKernel m_kernelZ;

    SAData<T> m_saData;
    SADataController<T, ValueDistance> m_saDataController;
    SimulatedAnnealing<T, ValueDistance> m_sa;

    VectorSTBNProgressData m_progress;

    ProgressReporter m_reporter;

    void GenerateStartingTexture()
    {
        m_progress.startedGenerateTexture = true;
        m_pdfStats = m_randomTextureGenerator.Generate(m_saData.cells, m_saData.pdf, m_rng);
        m_progress.finishedGenerateTexture = true;
    }

    void RunSimulatedAnnealing()
    {
        m_progress.startedSA = true;
        m_sa.Run(m_rng);
        m_progress.finishedSA = true;
    }

    void WriteOutFinalFiles()
    {
        m_progress.startedFileExport = true;
        auto output = ConvertCellsAndPDFToRGBA32Texture(m_saData.cells, m_saData.pdf, m_pdfStats.min, m_pdfStats.max);
        WriteRGBA32CellsToPNG(m_baseOutputFileName, output, m_dims);
        m_progress.finishedFileExport = true;
    }
};
