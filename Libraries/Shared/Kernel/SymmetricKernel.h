#pragma once

#include <vector>

class SymmetricKernel
{
public:
    // The evenWidthKernel flag may be from a bug in the original implementation
    // In 2Dx1Dx1D, the W kernel has 2 elements in it, but its start is 0 instead of -1 (bug?), and its end is 1 (correct).
    // The kernel is not symmetric like the others.
    // However, this option is here to let us pass the comparison tests with the original implementation
	SymmetricKernel(const std::vector<float>& kernelData, bool evenWidthKernel = false);

    float& operator[](size_t);
    const float& operator[](size_t) const;

    int start() const;
    int end() const;
    int max() const;

private:
	std::vector<float> m_kernel;

	int m_start;
	int m_end;
	int m_max;
};