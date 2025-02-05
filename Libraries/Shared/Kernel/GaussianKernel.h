#pragma once

#include "SymmetricKernel.h"

class GaussianKernel : public SymmetricKernel
{
public:
    GaussianKernel(float a, float b, float c, size_t radius, bool evenWidthKernel = false);
    GaussianKernel(float mu, float sigma, size_t radius, bool evenWidthKernel = false);
};