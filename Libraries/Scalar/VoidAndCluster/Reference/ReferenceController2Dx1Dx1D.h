#pragma once

#include "VoidAndCluster/VCController.h"
#include "Kernel/SymmetricKernel.h"

class ReferenceController2Dx1Dx1D : public VCController
{
public:
    ReferenceController2Dx1Dx1D(STBNData& data, SymmetricKernel kernelX, SymmetricKernel kernelY, SymmetricKernel kernelZ, SymmetricKernel kernelW);

    virtual ~ReferenceController2Dx1Dx1D();

    virtual STBNData& GetSTBNData() override final;

    virtual size_t GetPixelOnCount() const override final;

    virtual size_t GetTightestCluster() override final;

    virtual size_t GetLargestVoid() override final;

    virtual void SplatOn(const PixelCoords& pixelCoords) override final;

    virtual void SplatOff(const PixelCoords& pixelCoords) override final;

    virtual void SetPixelOn(size_t pixelIndex, bool value) override final;

    virtual void SetPixelRank(size_t pixelIndex, size_t rank) override final;

    virtual void SetAllEnergyToZero() override final;

    virtual void InvertPixelOn(size_t pixelIndex) override final;

private:
    STBNData& m_data;

    SymmetricKernel m_kernelX;
    SymmetricKernel m_kernelY;
    SymmetricKernel m_kernelZ;
    SymmetricKernel m_kernelW;
};