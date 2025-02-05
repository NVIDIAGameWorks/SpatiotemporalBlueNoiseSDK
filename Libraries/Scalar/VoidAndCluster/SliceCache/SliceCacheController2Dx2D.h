#pragma once

#include "VoidAndCluster/VCController.h"

#include "Kernel/SymmetricKernel.h"
#include "VoidAndCluster/SliceCache/SliceCacheImpl.h"

class SliceCacheController2Dx2D : public VCController
{
public:
    SliceCacheController2Dx2D(STBNData& data, SymmetricKernel kernelX, SymmetricKernel kernelY, SymmetricKernel kernelZ, SymmetricKernel kernelW);

    virtual STBNData& GetSTBNData() override;

    virtual size_t GetPixelOnCount() const override;

    virtual size_t GetTightestCluster() override;

    virtual size_t GetLargestVoid() override;

    virtual void SplatOn(const PixelCoords& pixelCoords) override;

    virtual void SplatOff(const PixelCoords& pixelCoords) override;

    virtual void SetPixelOn(size_t pixelIndex, bool value) override;

    virtual void SetPixelRank(size_t pixelIndex, size_t rank) override;

    virtual void SetAllEnergyToZero() override;

    virtual void InvertPixelOn(size_t pixelIndex) override;

private:
    STBNData& m_data;
    SliceCacheImpl m_impl;

    SymmetricKernel m_kernelX;
    SymmetricKernel m_kernelY;
    SymmetricKernel m_kernelZ;
    SymmetricKernel m_kernelW;
};