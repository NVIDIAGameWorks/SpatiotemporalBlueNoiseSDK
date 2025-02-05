#include "SliceCacheImpl.h"

#include "Kernel/SymmetricKernel.h"
#include "Utils/PixelCoords.h"

SliceCacheData2Dx1Dx1D::SliceCacheData2Dx1Dx1D(const Dimensions& dimensions) :
    dims(dimensions),
    sliceSizeXY(dims.x* dims.y),
    numSlicesXY(dims.z* dims.w),
    dirtyMax(numSlicesXY, true),
    maxValue(numSlicesXY, -FLT_MAX),
    maxValueIndex(numSlicesXY, 0),
    dirtyMin(numSlicesXY, true),
    minValue(numSlicesXY, FLT_MAX),
    minValueIndex(numSlicesXY, 0)
{

}

STBNData& SliceCacheImpl::GetSTBNData()
{
    return m_data;
}

SliceCacheImpl::SliceCacheImpl(STBNData& data) :
    m_data(data),
    m_cache(data.dimensions)
{

}

size_t SliceCacheImpl::GetPixelOnCount() const
{
    size_t ret = 0;
    for (bool b : m_data.pixelOn)
    {
        if (b)
            ret++;
    }
    return ret;
}

size_t SliceCacheImpl::GetTightestCluster() const
{
    size_t clusterPixelIndex = 0;
    float maxEnergy = -FLT_MAX;

    for (size_t sliceXYIndex = 0; sliceXYIndex < m_cache.numSlicesXY; sliceXYIndex++)
    {
        size_t sliceTightestClusterIndex = 0;
        float sliceMaxEnergy = -FLT_MAX;

        // Take the cached value if it's clean
        if (!m_cache.dirtyMax[sliceXYIndex])
        {
            sliceTightestClusterIndex = m_cache.maxValueIndex[sliceXYIndex];
            sliceMaxEnergy = m_cache.maxValue[sliceXYIndex];
        }
        // Otherwise compute it here
        else
        {
            size_t startPixelIndex = sliceXYIndex * m_cache.sliceSizeXY;
            size_t endPixelIndex = startPixelIndex + m_cache.sliceSizeXY;
            for (size_t i = startPixelIndex; i < endPixelIndex; i++)
            {
                if (!m_data.pixelOn[i])
                    continue;

                if (m_data.energy[i] > sliceMaxEnergy)
                {
                    sliceMaxEnergy = m_data.energy[i];
                    sliceTightestClusterIndex = i;
                }
            }
            // Update cache
            m_cache.maxValue[sliceXYIndex] = sliceMaxEnergy;
            m_cache.maxValueIndex[sliceXYIndex] = sliceTightestClusterIndex;
            m_cache.dirtyMax[sliceXYIndex] = false;
        }

        if (sliceMaxEnergy > maxEnergy)
        {
            maxEnergy = sliceMaxEnergy;
            clusterPixelIndex = sliceTightestClusterIndex;
        }
    }

    return clusterPixelIndex;
}

size_t SliceCacheImpl::GetLargestVoid() const
{
    size_t voidPixelIndex = 0;
    float minEnergy = FLT_MAX;

    for (size_t sliceXYIndex = 0; sliceXYIndex < m_cache.numSlicesXY; sliceXYIndex++)
    {
        size_t sliceLargestVoidIndex = 0;
        float sliceMinEnergy = FLT_MAX;

        // Take the cached value if it's clean
        if (!m_cache.dirtyMin[sliceXYIndex])
        {
            sliceLargestVoidIndex = m_cache.minValueIndex[sliceXYIndex];
            sliceMinEnergy = m_cache.minValue[sliceXYIndex];
        }
        // Otherwise compute it here
        else
        {
            size_t startPixelIndex = sliceXYIndex * m_cache.sliceSizeXY;
            size_t endPixelIndex = startPixelIndex + m_cache.sliceSizeXY;
            for (size_t i = startPixelIndex; i < endPixelIndex; i++)
            {
                if (m_data.pixelOn[i])
                    continue;

                if (m_data.energy[i] < sliceMinEnergy)
                {
                    sliceMinEnergy = m_data.energy[i];
                    sliceLargestVoidIndex = i;
                }
            }
            // Update cache
            m_cache.minValue[sliceXYIndex] = sliceMinEnergy;
            m_cache.minValueIndex[sliceXYIndex] = sliceLargestVoidIndex;
            m_cache.dirtyMin[sliceXYIndex] = false;
        }

        if (sliceMinEnergy < minEnergy)
        {
            minEnergy = sliceMinEnergy;
            voidPixelIndex = sliceLargestVoidIndex;
        }
    }

    return voidPixelIndex;
}

inline size_t CalcOffsetPixelCoord(size_t coord, int offset, size_t width)
{
    return (size_t)((((int)coord) + offset + (int)width) % width);
}

