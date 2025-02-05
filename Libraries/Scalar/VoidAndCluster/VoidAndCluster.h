#pragma once

#include <chrono>

#include "VCController.h"

struct VoidAndClusterProgressData
{
    VoidAndClusterProgressData();
    bool startedInitializeToWhiteNoise;
    std::chrono::steady_clock::time_point initializeToWhiteNoiseStartTime;
    std::chrono::steady_clock::time_point initializeToWhiteNoiseEndTime;
    size_t initializeToWhiteNoiseCurrentIndex;
    size_t initializeToWhiteNoiseTargetCount;

    std::chrono::steady_clock::time_point reorganizeToBlueNoiseStartTime;
    std::chrono::steady_clock::time_point reorganizeToBlueNoiseEndTime;
    size_t reorganizeToBlueNoiseIterationsSoFar;
    bool   reorganizeToBlueNoiseFinished;

    std::chrono::steady_clock::time_point phase1Part1StartTime;
    std::chrono::steady_clock::time_point phase1Part1EndTime;
    size_t phase1Part1OnesCountRemaining;
    size_t phase1Part1OnesCountTotal;

    std::chrono::steady_clock::time_point phase1Part2StartTime;
    std::chrono::steady_clock::time_point phase1Part2EndTime;
    size_t phase1Part2PixelIndex;

    std::chrono::steady_clock::time_point phase2StartTime;
    std::chrono::steady_clock::time_point phase2EndTime;
    size_t phase2OnesCountCurrent;
    size_t phase2OnesCountTotal;

    std::chrono::steady_clock::time_point phase3Part1StartTime;
    std::chrono::steady_clock::time_point phase3Part1EndTime;
    size_t phase3Part1PixelCountCurrent;
    size_t phase3Part2OnesCountRemaining;

    std::chrono::steady_clock::time_point phase3Part2StartTime;
    std::chrono::steady_clock::time_point phase3Part2EndTime;
    size_t phase3Part2OnesCountTotal;
};

class VoidAndCluster
{
public:
    VoidAndCluster(float initialBinaryPatternDensity, VCController* updater);
    VoidAndCluster(const VoidAndCluster&) = delete;
    VoidAndCluster& operator=(const VoidAndCluster&) = delete;

    size_t GetNumPixels() const;
    void InitializeToWhiteNoise();
    void ReorganizeToBlueNoise();
    void Phase1();
    void Phase2();
    void Phase3();

    const STBNData& GetSTBNData() const;
    const VoidAndClusterProgressData& GetProgressData() const;

private:
    size_t m_numPixels;
    STBNData& m_data;
    VCController* m_updater;

    float m_initialBinaryPatternDensity;

    VoidAndClusterProgressData m_pd;

    inline void SplatEnergyOn(size_t pixelIndex);
    inline void SplatEnergyOff(size_t pixelIndex);

    void Phase1Part1();
    void Phase1Part2();
    void Phase3Part1();
    void Phase3Part2();
};