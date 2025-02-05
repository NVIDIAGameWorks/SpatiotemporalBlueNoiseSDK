#include "ReferenceFuncs.h"

#include "STBNData.h"
#include "Kernel/SymmetricKernel.h"
#include "Utils/PixelCoords.h"

namespace ReferenceFuncs
{

size_t GetPixelOnCount(const STBNData& data)
{
    size_t ret = 0;
    for (bool b : data.pixelOn)
    {
        if (b)
            ret++;
    }
    return ret;
}

size_t GetTightestCluster(const STBNData& data)
{
    size_t clusterPixelIndex = 0;
    float maxEnergy = -FLT_MAX;

    for (size_t pixelIndex = 0; pixelIndex < data.numPixels; ++pixelIndex)
    {
        if (!data.pixelOn[pixelIndex])
            continue;

        if (data.energy[pixelIndex] > maxEnergy)
        {
            maxEnergy = data.energy[pixelIndex];
            clusterPixelIndex = pixelIndex;
        }
    }

    return clusterPixelIndex;
}

size_t GetLargestVoid(const STBNData& data)
{
    size_t voidPixelIndex = 0;
    float minEnergy = FLT_MAX;

    for (size_t pixelIndex = 0; pixelIndex < data.numPixels; ++pixelIndex)
    {
        if (data.pixelOn[pixelIndex])
            continue;

        if (data.energy[pixelIndex] < minEnergy)
        {
            minEnergy = data.energy[pixelIndex];
            voidPixelIndex = pixelIndex;
        }
    }

    return voidPixelIndex;
}

template<bool ON>
void Splat1DReference(std::vector<float>& energy, Dimensions dimensions, PixelCoords pixelCoords, size_t dimension, const SymmetricKernel& kernel)
{
    PixelCoords coords = pixelCoords;
    auto dims = dimensions.dim[dimension];
    for (int iz = kernel.start(); iz <= kernel.end(); ++iz)
    {
        int pixelZ = (int(pixelCoords[dimension] + dims) + iz) % int(dims);
        coords[dimension] = static_cast<size_t>(pixelZ);
        size_t pixelIndex = PixelCoordsToPixelIndex(coords, dimensions);

        if (ON)
            energy[pixelIndex] += kernel[size_t(abs(iz))];
        else
            energy[pixelIndex] -= kernel[size_t(abs(iz))];
    }
}

void SplatOn1D(STBNData& data, const PixelCoords& pixelCoords, size_t dimension, const SymmetricKernel& kernel)
{
    Splat1DReference<true>(data.energy, data.dimensions, pixelCoords, dimension, kernel);
}

void SplatOff1D(STBNData& data, const PixelCoords& pixelCoords, size_t dimension, const SymmetricKernel& kernel)
{
    Splat1DReference<false>(data.energy, data.dimensions, pixelCoords, dimension, kernel);
}

template<bool ON>
void Splat2DReference(std::vector<float>& energy, Dimensions dimensions, PixelCoords pixelCoords, size_t outerDimensionIndex, const SymmetricKernel& outerKernel, size_t innerDimensionIndex, const SymmetricKernel& innerKernel)
{
    PixelCoords newPixelCoords = pixelCoords;
    size_t outerDimensionSize = dimensions.dim[outerDimensionIndex];
    size_t innerDimensionSize = dimensions.dim[innerDimensionIndex];
    for (int iy = outerKernel.start(); iy <= outerKernel.end(); ++iy)
    {
        float kernelY = outerKernel[size_t(abs(iy))];
        size_t pixelY = size_t(((int)pixelCoords[outerDimensionIndex] + iy + (int)outerDimensionSize) % (int)outerDimensionSize);
        newPixelCoords[outerDimensionIndex] = pixelY;
        for (int ix = innerKernel.start(); ix <= innerKernel.end(); ++ix)
        {
            float kernelX = innerKernel[size_t(abs(ix))];
            size_t pixelX = size_t(((int)pixelCoords[innerDimensionIndex] + ix + (int)innerDimensionSize) % (int)innerDimensionSize);
            newPixelCoords[innerDimensionIndex] = pixelX;
            size_t pixelIndex = PixelCoordsToPixelIndex(newPixelCoords, dimensions);

            if (ON)
                energy[pixelIndex] += kernelX * kernelY;
            else
                energy[pixelIndex] -= kernelX * kernelY;
        }
    }
}

void SplatOn2D(STBNData& data, const PixelCoords& pixelCoords, size_t outerDimension, const SymmetricKernel& outerKernel, size_t innerDimension, const SymmetricKernel& innerKernel)
{
    Splat2DReference<true>(data.energy, data.dimensions, pixelCoords, outerDimension, outerKernel, innerDimension, innerKernel);
}

void SplatOff2D(STBNData& data, const PixelCoords& pixelCoords, size_t outerDimension, const SymmetricKernel& outerKernel, size_t innerDimension, const SymmetricKernel& innerKernel)
{
    Splat2DReference<false>(data.energy, data.dimensions, pixelCoords, outerDimension, outerKernel, innerDimension, innerKernel);
}

void SetPixelOn(STBNData& data, size_t pixelIndex, bool value)
{
    data.pixelOn[pixelIndex] = value;
}

void SetPixelRank(STBNData& data,size_t pixelIndex, size_t rank)
{
    data.pixelRank[pixelIndex] = rank;
}

void SetAllEnergyToZero(STBNData& data)
{
    std::fill(data.energy.begin(), data.energy.end(), 0.0f);
}

void InvertPixelOn(STBNData& data,size_t pixelIndex)
{
    data.pixelOn[pixelIndex] = !data.pixelOn[pixelIndex];
}

}