inline size_t CoordsToXYSlice(const PixelCoords& coords, const Dimensions& dims)
{
    return (coords.w * dims.z) + coords.z;
}

template<bool ON>
inline void SplatPixel(SliceCacheData2Dx1Dx1D& cache, std::vector<float>& energy, std::vector<bool>& pixelOn, size_t pixelIndex, float splatValue, size_t xySlice)
{
    if (ON)
    {
        // Only dirty min if we're adding to the min
        if (!cache.dirtyMin[xySlice] && !pixelOn[pixelIndex] && (energy[pixelIndex] == cache.minValue[xySlice]))
        {
            cache.dirtyMin[xySlice] = true;
        }
        energy[pixelIndex] += splatValue;// kernel[abs(iz)];
        // Only update max if we're adding to the max
        if (!cache.dirtyMax[xySlice] && pixelOn[pixelIndex] && (energy[pixelIndex] > cache.maxValue[xySlice]))
        {
            cache.maxValue[xySlice] = energy[pixelIndex];
            cache.maxValueIndex[xySlice] = pixelIndex;
        }
    }
    else
    {
        // Only dirty max if we're subtracing from the max
        if (!cache.dirtyMax[xySlice] && pixelOn[pixelIndex] && (energy[pixelIndex] == cache.maxValue[xySlice]))
        {
            cache.dirtyMax[xySlice] = true;
        }
        energy[pixelIndex] -= splatValue;// kernel[abs(iz)];
        // Update min if it's the new min and the cache isn't dirty
        if (!cache.dirtyMin[xySlice] && !pixelOn[pixelIndex] && (energy[pixelIndex] < cache.minValue[xySlice]))
        {
            cache.minValue[xySlice] = energy[pixelIndex];
            cache.minValueIndex[xySlice] = pixelIndex;
        }
    }
}

template<bool ON>
void SplatZ(SliceCacheData2Dx1Dx1D& cache, std::vector<float>& energy, std::vector<bool>& pixelOn, const Dimensions& dimensions, const PixelCoords& pixelCoords, const SymmetricKernel& kernel)
{
    PixelCoords coords = pixelCoords;

    for (int iz = kernel.start(); iz <= kernel.end(); ++iz)
    {
        coords.z = CalcOffsetPixelCoord(pixelCoords.z, iz, dimensions.z);

        size_t pixelIndex = PixelCoordsToPixelIndex(coords, dimensions);

        size_t xySlice = CoordsToXYSlice(coords, dimensions);
        float splatValue = kernel[size_t(abs(iz))];

        SplatPixel<ON>(cache, energy, pixelOn, pixelIndex, splatValue, xySlice);
    }
}

void SliceCacheImpl::SplatOnZ(const PixelCoords& pixelCoords, const SymmetricKernel& kernel)
{
    SplatZ<true>(m_cache, m_data.energy, m_data.pixelOn, m_data.dimensions, pixelCoords, kernel);
}

void SliceCacheImpl::SplatOffZ(const PixelCoords& pixelCoords, const SymmetricKernel& kernel)
{
    SplatZ<false>(m_cache, m_data.energy, m_data.pixelOn, m_data.dimensions, pixelCoords, kernel);
}

template<bool ON>
void SplatW(SliceCacheData2Dx1Dx1D& cache, std::vector<float>& energy, const Dimensions& dimensions, const PixelCoords& pixelCoords, const SymmetricKernel& kernel)
{
    PixelCoords coords = pixelCoords;

    for (int iw = kernel.start(); iw <= kernel.end(); ++iw)
    {
        coords.w = CalcOffsetPixelCoord(pixelCoords.w, iw, dimensions.w);

        size_t pixelIndex = PixelCoordsToPixelIndex(coords, dimensions);

        if (ON)
        {
            energy[pixelIndex] += kernel[size_t(abs(iw))];
        }
        else
        {
            energy[pixelIndex] -= kernel[size_t(abs(iw))];
        }
        // Naive implementation: Always dirty the cache
        // It's slightly faster when W=1 than the smart version the others have.
        auto xySlice = CoordsToXYSlice(coords, dimensions);
        cache.dirtyMax[xySlice] = true;
        cache.dirtyMin[xySlice] = true;
    }
}

void SliceCacheImpl::SplatOnW(const PixelCoords& pixelCoords, const SymmetricKernel& kernel)
{
    SplatW<true>(m_cache, m_data.energy, m_data.dimensions, pixelCoords, kernel);
}

void SliceCacheImpl::SplatOffW(const PixelCoords& pixelCoords, const SymmetricKernel& kernel)
{
    SplatW<false>(m_cache, m_data.energy, m_data.dimensions, pixelCoords, kernel);
}

