#include "BlueNoiseGaussianKernel.h"

#include <numeric>

namespace
{

double sqrtNewtonRaphson(double x, double curr, double prev)
{
    return curr == prev
        ? curr
        : sqrtNewtonRaphson(x, 0.5 * (curr + x / curr), curr);
}

double sqrtNewtonRaphson(double x)
{
    return sqrtNewtonRaphson(x, x, 0.0);
}

size_t KernelRadius(float sigma)
{
    // float energyLoss = 0.005f;
    float logEnergyLoss = -5.29831736655f;// log(energyLoss);  not a constexpr unfortunately!
    return size_t(sqrtNewtonRaphson(-2.0f * sigma * sigma * logEnergyLoss));
}

size_t KernelRadius(float sigma, size_t maxKernelWidth)
{
    size_t radius = KernelRadius(sigma);
    return std::min(radius, std::max(1ull,maxKernelWidth/2ull));
}

bool shouldUseEvenKernelWidth(float sigma, size_t maxKernelWidth)
{
    return KernelRadius(sigma, maxKernelWidth) * 2 + 1 > maxKernelWidth;
}

}

BlueNoiseGaussianKernel::BlueNoiseGaussianKernel(float sigma, size_t maxKernelWidth) : GaussianKernel(1.0f, 0.0f, sigma, KernelRadius(sigma, maxKernelWidth), shouldUseEvenKernelWidth(sigma, maxKernelWidth))
{
    operator[](0) = 1.0f;
    if (max() > 0)
    {
        float sum = 0.0f;
        for (int i = 1; i < max() + 1; i++)
            sum += SymmetricKernel::operator[](size_t(i));
        for (int i = 0; i < max() + 1; i++)
            SymmetricKernel::operator[](size_t(i)) /= sum;
    }
}