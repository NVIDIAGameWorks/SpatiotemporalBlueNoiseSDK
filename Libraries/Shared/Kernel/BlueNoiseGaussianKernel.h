#pragma once

#include "GaussianKernel.h"

class BlueNoiseGaussianKernel : public GaussianKernel
{
public:
	BlueNoiseGaussianKernel(float sigma, size_t maxKernelWidth);
};