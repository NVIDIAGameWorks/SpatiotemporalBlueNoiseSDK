/*
* Copyright (c) 2024, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

#include "Debug/DebugFileExport.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "STBNMath.h"

#include "ImportanceSampling/ImportanceSamplingData.h"
#include "Types/Float2.h"
#include "Types/Float3.h"
#include "Utils/Dimensions3D.h"
#include "Utils/SphericalCoordinateMath.h"

#include "stb_image_write.h"

void WriteOutPdfMinMaxFile(const std::string& baseOutputFileName, const Dimensions3D& dims, float pdfMin, float pdfMax, float isAvg, float lumMax)
{
	std::stringstream ss;
	ss << baseOutputFileName << "_" << dims.x << "x" << dims.y << "x" << dims.z;
    std::string fileName = ss.str();

    std::ofstream file;
    file.open(fileName);
    file << pdfMin << std::endl;
    file << pdfMax << std::endl;
    file << isAvg << std::endl;
    file << lumMax << std::endl;
    file.close();
}

void WriteOutUVAndDirsFile(const ImportanceSamplingData& isData, const std::string baseOutputFileName, const Dimensions3D& dims)
{
    std::vector<unsigned char> output_uv(isData.isw * isData.ish * 4, 0);
    std::vector<unsigned char> output_dir(isData.isw * isData.ish * 4, 0);
    for (size_t index = 0; index < isData.isw * isData.ish; ++index)
    {
        int ix = int(index % isData.isw);
        int iy = int(index / isData.isw);

        float uvx = (float(ix) + 0.5f) / float(isData.isw);
        float uvy = (float(iy) + 0.5f) / float(isData.ish);

        output_uv[index * 4 + 0] = (unsigned char)Clamp(uvx * 256.0f, 0.0f, 255.0f);
        output_uv[index * 4 + 1] = (unsigned char)Clamp(uvy * 256.0f, 0.0f, 255.0f);
        output_uv[index * 4 + 2] = 0;
        output_uv[index * 4 + 3] = 255;

        // TODO
        Float3 dir = SphericalMapUVToDirection(Float2{ uvx, uvy });
        output_dir[index * 4 + 0] = (unsigned char)Clamp(dir[0] * 256.0f, 0.0f, 255.0f);
        output_dir[index * 4 + 1] = (unsigned char)Clamp(dir[1] * 256.0f, 0.0f, 255.0f);
        output_dir[index * 4 + 2] = (unsigned char)Clamp(dir[2] * 256.0f, 0.0f, 255.0f);
        output_dir[index * 4 + 3] = 255;
    }

    std::stringstream uvSS;
    uvSS << baseOutputFileName << "_debuguv_" << dims.x << "x" << dims.y << "x" << dims.z << ".png";
    stbi_write_png(uvSS.str().c_str(), isData.isw, isData.ish, 4, output_uv.data(), 0);

    std::stringstream dirSS;
    dirSS << baseOutputFileName << "_debugdir_" << dims.x << "x" << dims.y << "x" << dims.z << ".png";
    stbi_write_png(dirSS.str().c_str(), isData.isw, isData.ish, 4, output_dir.data(), 0);
}

void WriteOutEnergyRecord(const std::vector<float>& energyRecord, const std::string& baseFileName, const Dimensions3D& dims)
{
    std::stringstream ss;
    ss << baseFileName << "_" << dims.x << "x" << dims.y << "x" << dims.z << ".energy.csv";
    std::string fileName = ss.str();

    // make CSV of energy records
    std::ofstream file;
    file.open(fileName);
    for (float f : energyRecord)
        file << "\"" << f << "\"\n";
    file.close();
}
