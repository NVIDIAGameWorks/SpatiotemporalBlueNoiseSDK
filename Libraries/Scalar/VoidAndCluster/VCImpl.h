#pragma once

union PixelCoords;
struct STBNData;
class SymmetricKernel;

class VCImpl
{
public:
    virtual ~VCImpl();

    virtual STBNData& GetSTBNData() = 0;

    virtual size_t GetPixelOnCount() const = 0;

    virtual size_t GetTightestCluster() const = 0;

    virtual size_t GetLargestVoid() const = 0;

    virtual void SplatOnXY(const PixelCoords& pixelCoords, const SymmetricKernel& outerKernel, const SymmetricKernel& innerKernel) = 0;
    virtual void SplatOnZW(const PixelCoords& pixelCoords, const SymmetricKernel& outerKernel, const SymmetricKernel& innerKernel) = 0;
    virtual void SplatOnZ(const PixelCoords& pixelCoords, const SymmetricKernel& kernel) = 0;
    virtual void SplatOnW(const PixelCoords& pixelCoords, const SymmetricKernel& kernel) = 0;

    virtual void SplatOffXY(const PixelCoords& pixelCoords, const SymmetricKernel& outerKernel, const SymmetricKernel& innerKernel) = 0;
    virtual void SplatOffZW(const PixelCoords& pixelCoords, const SymmetricKernel& outerKernel, const SymmetricKernel& innerKernel) = 0;
    virtual void SplatOffZ(const PixelCoords& pixelCoords, const SymmetricKernel& kernel) = 0;
    virtual void SplatOffW(const PixelCoords& pixelCoords, const SymmetricKernel& kernel) = 0;

    virtual void SetPixelOn(size_t pixelIndex, bool value) = 0;

    virtual void SetPixelRank(size_t pixelIndex, size_t rank) = 0;

    virtual void SetAllEnergyToZero() = 0;

    virtual void InvertPixelOn(size_t pixelIndex) = 0;	
};