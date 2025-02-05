/*
* Copyright (c) 2024, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

#include "ProgressReporter.h"

#include "Utils/Dimensions3D.h"

#include <chrono>
#include <format>
#include <sstream>
#include <string>

ProgressReporter::ProgressReporter(VectorSTBNProgressData& stbnProgress, SAProgressData& saData, Dimensions3D dims, size_t updateIntervalMs) :
    m_stbnProgress(stbnProgress),
    m_saProgress(saData),
    m_dims(dims),
    m_saStartingEnergy(0.0f),
    m_saEnergyTextWidth(10),
    m_saSwapCountTextWidth(10),
    m_updateIntervalMs(updateIntervalMs),
    m_startedGenerateTexture(false),
    m_finishedGenerateTexture(false),
    m_startedSA(false),
    m_finishedSA(false),
    m_startedFileExport(false),
    m_finishedFileExport(false),
    m_done(false)
{

}

ProgressReporter::~ProgressReporter()
{
    if (m_workerThread)
    {
        m_workerThread->join();
    }
}

void ProgressReporter::LaunchCMDReporter()
{
    m_workerThread = std::make_unique<std::thread>(&ProgressReporter::CmdReporter, this);
}

bool ProgressReporter::IsInProgress()
{
    return !m_done;
}

void LogLine(const std::string& line)
{
    printf("\r%s", line.c_str());
}

void PrintNewline()
{
    printf("\n");
}

std::string DurationToTimeStr(std::chrono::duration<float> duration)
{
    std::stringstream ss;
    using day_t = std::chrono::duration<long, std::ratio<3600 * 24>>;
    auto d = duration_cast<day_t>(duration);
    auto h = duration_cast<std::chrono::hours>(duration -= d);
    auto m = duration_cast<std::chrono::minutes>(duration -= h);
    auto s = duration_cast<std::chrono::seconds>(duration -= m);
    auto ms = duration_cast<std::chrono::milliseconds>(duration -= s);
    if (d.count() > 0)
    {
        ss << d.count() << " day"; if (d.count() > 1) { ss << "s"; } ss << ", ";
    }
    if (h.count() > 0)
    {
        ss << h.count() << " hour"; if (h.count() > 1) { ss << "s"; } ss << ", ";
    }
    if (m.count() > 0)
    {
        ss << m.count() << " minute"; if (m.count() > 1) { ss << "s"; } ss << ", ";
    }

    ss << std::setw(2) << s.count() << ".";
    auto msCount = ms.count();
    if (msCount < 10)
        ss << "00";
    else if (msCount < 100)
        ss << "0";
    ss << std::right << ms.count() << " second"; if (!(s.count() == 1 && ms.count() == 0)) { ss << "s"; }
    else { ss << " "; }

    return ss.str();
}

void ProgressReporter::CmdReporter()
{
    std::stringstream ss;
    ss << "Running SimulatedAnnealing on " << m_dims.x << "x" << m_dims.y << "x" << m_dims.z << "\n";
    LogLine(ss.str());
    while (IsInProgress())
    {
        UpdateCMD();
        std::this_thread::sleep_for(std::chrono::milliseconds(m_updateIntervalMs));
    }
}

void ProgressReporter::UpdateCMD()
{
    if (!m_startedGenerateTexture)
    {
        if (m_stbnProgress.startedGenerateTexture)
        {
            LogLine("Generating starting texture...");
            m_startedGenerateTexture = true;
        }
    }
    if (!m_finishedGenerateTexture)
    {
        if (m_stbnProgress.finishedGenerateTexture)
        {
            LogLine("Generating starting texture...Done.");
            PrintNewline();
            m_finishedGenerateTexture = true;
        }
        return;
    }
    if (!m_startedSA)
    {
        if (m_stbnProgress.startedSA)
        {
            m_startedSA = true;
            InitializeSAState();
        }
        return;
    }
    if (!m_finishedSA)
    {
        UpdateSALine();
        if (m_stbnProgress.finishedSA)
        {
            m_finishedSA = true;
            PrintNewline();
            LogLine("Running simulated annealing...Done");
            PrintNewline();
        }
        return;
    }
    if (!m_startedFileExport)
    {
        if (m_stbnProgress.startedFileExport)
        {
            m_startedFileExport = true;
            LogLine("Exporting files to disk...");
        }
        return;
    }
    if (!m_finishedFileExport)
    {
        if (m_stbnProgress.finishedFileExport)
        {
            m_finishedFileExport = true;
            LogLine("Exporting files to disk...Done.");
            PrintNewline();
        }
        return;
    }
    if (m_finishedFileExport)
    {
        m_done = true;
    }
}

void ProgressReporter::InitializeSAState()
{
    m_saStartingEnergy = m_saProgress.energy;
    m_saEnergyTextWidth = std::max(5, static_cast<int>(std::log10f(m_saStartingEnergy)) + 1);
    m_saSwapCountTextWidth = std::max(3, static_cast<int>(std::log10f(m_saProgress.totalSwaps)));
    m_saStartTime = std::chrono::system_clock::now();
}

void ProgressReporter::UpdateSALine()
{
    std::stringstream ss;
    ss << "Simulated annealing: ";
    ss << "Energy: " << std::setw(m_saEnergyTextWidth) << std::setprecision(3) << std::setfill(' ') << std::fixed << m_saProgress.energy << ", ";
    ss << "Swap index: " << std::setw(m_saSwapCountTextWidth) << m_saProgress.swapIndex << " / " << std::setw(m_saSwapCountTextWidth) << m_saProgress.totalSwaps;
    float percentage = 100.0f * static_cast<float>(m_saProgress.swapIndex) / static_cast<float>(m_saProgress.totalSwaps);
    ss << " (" << std::setw(7) << std::setprecision(3) << std::setfill(' ') << std::fixed << percentage << "%), ";
    auto currentTime = std::chrono::system_clock::now();
    auto deltaTime = currentTime - m_saStartTime;
    ss << "Duration: " << DurationToTimeStr(deltaTime);
    LogLine(ss.str());
}