#include "VoidAndCluster.h"

#include "STBNRandom.h"
#include "Utils/PixelCoords.h"

VoidAndClusterProgressData::VoidAndClusterProgressData() :
    startedInitializeToWhiteNoise(false),
    initializeToWhiteNoiseCurrentIndex(0),
    initializeToWhiteNoiseTargetCount(UINT32_MAX),
    reorganizeToBlueNoiseFinished(false),
    reorganizeToBlueNoiseIterationsSoFar(0),
    phase1Part1OnesCountRemaining(UINT32_MAX),
    phase1Part1OnesCountTotal(UINT32_MAX),
    phase1Part2PixelIndex(0),
    phase2OnesCountCurrent(0),
    phase2OnesCountTotal(UINT32_MAX),
    phase3Part1PixelCountCurrent(0),
    phase3Part2OnesCountRemaining(UINT32_MAX),
    phase3Part2OnesCountTotal(UINT32_MAX)
{

}

size_t CalcNumPixels(const STBNData& data)
{
    return data.dimensions.x * data.dimensions.y * data.dimensions.z * data.dimensions.w;
}

VoidAndCluster::VoidAndCluster(float initialBinaryPatternDensity, VCController* updater) :
    m_numPixels(CalcNumPixels(updater->GetSTBNData())),
    m_data(updater->GetSTBNData()),
    m_updater(updater),
    m_initialBinaryPatternDensity(initialBinaryPatternDensity)
{

}

size_t VoidAndCluster::GetNumPixels() const
{
    return m_numPixels;
}

void VoidAndCluster::InitializeToWhiteNoise()
{
    // generate an initial set of on pixels, with a max density of m_initialBinaryPatternDensity
    // If we get duplicate random numbers, we won't get the full targetCount of on pixels, but that is ok.
    m_pd.startedInitializeToWhiteNoise = true;
    m_pd.initializeToWhiteNoiseStartTime = std::chrono::steady_clock::now();
    pcg32_random_t rng = GetRNG();
    m_pd.initializeToWhiteNoiseTargetCount = std::max(size_t(float(m_numPixels) * m_initialBinaryPatternDensity), (size_t)2);
    for (m_pd.initializeToWhiteNoiseCurrentIndex = 0; m_pd.initializeToWhiteNoiseCurrentIndex < m_pd.initializeToWhiteNoiseTargetCount; ++m_pd.initializeToWhiteNoiseCurrentIndex)
    {
        size_t pixelIndex = pcg32_boundedrand_r(&rng, uint32_t(m_numPixels) - 1u);
        m_updater->SetPixelOn(pixelIndex, true);
        SplatEnergyOn(pixelIndex);
    }
    m_pd.initializeToWhiteNoiseEndTime = std::chrono::steady_clock::now();
}

void VoidAndCluster::ReorganizeToBlueNoise()
{
    // Make these into blue noise distributed points by removing the point at the tightest
    // cluster and placing it into the largest void. Repeat until those are the same location.
    m_pd.reorganizeToBlueNoiseStartTime = std::chrono::steady_clock::now();
    m_pd.reorganizeToBlueNoiseIterationsSoFar = 0;
    while (1)
    {
        size_t tightestClusterIndex = m_updater->GetTightestCluster();
        m_updater->SetPixelOn(tightestClusterIndex, false);
        SplatEnergyOff(tightestClusterIndex);

        size_t largestVoidIndex = m_updater->GetLargestVoid();
        m_updater->SetPixelOn(largestVoidIndex, true);
        SplatEnergyOn(largestVoidIndex);

        m_pd.reorganizeToBlueNoiseIterationsSoFar++;

        if (tightestClusterIndex == largestVoidIndex)
            break;
    }
    m_pd.reorganizeToBlueNoiseFinished = true;
    m_pd.reorganizeToBlueNoiseEndTime = std::chrono::steady_clock::now();
}

void VoidAndCluster::Phase1Part1()
{
    // Make the initial pattern progressive.
    // Find the tightest cluster and remove it. The rank for that pixel
    // is the number of ones left in the pattern.
    // Go until no more ones are in the texture.
    m_pd.phase1Part1StartTime = std::chrono::steady_clock::now();

    m_pd.phase1Part1OnesCountRemaining = m_updater->GetPixelOnCount();
    m_pd.phase1Part1OnesCountTotal = m_pd.phase1Part1OnesCountRemaining;

    while (m_pd.phase1Part1OnesCountRemaining > 0)
    {
        size_t tightestClusterIndex = m_updater->GetTightestCluster();

        m_pd.phase1Part1OnesCountRemaining--;

        m_updater->SetPixelOn(tightestClusterIndex, false);
        m_updater->SetPixelRank(tightestClusterIndex, m_pd.phase1Part1OnesCountRemaining);

        SplatEnergyOff(tightestClusterIndex);
    }
    m_pd.phase1Part1EndTime = std::chrono::steady_clock::now();
}

