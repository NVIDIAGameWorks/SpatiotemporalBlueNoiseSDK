#include "gtest/gtest.h"

#include "Kernel/GaussianKernel.h"

TEST(GaussianKernel, Constructor)
{
	GaussianKernel kernel1(1.0f, 2.0f, 3.0f, 4);
	GaussianKernel kernel2(1.0f, 2.0f, 8);
}

TEST(GaussianKernel, MuSigmaValues)
{
	float mu = 0;
	float sigma = 1;
	size_t radius = 5;
	GaussianKernel kernel(mu, sigma, radius);

    EXPECT_EQ(kernel.start(), -radius);
    EXPECT_EQ(kernel.end(), radius);
    EXPECT_EQ(kernel.max(), radius);

    // Checked with WolframAlpha
    EXPECT_EQ(kernel[0], 0.398942280f);
    EXPECT_EQ(kernel[1], 0.241970733f);
    EXPECT_EQ(kernel[2], 0.0539909665f);
    EXPECT_EQ(kernel[3], 0.004431848411f);
    EXPECT_EQ(kernel[4], 0.00013383022576f);
    EXPECT_EQ(kernel[5], 0.00000148671961f);
}