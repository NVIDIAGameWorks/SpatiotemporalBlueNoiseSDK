#pragma once

union PixelCoords;

#include "STBNData.h"

class VCController
{
public:
    virtual ~VCController();

    virtual STBNData& GetSTBNData() = 0;

    virtual size_t GetPixelOnCount() const = 0;

    virtual size_t GetTightestCluster() = 0;

    virtual size_t GetLargestVoid() = 0;

    virtual void SplatOn(const PixelCoords& pixelCoords) = 0;

    virtual void SplatOff(const PixelCoords& pixelCoords) = 0;

    virtual void SetPixelOn(size_t pixelIndex, bool value) = 0;

    virtual void SetPixelRank(size_t pixelIndex, size_t rank) = 0;

    virtual void SetAllEnergyToZero() = 0;

    virtual void InvertPixelOn(size_t pixelIndex) = 0;
};