#pragma once

union PixelCoords;
struct STBNData;
class SymmetricKernel;

namespace ReferenceFuncs
{

size_t GetPixelOnCount(const STBNData& data);

size_t GetTightestCluster(const STBNData& data);

size_t GetLargestVoid(const STBNData& data);

void SplatOn1D(STBNData& data, const PixelCoords& pixelCoords, size_t dimension, const SymmetricKernel& kernel);
void SplatOff1D(STBNData& data, const PixelCoords& pixelCoords, size_t dimension, const SymmetricKernel& kernel);

void SplatOn2D(STBNData& data, const PixelCoords& pixelCoords, size_t outerDimension, const SymmetricKernel& outerKernel, size_t innerDimension, const SymmetricKernel& innerKernel);
void SplatOff2D(STBNData& data, const PixelCoords& pixelCoords, size_t outerDimension, const SymmetricKernel& outerKernel, size_t innerDimension, const SymmetricKernel& innerKernel);

void SetPixelOn(STBNData& data, size_t pixelIndex, bool value);

void SetPixelRank(STBNData& data, size_t pixelIndex, size_t rank);

void SetAllEnergyToZero(STBNData& data);

void InvertPixelOn(STBNData& data, size_t pixelIndex);

}