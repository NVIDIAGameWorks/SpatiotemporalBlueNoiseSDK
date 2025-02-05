#include "ReferenceController2Dx2D.h"

#include "ReferenceFuncs.h"

ReferenceController2Dx2D::ReferenceController2Dx2D(STBNData& data, SymmetricKernel kernelX, SymmetricKernel kernelY, SymmetricKernel kernelZ, SymmetricKernel kernelW) :
    m_data(data),
    m_kernelX(kernelX),
    m_kernelY(kernelY),
    m_kernelZ(kernelZ),
    m_kernelW(kernelW)
{

}

ReferenceController2Dx2D::~ReferenceController2Dx2D()
{

}

STBNData& ReferenceController2Dx2D::GetSTBNData()
{
    return m_data;
}

size_t ReferenceController2Dx2D::GetPixelOnCount() const
{
    return ReferenceFuncs::GetPixelOnCount(m_data);
}

size_t ReferenceController2Dx2D::GetTightestCluster()
{
    return ReferenceFuncs::GetTightestCluster(m_data);
}

size_t ReferenceController2Dx2D::GetLargestVoid()
{
    return ReferenceFuncs::GetLargestVoid(m_data);
}

void ReferenceController2Dx2D::SplatOn(const PixelCoords& pixelCoords)
{
    ReferenceFuncs::SplatOn2D(m_data, pixelCoords, 1, m_kernelY, 0, m_kernelX);
    ReferenceFuncs::SplatOn2D(m_data, pixelCoords, 3, m_kernelW, 2, m_kernelZ);
}

void ReferenceController2Dx2D::SplatOff(const PixelCoords& pixelCoords)
{
    ReferenceFuncs::SplatOff2D(m_data, pixelCoords, 1, m_kernelY, 0, m_kernelX);
    ReferenceFuncs::SplatOff2D(m_data, pixelCoords, 3, m_kernelW, 2, m_kernelZ);
}

void ReferenceController2Dx2D::SetPixelOn(size_t pixelIndex, bool value)
{
    ReferenceFuncs::SetPixelOn(m_data, pixelIndex, value);
}

void ReferenceController2Dx2D::SetPixelRank(size_t pixelIndex, size_t rank)
{
    ReferenceFuncs::SetPixelRank(m_data, pixelIndex, rank);
}

void ReferenceController2Dx2D::SetAllEnergyToZero()
{
    ReferenceFuncs::SetAllEnergyToZero(m_data);
}

void ReferenceController2Dx2D::InvertPixelOn(size_t pixelIndex)
{
    ReferenceFuncs::InvertPixelOn(m_data, pixelIndex);
}