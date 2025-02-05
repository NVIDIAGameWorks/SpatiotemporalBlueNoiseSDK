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

#include "Types/Float3.h"
#include "ValueDistanceFunctions/Float3ValueDistanceFunctions.h"

TEST(Float3ValueDistanceFunctions, L1IdentityTest)
{
    Float3 a = { 0.0f, 0.0f, 0.0f };
    Float3 b = { 0.0f, 0.0f, 0.0f };

    EXPECT_EQ(L1(a, a), 0.0f);
    EXPECT_EQ(L1(a, b), 0.0f);

    a.x = 30.0f;
    b.x = 30.0f;

    EXPECT_EQ(L1(a, a), 0.0f);
    EXPECT_EQ(L1(a, b), 0.0f);
}

TEST(Float3ValueDistanceFunctions, L1DistanceTest)
{
    Float3 a = { 0.0f, 0.0f, 0.0f };
    Float3 b = { 0.0f, 0.0f, 1.0f };

    EXPECT_EQ(L1(a, b), 1.0f);
    EXPECT_EQ(L1(b, a), 1.0f);

    b.x = 1.0f;

    EXPECT_EQ(L1(a, b), 2.0f);
    EXPECT_EQ(L1(b, a), 2.0f);
}

TEST(Float3ValueDistanceFunctions, L2IdentityTest)
{
    Float3 a = { 0.0f, 0.0f, 0.0f };
    Float3 b = { 0.0f, 0.0f, 0.0f };

    EXPECT_EQ(L2(a, a), 0.0f);
    EXPECT_EQ(L2(a, b), 0.0f);

    a.x = 30.0f;
    b.x = 30.0f;

    EXPECT_EQ(L2(a, a), 0.0f);
    EXPECT_EQ(L2(a, b), 0.0f);
}

TEST(Float3ValueDistanceFunctions, L2DistanceTest)
{
    Float3 a = { 0.0f, 0.0f, 0.0f };
    Float3 b = { 0.0f, 0.0f, 1.0f };

    EXPECT_EQ(L2(a, b), 1.0f);

    b.x = 1.0f;

    EXPECT_EQ(L2(a, b), std::sqrtf(2.0f));
    EXPECT_EQ(L2(b, a), std::sqrtf(2.0f));
}

TEST(Float3ValueDistanceFunctions, LInfinityIdentityTest)
{
    Float3 a = { 0.0f, 0.0f, 0.0f };
    Float3 b = { 0.0f, 0.0f, 0.0f };

    EXPECT_EQ(LInfinity(a, a), 0.0f);
    EXPECT_EQ(LInfinity(a, b), 0.0f);

    a.x = 30.0f;
    b.x = 30.0f;

    EXPECT_EQ(LInfinity(a, a), 0.0f);
    EXPECT_EQ(LInfinity(a, b), 0.0f);
}

TEST(Float3ValueDistanceFunctions, LInfinityDistanceTest)
{
    Float3 a = { 0.0f, 0.0f, 0.0f };
    Float3 b = { 0.0f, 0.0f, 5.0f };

    EXPECT_EQ(LInfinity(a, b), 5.0f);
    EXPECT_EQ(LInfinity(b, a), 5.0f);

    b.x = -9.0f;

    EXPECT_EQ(LInfinity(a, b), 9.0f);
    EXPECT_EQ(LInfinity(b, a), 9.0f);
}

TEST(Float3ValueDistanceFunctions, DotTest)
{
    Float3 a = { 0.0f, 0.0f, 0.0f };
    Float3 b = { 0.0f, 0.0f, 0.0f };

    EXPECT_EQ(Dot(a, a), 0.0f);
    EXPECT_EQ(Dot(a, b), 0.0f);

    a.x = 30.0f;
    b.x = -15.0f;

    EXPECT_EQ(Dot(a, a),  900.0f);
    EXPECT_EQ(Dot(a, b), -450.0f);
}

TEST(Float3ValueDistanceFunctions, NegativeDotTest)
{
    Float3 a = { 0.0f, 0.0f, 0.0f };
    Float3 b = { 0.0f, 0.0f, 0.0f };

    EXPECT_EQ(NegativeDot(a, a), 0.0f);
    EXPECT_EQ(NegativeDot(a, b), 0.0f);

    a.x = 30.0f;
    b.x = -15.0f;

    EXPECT_EQ(NegativeDot(a, a), -900.0f);
    EXPECT_EQ(NegativeDot(a, b),  450.0f);
}
