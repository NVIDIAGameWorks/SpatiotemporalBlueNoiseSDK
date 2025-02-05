#pragma once

#include <chrono>
#include <memory>
#include <thread>

class VoidAndCluster;

class ProgressReporter
{
public:
    ProgressReporter(const VoidAndCluster* vc, size_t updateIntervalMs);
    ~ProgressReporter();

    void LaunchCMDReporter();
    bool IsDone() const;

private:
    const VoidAndCluster* m_vc;
    std::unique_ptr<std::thread> m_workerThread;
    size_t m_updateIntervalMs;
    size_t m_totalLabelWidth;

    bool m_startedInitializeToWhiteNoise;
    bool m_finishedInitializeToWhiteNoise;
    bool m_finishedReorganizeToBlueNoise;
    bool m_finishedPhase1Part1;
    bool m_finishedPhase1Part2;
    bool m_finishedPhase2;
    bool m_finishedPhase3Part1;
    bool m_finishedPhase3Part2;
    bool m_doneAnalyzing;
    bool m_donePrinting;

    std::chrono::system_clock::time_point m_initializeToWhiteNoiseStartTime;
    std::chrono::system_clock::time_point m_initializeToWhiteNoiseEndTime;
    double m_reorganizeToBlueNoiseStartTime;
    double m_phase1Part1StartTime;
    double m_phase1Part2StartTime;
    double m_phase2StartTime;
    double m_phase3Part1StartTime;
    double m_phase3Part2StartTime;

    bool IsInProgress();
    void CmdReporter();
    void UpdateCMD();

    void UpdateInitializeToWhiteNoise();
    void UpdateReorganizeToBlueNoise();
    void FinalizeReorganizeToBlueNoise();
    void UpdatePhase1Part1();
    void UpdatePhase1Part2();
    void UpdatePhase2();
    void UpdatePhase3Part1();
    void UpdatePhase3Part2();

};