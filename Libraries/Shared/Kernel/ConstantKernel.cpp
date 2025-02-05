#include "ConstantKernel.h"

ConstantKernel::ConstantKernel(float value, int radius, bool evenWidthKernel) :
	SymmetricKernel(std::vector<float>(static_cast<unsigned int>(radius)+1u, value), evenWidthKernel)
{

}