#pragma once

#include "VoidAndCluster/VCImpl.h"

class ReferenceImpl2Dx1Dx1D : public VCImpl
{
public:
	ReferenceImpl2Dx1Dx1D(STBNData& data);

    virtual ~ReferenceImpl2Dx1Dx1D() override;

    virtual STBNData& GetSTBNData() override;

    virtual size_t GetPixelOnCount() const override;

    virtual size_t GetTightestCluster() const override;

    virtual size_t GetLargestVoid() const override;

    virtual void SplatOnXY(const PixelCoords& pixelCoords, const SymmetricKernel& outerKernel, const SymmetricKernel& innerKernel) override;
    virtual void SplatOnZW(const PixelCoords& pixelCoords, const SymmetricKernel& outerKernel, const SymmetricKernel& innerKernel) override;
    virtual void SplatOnZ(const PixelCoords& pixelCoords, const SymmetricKernel& kernel) override;
    virtual void SplatOnW(const PixelCoords& pixelCoords, const SymmetricKernel& kernel) override;

    virtual void SplatOffXY(const PixelCoords& pixelCoords, const SymmetricKernel& outerKernel, const SymmetricKernel& innerKernel) override;
    virtual void SplatOffZW(const PixelCoords& pixelCoords, const SymmetricKernel& outerKernel, const SymmetricKernel& innerKernel) override;
    virtual void SplatOffZ(const PixelCoords& pixelCoords, const SymmetricKernel& kernel) override;
    virtual void SplatOffW(const PixelCoords& pixelCoords, const SymmetricKernel& kernel) override;

    virtual void SetPixelOn(size_t pixelIndex, bool value) override;

    virtual void SetPixelRank(size_t pixelIndex, size_t rank) override;

    virtual void SetAllEnergyToZero() override;

    virtual void InvertPixelOn(size_t pixelIndex) override;

private:
	STBNData& m_data;
};