template<bool ON>
void SplatXY(SliceCacheData2Dx1Dx1D& cache, std::vector<float>& energy, std::vector<bool>& pixelOn, const Dimensions& dimensions, PixelCoords pixelCoords, const SymmetricKernel& outerKernel, const SymmetricKernel& innerKernel)
{
    PixelCoords newCoords = pixelCoords;
    auto xySlice = CoordsToXYSlice(pixelCoords, dimensions);
    for (int iy = outerKernel.start(); iy <= outerKernel.end(); ++iy)
    {
        float kernelY = outerKernel[size_t(abs(iy))];
        newCoords.y = CalcOffsetPixelCoord(pixelCoords.y, iy, dimensions.y);

        for (int ix = innerKernel.start(); ix <= innerKernel.end(); ++ix)
        {
            float kernelX = innerKernel[size_t(abs(ix))];
            newCoords.x = CalcOffsetPixelCoord(pixelCoords.x, ix, dimensions.x);
            size_t pixelIndex = PixelCoordsToPixelIndex(newCoords, dimensions);
            float splatValue = kernelX * kernelY;

            SplatPixel<ON>(cache, energy, pixelOn, pixelIndex, splatValue, xySlice);
        }
    }
}

void SliceCacheImpl::SplatOnXY(const PixelCoords& pixelCoords, const SymmetricKernel& outerKernel, const SymmetricKernel& innerKernel)
{
    SplatXY<true>(m_cache, m_data.energy, m_data.pixelOn, m_data.dimensions, pixelCoords, outerKernel, innerKernel);
}

void SliceCacheImpl::SplatOffXY(const PixelCoords& pixelCoords, const SymmetricKernel& outerKernel, const SymmetricKernel& innerKernel)
{
    SplatXY<false>(m_cache, m_data.energy, m_data.pixelOn, m_data.dimensions, pixelCoords, outerKernel, innerKernel);
}

template<bool ON>
void SplatZW(SliceCacheData2Dx1Dx1D& cache, std::vector<float>& energy, std::vector<bool>& pixelOn, const Dimensions& dimensions, PixelCoords pixelCoords, const SymmetricKernel& outerKernel, const SymmetricKernel& innerKernel)
{
    PixelCoords newCoords = pixelCoords;
    for (int iw = outerKernel.start(); iw <= outerKernel.end(); ++iw)
    {
        float kernelW = outerKernel[size_t(abs(iw))];
        newCoords.w = CalcOffsetPixelCoord(pixelCoords.w, iw, dimensions.w);

        for (int iz = innerKernel.start(); iz <= innerKernel.end(); ++iz)
        {
            float kernelZ = innerKernel[size_t(abs(iz))];
            newCoords.z = CalcOffsetPixelCoord(pixelCoords.z, iz, dimensions.z);
            auto xySlice = CoordsToXYSlice(newCoords, dimensions);
            size_t pixelIndex = PixelCoordsToPixelIndex(newCoords, dimensions);
            float splatValue = kernelW * kernelZ;

            SplatPixel<ON>(cache, energy, pixelOn, pixelIndex, splatValue, xySlice);
        }
    }
}

void SliceCacheImpl::SplatOnZW(const PixelCoords& pixelCoords, const SymmetricKernel& outerKernel, const SymmetricKernel& innerKernel)
{
    SplatZW<true>(m_cache, m_data.energy, m_data.pixelOn, m_data.dimensions, pixelCoords, outerKernel, innerKernel);
}

void SliceCacheImpl::SplatOffZW(const PixelCoords& pixelCoords, const SymmetricKernel& outerKernel, const SymmetricKernel& innerKernel)
{
    SplatZW<false>(m_cache, m_data.energy, m_data.pixelOn, m_data.dimensions, pixelCoords, outerKernel, innerKernel);
}

void SliceCacheImpl::SetPixelOn(size_t pixelIndex, bool value)
{
    m_data.pixelOn[pixelIndex] = value;
    auto xySlice = CoordsToXYSlice(PixelIndexToPixelCoords(pixelIndex, m_data.dimensions), m_data.dimensions);
    m_cache.dirtyMax[xySlice] = true;
    m_cache.dirtyMin[xySlice] = true;
}

void SliceCacheImpl::SetPixelRank(size_t pixelIndex, size_t rank)
{
    m_data.pixelRank[pixelIndex] = rank;
}

void SliceCacheImpl::SetAllEnergyToZero()
{
    std::fill(m_data.energy.begin(), m_data.energy.end(), 0.0f);
    std::fill(m_cache.dirtyMax.begin(), m_cache.dirtyMax.end(), true);
    std::fill(m_cache.dirtyMin.begin(), m_cache.dirtyMin.end(), true);
}

void SliceCacheImpl::InvertPixelOn(size_t pixelIndex)
{
    SetPixelOn(pixelIndex, !m_data.pixelOn[pixelIndex]);
}