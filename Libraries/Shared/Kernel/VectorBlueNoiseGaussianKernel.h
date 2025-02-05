#pragma once

#include "GaussianKernel.h"

class VectorBlueNoiseGaussianKernel : public GaussianKernel
{
public:
	VectorBlueNoiseGaussianKernel(float sigma, size_t maxKernelWidth);
};