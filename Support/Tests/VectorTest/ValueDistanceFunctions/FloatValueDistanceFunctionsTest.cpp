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

#include "Types/Float.h"
#include "ValueDistanceFunctions/FloatValueDistanceFunctions.h"

TEST(FloatValueDistanceFunctions, AbsIdentityTest)
{
	Float a = {0.0f};
	Float b = {0.0f};

    EXPECT_EQ(AbsDistance(a, a), 0.0f);
	EXPECT_EQ(AbsDistance(a, b), 0.0f);

    a.x = 30.0f;
    b.x = 30.0f;

    EXPECT_EQ(AbsDistance(a, a), 0.0f);
    EXPECT_EQ(AbsDistance(a, b), 0.0f);
}

TEST(FloatValueDistanceFunctions, AbsDistanceTest)
{
    Float a = { 0.0f };
    Float b = { 3.0f };

    EXPECT_EQ(AbsDistance(a, b), 3.0f);
    EXPECT_EQ(AbsDistance(b, a), 3.0f);

    a = { 0.0f };
    b = { -9.0f };

    EXPECT_EQ(AbsDistance(a, b), 9.0f);
    EXPECT_EQ(AbsDistance(b, a), 9.0f);

    a.x = -4.0f;
    b.x = 17.0f;

    EXPECT_EQ(AbsDistance(a, b), 21.0f);
    EXPECT_EQ(AbsDistance(b, a), 21.0f);

    a.x = -2.0f;
    b.x = -2.5f;

    EXPECT_EQ(AbsDistance(a, b), 0.5f);
    EXPECT_EQ(AbsDistance(b, a), 0.5f);

    a.x = 100.1f;
    b.x = 25.6f;

    EXPECT_EQ(AbsDistance(a, b), 74.5f);
    EXPECT_EQ(AbsDistance(b, a), 74.5f);
}
