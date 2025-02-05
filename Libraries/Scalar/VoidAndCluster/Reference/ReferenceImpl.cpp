#include "ReferenceImpl.h"

#include "ReferenceFuncs.h"

ReferenceImpl2Dx1Dx1D::ReferenceImpl2Dx1Dx1D(STBNData& data) :
    m_data(data)
{

}

ReferenceImpl2Dx1Dx1D::~ReferenceImpl2Dx1Dx1D()
{

}

STBNData& ReferenceImpl2Dx1Dx1D::GetSTBNData()
{
    return m_data;
}

size_t ReferenceImpl2Dx1Dx1D::GetPixelOnCount() const
{
    return ReferenceFuncs::GetPixelOnCount(m_data);
}

size_t ReferenceImpl2Dx1Dx1D::GetTightestCluster() const
{
    return ReferenceFuncs::GetTightestCluster(m_data);
}

size_t ReferenceImpl2Dx1Dx1D::GetLargestVoid() const
{
    return ReferenceFuncs::GetLargestVoid(m_data);
}

void ReferenceImpl2Dx1Dx1D::SplatOnXY(const PixelCoords& pixelCoords, const SymmetricKernel& outerKernel, const SymmetricKernel& innerKernel)
{
    ReferenceFuncs::SplatOn2D(m_data, pixelCoords, 1, outerKernel, 0, innerKernel);
}

void ReferenceImpl2Dx1Dx1D::SplatOnZW(const PixelCoords& pixelCoords, const SymmetricKernel& outerKernel, const SymmetricKernel& innerKernel)
{
    ReferenceFuncs::SplatOn2D(m_data, pixelCoords, 3, outerKernel, 2, innerKernel);
}

void ReferenceImpl2Dx1Dx1D::SplatOnZ(const PixelCoords& pixelCoords, const SymmetricKernel& kernel)
{
    ReferenceFuncs::SplatOn1D(m_data, pixelCoords, 2, kernel);
}

void ReferenceImpl2Dx1Dx1D::SplatOnW(const PixelCoords& pixelCoords, const SymmetricKernel& kernel)
{
    ReferenceFuncs::SplatOn1D(m_data, pixelCoords, 3, kernel);
}

void ReferenceImpl2Dx1Dx1D::SplatOffXY(const PixelCoords& pixelCoords, const SymmetricKernel& outerKernel, const SymmetricKernel& innerKernel)
{
    ReferenceFuncs::SplatOff2D(m_data, pixelCoords, 1, outerKernel, 0, innerKernel);
}

void ReferenceImpl2Dx1Dx1D::SplatOffZW(const PixelCoords& pixelCoords, const SymmetricKernel& outerKernel, const SymmetricKernel& innerKernel)
{
    ReferenceFuncs::SplatOff2D(m_data, pixelCoords, 3, outerKernel, 2, innerKernel);
}

void ReferenceImpl2Dx1Dx1D::SplatOffZ(const PixelCoords& pixelCoords, const SymmetricKernel& kernel)
{
    ReferenceFuncs::SplatOff1D(m_data, pixelCoords, 2, kernel);
}

void ReferenceImpl2Dx1Dx1D::SplatOffW(const PixelCoords& pixelCoords, const SymmetricKernel& kernel)
{
    ReferenceFuncs::SplatOff1D(m_data, pixelCoords, 3, kernel);
}

void ReferenceImpl2Dx1Dx1D::SetPixelOn(size_t pixelIndex, bool value)
{
    ReferenceFuncs::SetPixelOn(m_data, pixelIndex, value);
}

void ReferenceImpl2Dx1Dx1D::SetPixelRank(size_t pixelIndex, size_t rank)
{
    ReferenceFuncs::SetPixelRank(m_data, pixelIndex, rank);
}

void ReferenceImpl2Dx1Dx1D::SetAllEnergyToZero()
{
    ReferenceFuncs::SetAllEnergyToZero(m_data);
}

void ReferenceImpl2Dx1Dx1D::InvertPixelOn(size_t pixelIndex)
{
    ReferenceFuncs::InvertPixelOn(m_data, pixelIndex);
}