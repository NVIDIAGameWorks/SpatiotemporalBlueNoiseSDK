#pragma once

#include <array>
#include <memory>

#include "BlueNoiseTexturesND.h"
#include "Kernel/BlueNoiseGaussianKernel.h"
#include "STBNData.h"
#include "VoidAndCluster/VCController.h"

class VoidAndCluster;

enum ScalarImplementation
{
    Reference_2Dx1Dx1D,
    Reference_2Dx2D,
    SliceCache_2Dx1Dx1D,
    SliceCache_2Dx2D
};

class STBNMaker
{
public:
    STBNMaker(Dimensions dims, SigmaPerDimension sigmas, float initialBinaryPatternDensity, ScalarImplementation scalarImplementation);

    void Make();

    BlueNoiseTexturesND GetBlueNoiseTextures() const;

    const VoidAndCluster* GetVoidAndCluster() const;

private:

    size_t m_numPixels;

    BlueNoiseGaussianKernel m_kernelX;
    BlueNoiseGaussianKernel m_kernelY;
    BlueNoiseGaussianKernel m_kernelZ;
    BlueNoiseGaussianKernel m_kernelW;

    STBNData m_data;
    std::unique_ptr<VCController> m_updater;
    std::unique_ptr<VoidAndCluster> m_vc;

    SigmaPerDimension m_sigmas;
    float m_initialBinaryPatternDensity;
};

