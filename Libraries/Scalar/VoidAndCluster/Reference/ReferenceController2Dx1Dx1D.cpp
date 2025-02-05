#include "ReferenceController2Dx1Dx1D.h"

#include "ReferenceFuncs.h"

ReferenceController2Dx1Dx1D::ReferenceController2Dx1Dx1D(STBNData& data, SymmetricKernel kernelX, SymmetricKernel kernelY, SymmetricKernel kernelZ, SymmetricKernel kernelW) :
    m_data(data),
    m_kernelX(kernelX),
    m_kernelY(kernelY),
    m_kernelZ(kernelZ),
    m_kernelW(kernelW)
{

}

ReferenceController2Dx1Dx1D::~ReferenceController2Dx1Dx1D()
{

}

STBNData& ReferenceController2Dx1Dx1D::GetSTBNData()
{
    return m_data;
}

size_t ReferenceController2Dx1Dx1D::GetPixelOnCount() const
{
    return ReferenceFuncs::GetPixelOnCount(m_data);
}

size_t ReferenceController2Dx1Dx1D::GetTightestCluster()
{
    return ReferenceFuncs::GetTightestCluster(m_data);
}

size_t ReferenceController2Dx1Dx1D::GetLargestVoid()
{
    return ReferenceFuncs::GetLargestVoid(m_data);
}

void ReferenceController2Dx1Dx1D::SplatOn(const PixelCoords& pixelCoords)
{
    ReferenceFuncs::SplatOn2D(m_data, pixelCoords, 1, m_kernelY, 0, m_kernelX);
    ReferenceFuncs::SplatOn1D(m_data, pixelCoords, 2, m_kernelZ);
    ReferenceFuncs::SplatOn1D(m_data, pixelCoords, 3, m_kernelW);
}

void ReferenceController2Dx1Dx1D::SplatOff(const PixelCoords& pixelCoords)
{
    ReferenceFuncs::SplatOff2D(m_data, pixelCoords, 1, m_kernelY, 0, m_kernelX);
    ReferenceFuncs::SplatOff1D(m_data, pixelCoords, 2, m_kernelZ);
    ReferenceFuncs::SplatOff1D(m_data, pixelCoords, 3, m_kernelW);
}

void ReferenceController2Dx1Dx1D::SetPixelOn(size_t pixelIndex, bool value)
{
    ReferenceFuncs::SetPixelOn(m_data, pixelIndex, value);
}

void ReferenceController2Dx1Dx1D::SetPixelRank(size_t pixelIndex, size_t rank)
{
    ReferenceFuncs::SetPixelRank(m_data, pixelIndex, rank);
}

void ReferenceController2Dx1Dx1D::SetAllEnergyToZero()
{
    ReferenceFuncs::SetAllEnergyToZero(m_data);
}

void ReferenceController2Dx1Dx1D::InvertPixelOn(size_t pixelIndex)
{
    ReferenceFuncs::InvertPixelOn(m_data, pixelIndex);
}