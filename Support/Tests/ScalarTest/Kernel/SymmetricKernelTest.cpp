#include "gtest/gtest.h"

#include "Kernel/SymmetricKernel.h"

TEST(SymmetricKernel, Constructor1)
{
    std::vector<float> kernelData = { 1.0f };
    SymmetricKernel kernel(kernelData);

    for (int i = 0; i < kernelData.size(); i++)
    {
        EXPECT_EQ(kernel[i], kernelData[i]);
    }

    EXPECT_EQ(kernel.start(), 0);
    EXPECT_EQ(kernel.end(), 0);
    EXPECT_EQ(kernel.max(), 0);
}

TEST(SymmetricKernel, Constructor5)
{
	std::vector<float> kernelData = {1.0f, 2.0f, 3.0f, 8.0, 5.0f};
	SymmetricKernel kernel(kernelData);

    for(int i = 0; i < kernelData.size(); i++)
    {
    	EXPECT_EQ(kernel[i], kernelData[i]);
    }

    EXPECT_EQ(kernel.start(), -(kernelData.size() - 1));
    EXPECT_EQ(kernel.end(), kernelData.size() - 1);
    EXPECT_EQ(kernel.max(), kernelData.size() - 1);
}

TEST(SymmetricKernel, MaxWidth10)
{
    std::vector<float> kernelData = { 1.0f, 2.0f, 3.0f, 8.0, 5.0f, 17.2f };
    bool evenKernelWidth = true;
    SymmetricKernel kernel(kernelData, evenKernelWidth);

    for (int i = 0; i < kernelData.size(); i++)
    {
        EXPECT_EQ(kernel[i], kernelData[i]);
    }

    EXPECT_EQ(kernel.start(), -(kernelData.size() - 2));
    EXPECT_EQ(kernel.end(), kernelData.size() - 1);
    EXPECT_EQ(kernel.max(), kernelData.size() - 1);
}