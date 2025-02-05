#include "SymmetricKernel.h"

#include <algorithm>

SymmetricKernel::SymmetricKernel(const std::vector<float>& kernelData, bool evenWidthKernel) :
	m_kernel(kernelData),
    m_start(evenWidthKernel ? std::min(0, -(static_cast<int>(kernelData.size()) - 2)) : -(static_cast<int>(kernelData.size()) - 1)),
	m_end(std::max(static_cast<int>(kernelData.size()) - 1, 0)),
	m_max(std::max(m_start, m_end))
{

}

float& SymmetricKernel::operator[](size_t index)
{
    return m_kernel[index];
}

const float& SymmetricKernel::operator[](size_t index) const
{
    return m_kernel.at(index);
}

int SymmetricKernel::start() const
{
    return m_start;
}

int SymmetricKernel::end() const
{
    return m_end;
}

int SymmetricKernel::max() const
{
    return m_max;
}