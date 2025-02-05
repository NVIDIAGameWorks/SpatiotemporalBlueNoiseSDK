#include "gtest/gtest.h"

#include "Kernel/ConstantKernel.h"

TEST(ConstantKernel, Radius1)
{
    float constantValue = 3.2f;
    size_t radius = 0;
    ConstantKernel kernel(constantValue, radius);

    for (int i = 0; i < kernel.max() + 1; i++)
    {
        EXPECT_EQ(kernel[i], constantValue);
    }

    EXPECT_EQ(kernel.start(), 0);
    EXPECT_EQ(kernel.end(), 0);
    EXPECT_EQ(kernel.max(), 0);
}

TEST(ConstantKernel, Radius6)
{
    float constantValue = 3.2f;
    size_t radius = 5;
    ConstantKernel kernel(constantValue, radius);
    for (int i = 0; i < radius + 1; i++)
    {
        EXPECT_EQ(kernel[i], constantValue);
    }

    EXPECT_EQ(kernel.start(), -radius);
    EXPECT_EQ(kernel.end(), radius);
    EXPECT_EQ(kernel.max(), radius);
}
