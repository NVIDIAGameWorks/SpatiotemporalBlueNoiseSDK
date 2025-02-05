#include "GaussianKernel.h"

static float sqrt2pi = 2.5066282746310005024157652848110452530069867406099383166299235763f; // Because WolframAlpha

float Gaussian(float a, float b, float c, float x)
{
	return a * (float)exp(- (x - b) * (x - b) / (2.0f * c * c));
}

std::vector<float> fillGaussianKernel(float a, float b, float c, size_t radius)
{
	std::vector<float> kernel(radius + 1);
	for(size_t x = 0; x < radius + 1; x++)
	{
		kernel[x] = Gaussian(a, b, c, static_cast<float>(x));
	}
	return kernel;
}

GaussianKernel::GaussianKernel(float a, float b, float c, size_t radius, bool evenWidthKernel) :
	SymmetricKernel(fillGaussianKernel(a, b, c, radius), evenWidthKernel)
{

}

GaussianKernel::GaussianKernel(float mu, float sigma, size_t radius, bool evenWidthKernel) :
	GaussianKernel(1.0f / (sigma * sqrt2pi), mu, sigma, radius, evenWidthKernel)
{

}