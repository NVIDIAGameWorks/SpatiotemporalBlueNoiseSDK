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
#include <memory>
#include <thread>

#include "VectorSTBNProgressData.h"
#include "SimulatedAnnealing/SAProgressData.h"
#include "Utils/Dimensions3D.h"

class ProgressReporter
{
public:
    ProgressReporter(VectorSTBNProgressData& stbnProgress, SAProgressData& saData, Dimensions3D dims, size_t updateIntervalMs);
    ~ProgressReporter();

    void LaunchCMDReporter();

private:
    VectorSTBNProgressData& m_stbnProgress;
    SAProgressData& m_saProgress;
    Dimensions3D m_dims;
    float m_saStartingEnergy;
    size_t m_saEnergyTextWidth;
    size_t m_saSwapCountTextWidth;

    std::unique_ptr<std::thread> m_workerThread;
    size_t m_updateIntervalMs;

    bool m_startedGenerateTexture;
    bool m_finishedGenerateTexture;
    bool m_startedSA;
    bool m_finishedSA;
    bool m_startedFileExport;
    bool m_finishedFileExport;
    bool m_done;

    std::chrono::system_clock::time_point m_saStartTime;

    bool IsInProgress();
    void CmdReporter();
    void UpdateCMD();
    void InitializeSAState();
    void UpdateSALine();
};