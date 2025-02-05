#pragma once

union PixelCoords;
class SymmetricKernel;

#include <vector>

#include "Utils/Dimensions.h"
#include "STBNData.h"
#include "VoidAndCluster/VCImpl.h"

struct SliceCacheData2Dx1Dx1D
{
    SliceCacheData2Dx1Dx1D(const Dimensions& dimensions);

    Dimensions dims;
    size_t              sliceSizeXY;
    size_t              numSlicesXY;

    std::vector<bool>   dirtyMax;
    std::vector<float>  maxValue;
    std::vector<size_t> maxValueIndex;

    std::vector<bool>   dirtyMin;
    std::vector<float>  minValue;
    std::vector<size_t> minValueIndex;
};

class SliceCacheImpl : public VCImpl
{
public:
    SliceCacheImpl(STBNData& data);

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
    mutable SliceCacheData2Dx1Dx1D m_cache;
};