#pragma once

#include "SymmetricKernel.h"

class ConstantKernel : public SymmetricKernel
{
public:
	ConstantKernel(float value, int radius, bool evenWidthKernel = false);
};