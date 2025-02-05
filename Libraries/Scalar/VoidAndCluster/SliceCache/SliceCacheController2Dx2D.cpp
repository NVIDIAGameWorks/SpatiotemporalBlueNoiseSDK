#include "SliceCacheController2Dx2D.h"

#include "Kernel/SymmetricKernel.h"

SliceCacheController2Dx2D::SliceCacheController2Dx2D(STBNData& data, SymmetricKernel kernelX, SymmetricKernel kernelY, SymmetricKernel kernelZ, SymmetricKernel kernelW) :
    m_data(data),
    m_impl(data),
    m_kernelX(kernelX),
    m_kernelY(kernelY),
    m_kernelZ(kernelZ),
    m_kernelW(kernelW)
{

}

STBNData& SliceCacheController2Dx2D::GetSTBNData()
{
    return m_data;
}

size_t SliceCacheController2Dx2D::GetPixelOnCount() const
{
    return m_impl.GetPixelOnCount();
}

size_t SliceCacheController2Dx2D::GetTightestCluster()
{
    return m_impl.GetTightestCluster();
}

size_t SliceCacheController2Dx2D::GetLargestVoid()
{
    return m_impl.GetLargestVoid();
}

void SliceCacheController2Dx2D::SplatOn(const PixelCoords& pixelCoords)
{
    m_impl.SplatOnXY(pixelCoords, m_kernelY, m_kernelX);
    m_impl.SplatOnZW(pixelCoords, m_kernelW, m_kernelZ);
}

void SliceCacheController2Dx2D::SplatOff(const PixelCoords& pixelCoords)
{
    m_impl.SplatOffXY(pixelCoords, m_kernelY, m_kernelX);
    m_impl.SplatOffZW(pixelCoords, m_kernelW, m_kernelZ);
}

void SliceCacheController2Dx2D::SetPixelOn(size_t pixelIndex, bool value)
{
    m_impl.SetPixelOn(pixelIndex, value);
}

void SliceCacheController2Dx2D::SetPixelRank(size_t pixelIndex, size_t rank)
{
    m_impl.SetPixelRank(pixelIndex, rank);
}

void SliceCacheController2Dx2D::SetAllEnergyToZero()
{
    m_impl.SetAllEnergyToZero();
}

void SliceCacheController2Dx2D::InvertPixelOn(size_t pixelIndex)
{
    m_impl.InvertPixelOn(pixelIndex);
}