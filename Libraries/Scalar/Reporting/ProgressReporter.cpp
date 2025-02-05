#include "ProgressReporter.h"

#include "VoidAndCluster/VoidAndCluster.h"
#include "Utils/Dimensions.h"

#include <chrono>
#include <format>
#include <sstream>
#include <string>

size_t calcTotalLabelWidth(const Dimensions& dims)
{
    return (size_t)(1.0 + std::log10(dims.x * dims.y * dims.z * dims.w));
}

ProgressReporter::ProgressReporter(const VoidAndCluster* vc, size_t updateIntervalMs) :
    m_vc(vc),
    m_updateIntervalMs(updateIntervalMs),
    m_totalLabelWidth(calcTotalLabelWidth(vc->GetSTBNData().dimensions)),
    m_startedInitializeToWhiteNoise(false),
    m_finishedInitializeToWhiteNoise(false),
    m_finishedReorganizeToBlueNoise(false),
    m_finishedPhase1Part1(false),
    m_finishedPhase1Part2(false),
    m_finishedPhase2(false),
    m_finishedPhase3Part1(false),
    m_finishedPhase3Part2(false),
    m_doneAnalyzing(false),
    m_donePrinting(false)
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

bool ProgressReporter::IsDone() const
{
    return m_donePrinting;
}

bool ProgressReporter::IsInProgress()
{
    return !m_doneAnalyzing;
}

void LogLine(const std::string& line)
{
    printf("\r%s", line.c_str());
}

void PrintNewline()
{
    printf("\n");
}

static const std::string s_labels[] =
{
    "Initialize to white noise",
    "Reorder to blue noise",
    "Phase 1 Part 1",
    "Phase 1 Part 2",
    "Phase 2",
    "Phase 3 Part 1",
    "Phase 3 Part 2"
};

static size_t NumLabels()
{
    return sizeof(s_labels) / (sizeof(*s_labels));
}

static size_t LabelWidth()
{
    static size_t width = 0;
    auto numLabels = sizeof(s_labels) / (sizeof(*s_labels));
    for (size_t i = 0; i < numLabels; i++)
    {
        width = std::max(width, s_labels[i].size());
    }
    return width;
}

