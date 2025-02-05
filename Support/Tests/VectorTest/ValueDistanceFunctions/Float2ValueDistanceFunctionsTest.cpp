/*
* Copyright (c) 2024, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

#include "gtest/gtest.h"

#include "Types/Float2.h"
#include "ValueDistanceFunctions/Float2ValueDistanceFunctions.h"

TEST(Float2ValueDistanceFunctions, L1IdentityTest)
{
    Float2 a = { 0.0f, 0.0f };
    Float2 b = { 0.0f, 0.0f };

    EXPECT_EQ(L1(a, a), 0.0f);
    EXPECT_EQ(L1(a, b), 0.0f);

    a.x = 30.0f;
    b.x = 30.0f;

    EXPECT_EQ(L1(a, a), 0.0f);
    EXPECT_EQ(L1(a, b), 0.0f);
}

TEST(Float2ValueDistanceFunctions, L1DistanceTest)
{
    Float2 a = { 0.0f, 0.0f };
    Float2 b = { 0.0f, 1.0f };

    EXPECT_EQ(L1(a, b), 1.0f);
    EXPECT_EQ(L1(b, a), 1.0f);

    b.x = 1.0f;

    EXPECT_EQ(L1(a, b), 2.0f);
    EXPECT_EQ(L1(b, a), 2.0f);
}

TEST(Float2ValueDistanceFunctions, L2IdentityTest)
{
    Float2 a = { 0.0f, 0.0f };
    Float2 b = { 0.0f, 0.0f };

    EXPECT_EQ(L2(a, a), 0.0f);
    EXPECT_EQ(L2(a, b), 0.0f);

    a.x = 30.0f;
    b.x = 30.0f;

    EXPECT_EQ(L2(a, a), 0.0f);
    EXPECT_EQ(L2(a, b), 0.0f);
}

TEST(Float2ValueDistanceFunctions, L2DistanceTest)
{
    Float2 a = { 0.0f, 0.0f };
    Float2 b = { 0.0f, 1.0f };

    EXPECT_EQ(L2(a, b), 1.0f);

    b.x = 1.0f;

    EXPECT_EQ(L2(a, b), std::sqrtf(2.0f));
    EXPECT_EQ(L2(b, a), std::sqrtf(2.0f));
}

TEST(Float2ValueDistanceFunctions, LInfinityIdentityTest)
{
    Float2 a = { 0.0f, 0.0f };
    Float2 b = { 0.0f, 0.0f };

    EXPECT_EQ(LInfinity(a, a), 0.0f);
    EXPECT_EQ(LInfinity(a, b), 0.0f);

    a.x = 30.0f;
    b.x = 30.0f;

    EXPECT_EQ(LInfinity(a, a), 0.0f);
    EXPECT_EQ(LInfinity(a, b), 0.0f);
}

TEST(Float2ValueDistanceFunctions, LInfinityDistanceTest)
{
    Float2 a = { 0.0f, 0.0f };
    Float2 b = { 0.0f, 5.0f };

    EXPECT_EQ(LInfinity(a, b), 5.0f);
    EXPECT_EQ(LInfinity(b, a), 5.0f);

    b.x = -9.0f;

    EXPECT_EQ(LInfinity(a, b), 9.0f);
    EXPECT_EQ(LInfinity(b, a), 9.0f);
}
