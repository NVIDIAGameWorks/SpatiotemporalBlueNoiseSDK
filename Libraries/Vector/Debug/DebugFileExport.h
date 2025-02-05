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

#include <string>
#include <vector>
union Dimensions3D;
struct ImportanceSamplingData;

void WriteOutPdfMinMaxFile(const std::string& baseOutputFileName, const Dimensions3D& dims, float pdfMin, float pdfMax, float isAvg, float lumax);

void WriteOutUVAndDirsFile(const ImportanceSamplingData& isData, const std::string baseOutputFileName, const Dimensions3D& dims);

void WriteOutEnergyRecord(const std::vector<float>& energyRecord, const std::string& baseFileName, const Dimensions3D& dims);