std::string PercentToStr(float percent)
{
    std::stringstream ss;
    ss << std::setw(7) << std::fixed << std::setprecision(3) << percent << "%";
    return ss.str();
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

void LogProgressLine(int step, float percentage, size_t count, size_t total, size_t totalWidth, std::chrono::duration<float> duration, bool hasTotal = true)
{
    std::stringstream ss;
    ss << "[" << (step+1) << "/" << NumLabels() << "][";
    if (hasTotal)
    {
        ss << PercentToStr(percentage);
    }
    else
    {
        ss << "---.---%";
    }
    ss << "][" << std::left << std::setw(LabelWidth()) << s_labels[step] << "][" << std::right << std::setw(totalWidth) << count << " / ";
    if (hasTotal)
    {
        ss << std::setw(totalWidth) << total;
    }
    else
    {
        std::string noTotal(totalWidth, '-');
        ss << noTotal;
    }
    ss << "][";
    ss << DurationToTimeStr(duration) << "]";
    LogLine(ss.str());
}

void LogSummaryLine(std::chrono::duration<float> duration)
{
    std::stringstream ss;
    ss << "[Total time taken: " << std::right << DurationToTimeStr(duration) << "]\n";
    LogLine(ss.str());
}

void ProgressReporter::CmdReporter()
{
    auto dims = m_vc->GetSTBNData().dimensions;
    std::stringstream ss;
    ss << "Running VoidAndCluster on " << dims.x << "x" << dims.y << "x" << dims.z << "x" << dims.w << "\n";
    LogLine(ss.str());
    while (IsInProgress())
    {
        UpdateCMD();
        std::this_thread::sleep_for(std::chrono::milliseconds(m_updateIntervalMs));
    }
    LogSummaryLine(m_vc->GetProgressData().phase3Part2EndTime - m_vc->GetProgressData().initializeToWhiteNoiseStartTime);
    m_donePrinting = true;
}

void ProgressReporter::UpdateCMD()
{
    if (!m_startedInitializeToWhiteNoise)
    {
        if (m_vc->GetProgressData().startedInitializeToWhiteNoise)
        {
            m_initializeToWhiteNoiseStartTime = std::chrono::system_clock::now();
            m_startedInitializeToWhiteNoise = true;
        }
    }
    if (!m_finishedInitializeToWhiteNoise)
    {
        UpdateInitializeToWhiteNoise();
        if (m_vc->GetProgressData().initializeToWhiteNoiseCurrentIndex == m_vc->GetProgressData().initializeToWhiteNoiseTargetCount)
        {
            m_initializeToWhiteNoiseEndTime = std::chrono::system_clock::now();
            PrintNewline();
            m_finishedInitializeToWhiteNoise = true;
        }
        return;
    }
    if (!m_finishedReorganizeToBlueNoise)
    {
        UpdateReorganizeToBlueNoise();
        if (m_vc->GetProgressData().reorganizeToBlueNoiseFinished)
        {
            FinalizeReorganizeToBlueNoise();
            PrintNewline();
            m_finishedReorganizeToBlueNoise = true;
        }
        return;
    }
    if (!m_finishedPhase1Part1)
    {
        UpdatePhase1Part1();
        if (m_vc->GetProgressData().phase1Part1OnesCountRemaining == 0)
        {
            PrintNewline();
            m_finishedPhase1Part1 = true;
        }
        return;
    }
    if (!m_finishedPhase1Part2)
    {
        UpdatePhase1Part2();
        if (m_vc->GetProgressData().phase1Part2PixelIndex == m_vc->GetNumPixels())
        {
            PrintNewline();
            m_finishedPhase1Part2 = true;
        }
        return;
    }
    if (!m_finishedPhase2)
    {
        UpdatePhase2();
        if (m_vc->GetProgressData().phase2OnesCountCurrent == m_vc->GetProgressData().phase2OnesCountTotal)
        {
            PrintNewline();
            m_finishedPhase2 = true;
        }
        return;
    }
    if (!m_finishedPhase3Part1)
    {
        UpdatePhase3Part1();
        if (m_vc->GetProgressData().phase3Part1PixelCountCurrent == m_vc->GetNumPixels())
        {
            PrintNewline();
            m_finishedPhase3Part1 = true;
        }
        return;
    }
    if (!m_finishedPhase3Part2)
    {
        UpdatePhase3Part2();
        if (m_vc->GetProgressData().phase3Part2OnesCountRemaining == 0)
        {
            PrintNewline();
            m_finishedPhase3Part2 = true;
        }
        return;
    }
    if (m_finishedPhase3Part2)
    {
        m_doneAnalyzing = true;
    }
}

void ProgressReporter::UpdateInitializeToWhiteNoise()
{
    auto current = m_vc->GetProgressData().initializeToWhiteNoiseCurrentIndex;
    auto target = m_vc->GetProgressData().initializeToWhiteNoiseTargetCount;
    float percentage = 100.0f * (static_cast<float>(current) / static_cast<float>(target));

    auto duration = std::chrono::steady_clock::now() - m_vc->GetProgressData().initializeToWhiteNoiseStartTime;
    LogProgressLine(0, percentage, current, target, m_totalLabelWidth, duration);
}

void ProgressReporter::UpdateReorganizeToBlueNoise()
{
    auto totalSoFar = m_vc->GetProgressData().reorganizeToBlueNoiseIterationsSoFar;

    auto duration = std::chrono::steady_clock::now() - m_vc->GetProgressData().reorganizeToBlueNoiseStartTime;
    LogProgressLine(1, 0.0f, totalSoFar, totalSoFar, m_totalLabelWidth, duration, false);
}

void ProgressReporter::FinalizeReorganizeToBlueNoise()
{
    auto totalSoFar = m_vc->GetProgressData().reorganizeToBlueNoiseIterationsSoFar;

    auto duration = std::chrono::steady_clock::now() - m_vc->GetProgressData().reorganizeToBlueNoiseStartTime;
    LogProgressLine(1, 100.0f, totalSoFar, totalSoFar, m_totalLabelWidth, duration);
}

void ProgressReporter::UpdatePhase1Part1()
{
    auto remaining = m_vc->GetProgressData().phase1Part1OnesCountRemaining;
    auto total = m_vc->GetProgressData().phase1Part1OnesCountTotal;
    auto completed = total - remaining;
    float percentage = 100.0f * (static_cast<float>(completed) / static_cast<float>(total));

    auto duration = std::chrono::steady_clock::now() - m_vc->GetProgressData().phase1Part1StartTime;
    LogProgressLine(2, percentage, completed, total, m_totalLabelWidth, duration);
}

void ProgressReporter::UpdatePhase1Part2()
{
    auto current = m_vc->GetProgressData().phase1Part2PixelIndex;
    auto total = m_vc->GetNumPixels();
    float percentage = 100.0f * (static_cast<float>(current) / static_cast<float>(total));

    auto duration = std::chrono::steady_clock::now() - m_vc->GetProgressData().phase1Part2StartTime;
    LogProgressLine(3, percentage, current, total, m_totalLabelWidth, duration);
}

void ProgressReporter::UpdatePhase2()
{
    auto current = m_vc->GetProgressData().phase2OnesCountCurrent;
    auto total = m_vc->GetProgressData().phase2OnesCountTotal;
    float percentage = 100.0f * (static_cast<float>(current) / static_cast<float>(total));

    auto duration = std::chrono::steady_clock::now() - m_vc->GetProgressData().phase2StartTime;
    LogProgressLine(4, percentage, current, total, m_totalLabelWidth, duration);
}

void ProgressReporter::UpdatePhase3Part1()
{
    auto current = m_vc->GetProgressData().phase3Part1PixelCountCurrent;
    auto total = m_vc->GetNumPixels();
    float percentage = 100.0f * (static_cast<float>(current) / static_cast<float>(total));

    auto duration = std::chrono::steady_clock::now() - m_vc->GetProgressData().phase3Part1StartTime;
    LogProgressLine(5, percentage, current, total, m_totalLabelWidth, duration);
}

void ProgressReporter::UpdatePhase3Part2()
{
    auto remaining = m_vc->GetProgressData().phase3Part2OnesCountRemaining;
    auto total = m_vc->GetProgressData().phase3Part2OnesCountTotal;
    auto completed = total - remaining;
    float percentage = 100.0f * (static_cast<float>(completed) / static_cast<float>(total));

    auto duration = std::chrono::steady_clock::now() - m_vc->GetProgressData().phase3Part2StartTime;
    LogProgressLine(6, percentage, completed, total, m_totalLabelWidth, duration);
}