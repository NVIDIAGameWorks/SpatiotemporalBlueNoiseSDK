#include "SliceCacheController2Dx1Dx1D.h"

#include "Kernel/SymmetricKernel.h"

SliceCacheController2Dx1Dx1D::SliceCacheController2Dx1Dx1D(STBNData& data, SymmetricKernel kernelX, SymmetricKernel kernelY, SymmetricKernel kernelZ, SymmetricKernel kernelW) :
    m_data(data),
    m_impl(data),
    m_kernelX(kernelX),
    m_kernelY(kernelY),
    m_kernelZ(kernelZ),
    m_kernelW(kernelW)
{

}

STBNData& SliceCacheController2Dx1Dx1D::GetSTBNData()
{
    return m_data;
}

size_t SliceCacheController2Dx1Dx1D::GetPixelOnCount() const
{
    return m_impl.GetPixelOnCount();
}

size_t SliceCacheController2Dx1Dx1D::GetTightestCluster()
{
    return m_impl.GetTightestCluster();
}

size_t SliceCacheController2Dx1Dx1D::GetLargestVoid()
{
    return m_impl.GetLargestVoid();
}

void SliceCacheController2Dx1Dx1D::SplatOn(const PixelCoords& pixelCoords)
{
    m_impl.SplatOnXY(pixelCoords, m_kernelY, m_kernelX);
    m_impl.SplatOnZ(pixelCoords, m_kernelZ);
    m_impl.SplatOnW(pixelCoords, m_kernelW);
}

void SliceCacheController2Dx1Dx1D::SplatOff(const PixelCoords& pixelCoords)
{
    m_impl.SplatOffXY(pixelCoords, m_kernelY, m_kernelX);
    m_impl.SplatOffZ(pixelCoords, m_kernelZ);
    m_impl.SplatOffW(pixelCoords, m_kernelW);
}

void SliceCacheController2Dx1Dx1D::SetPixelOn(size_t pixelIndex, bool value)
{
    m_impl.SetPixelOn(pixelIndex, value);
}

void SliceCacheController2Dx1Dx1D::SetPixelRank(size_t pixelIndex, size_t rank)
{
    m_impl.SetPixelRank(pixelIndex, rank);
}

void SliceCacheController2Dx1Dx1D::SetAllEnergyToZero()
{
    m_impl.SetAllEnergyToZero();
}

void SliceCacheController2Dx1Dx1D::InvertPixelOn(size_t pixelIndex)
{
    m_impl.InvertPixelOn(pixelIndex);
}