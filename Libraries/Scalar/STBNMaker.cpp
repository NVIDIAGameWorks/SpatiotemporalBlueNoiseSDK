#include "STBNMaker.h"

#include <algorithm>
#include <vector>

#include "STBNRandom.h"
#include "VoidAndCluster/VoidAndCluster.h"
#include "VoidAndCluster/Reference/ReferenceController2Dx1Dx1D.h"
#include "VoidAndCluster/Reference/ReferenceController2Dx2D.h"
#include "VoidAndCluster/SliceCache/SliceCacheController2Dx1Dx1D.h"
#include "VoidAndCluster/SliceCache/SliceCacheController2Dx2D.h"

STBNMaker::STBNMaker(Dimensions dims, SigmaPerDimension sigmas, float initialBinaryPatternDensity, ScalarImplementation scalarImplementation) :
    m_numPixels(dims.x * dims.y * dims.z * dims.w),
    m_kernelX(sigmas.x, dims.x),
    m_kernelY(sigmas.y, dims.y),
    m_kernelZ(sigmas.z, dims.z),
    m_kernelW(sigmas.w, dims.w),
    m_data(dims),
    m_sigmas(sigmas),
    m_initialBinaryPatternDensity(initialBinaryPatternDensity)
{
    switch (scalarImplementation)
    {
    case ScalarImplementation::Reference_2Dx1Dx1D:
        m_updater = std::make_unique<ReferenceController2Dx1Dx1D>(m_data, m_kernelX, m_kernelY, m_kernelZ, m_kernelW);
        break;
    case ScalarImplementation::Reference_2Dx2D:
        m_updater = std::make_unique<ReferenceController2Dx2D>(m_data, m_kernelX, m_kernelY, m_kernelZ, m_kernelW);
        break;
    case ScalarImplementation::SliceCache_2Dx1Dx1D:
        m_updater = std::make_unique<SliceCacheController2Dx1Dx1D>(m_data, m_kernelX, m_kernelY, m_kernelZ, m_kernelW);
        break;
    case ScalarImplementation::SliceCache_2Dx2D:
        m_updater = std::make_unique<SliceCacheController2Dx2D>(m_data, m_kernelX, m_kernelY, m_kernelZ, m_kernelW);
        break;
    }

    m_vc = std::make_unique<VoidAndCluster>(initialBinaryPatternDensity, m_updater.get());
}

void STBNMaker::Make()
{
    m_vc->InitializeToWhiteNoise();
    m_vc->ReorganizeToBlueNoise();
    m_vc->Phase1();
    m_vc->Phase2();
    m_vc->Phase3();
}

BlueNoiseTexturesND STBNMaker::GetBlueNoiseTextures() const
{
    BlueNoiseTexturesND textures;
    textures.Init({ static_cast<int>(m_data.dimensions.x), static_cast<int>(m_data.dimensions.y), static_cast<int>(m_data.dimensions.z), 1 }, { m_sigmas.x, m_sigmas.y, m_sigmas.z, 1 }, { 0, 0, 1, 2 });
    auto& pixels = textures.GetPixels();
    for (size_t pixelIndex = 0; pixelIndex < m_numPixels; ++pixelIndex)
    {
        Pixel& pixel = pixels[pixelIndex];
        pixel.energy = 0.0f;
        pixel.on = true;
        pixel.rank = static_cast<int>(m_data.pixelRank[pixelIndex]);
    }

    return textures;
}

const VoidAndCluster* STBNMaker::GetVoidAndCluster() const
{
    return m_vc.get();
}