void VoidAndCluster::Phase1Part2()
{
    // restore the "on" states
    m_pd.phase1Part2StartTime = std::chrono::steady_clock::now();
    for (m_pd.phase1Part2PixelIndex = 0; m_pd.phase1Part2PixelIndex < m_numPixels; ++m_pd.phase1Part2PixelIndex)
    {
        if (m_data.pixelRank[m_pd.phase1Part2PixelIndex] < m_numPixels)
        {
            m_updater->SetPixelOn(m_pd.phase1Part2PixelIndex, true);
            SplatEnergyOn(m_pd.phase1Part2PixelIndex);
        }
    }

    m_pd.phase1Part2EndTime = std::chrono::steady_clock::now();
}

void VoidAndCluster::Phase1()
{
    Phase1Part1();
    Phase1Part2();
}

void VoidAndCluster::Phase2()
{
    // Add new samples until half are ones.
    // Do this by repeatedly inserting a one into the largest void, and the
    // rank is the number of ones before you added it.
    m_pd.phase2StartTime = std::chrono::steady_clock::now();

    m_pd.phase2OnesCountCurrent = m_updater->GetPixelOnCount();
    m_pd.phase2OnesCountTotal = m_numPixels / 2;

    while (m_pd.phase2OnesCountCurrent < m_pd.phase2OnesCountTotal)
    {
        size_t largestVoidIndex = m_updater->GetLargestVoid();

        m_updater->SetPixelOn(largestVoidIndex, true);
        m_updater->SetPixelRank(largestVoidIndex, m_pd.phase2OnesCountCurrent);

        SplatEnergyOn(largestVoidIndex);

        m_pd.phase2OnesCountCurrent++;
    }

    m_pd.phase2EndTime = std::chrono::steady_clock::now();
}

void VoidAndCluster::Phase3Part1()
{
    // Reverse the meaning of zeros and ones.
// Remove the tightest cluster and give it the rank of the number of zeros in the binary pattern before you removed it.
// Go until there are no more ones
    m_pd.phase3Part1StartTime = std::chrono::steady_clock::now();
    m_updater->SetAllEnergyToZero();
    for (m_pd.phase3Part1PixelCountCurrent = 0; m_pd.phase3Part1PixelCountCurrent < m_numPixels; ++m_pd.phase3Part1PixelCountCurrent)
    {
        m_updater->InvertPixelOn(m_pd.phase3Part1PixelCountCurrent);
        if (m_data.pixelOn[m_pd.phase3Part1PixelCountCurrent])
            SplatEnergyOn(m_pd.phase3Part1PixelCountCurrent);
    }
    m_pd.phase3Part1EndTime = std::chrono::steady_clock::now();
}

void VoidAndCluster::Phase3Part2()
{
    m_pd.phase3Part2StartTime = std::chrono::steady_clock::now();
    m_pd.phase3Part2OnesCountTotal = m_updater->GetPixelOnCount();
    m_pd.phase3Part2OnesCountRemaining = m_pd.phase3Part2OnesCountTotal;

    while (m_pd.phase3Part2OnesCountRemaining > 0)
    {
        size_t tightestClusterIndex = m_updater->GetTightestCluster();

        m_updater->SetPixelOn(tightestClusterIndex, false);
        m_updater->SetPixelRank(tightestClusterIndex, m_numPixels - m_pd.phase3Part2OnesCountRemaining);

        m_pd.phase3Part2OnesCountRemaining--;

        SplatEnergyOff(tightestClusterIndex);
    }

    m_pd.phase3Part2EndTime = std::chrono::steady_clock::now();
}

void VoidAndCluster::Phase3()
{
    Phase3Part1();
    Phase3Part2();
}

const STBNData& VoidAndCluster::GetSTBNData() const
{
    return m_data;
}

const VoidAndClusterProgressData& VoidAndCluster::GetProgressData() const
{
    return m_pd;
}

void VoidAndCluster::SplatEnergyOn(size_t pixelIndex)
{
    const PixelCoords pixelCoords = PixelIndexToPixelCoords(pixelIndex, m_data.dimensions);
    m_updater->SplatOn(pixelCoords);
}

void VoidAndCluster::SplatEnergyOff(size_t pixelIndex)
{
    const PixelCoords pixelCoords = PixelIndexToPixelCoords(pixelIndex, m_data.dimensions);
    m_updater->SplatOff(pixelCoords);
}