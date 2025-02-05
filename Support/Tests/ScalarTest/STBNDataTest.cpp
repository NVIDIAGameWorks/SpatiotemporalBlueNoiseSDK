#include "gtest/gtest.h"

#include <vector>

#include "STBNData.h"

TEST(STBNData, Constructor)
{
    Dimensions dims = { 128, 128, 64, 1 };
    SigmaPerDimension sigmas = { 1.9f, 1.9f, 1.9f, 1.9f };
    float initialBinaryPatternDensity = 0.1f;
    float energySigma = 2.0f;
    size_t numPixels = dims.x * dims.y * dims.z * dims.w;

    std::vector<float> energy;
    energy.resize(numPixels, 0.0f);

    std::vector<bool> pixelOn;
    pixelOn.resize(numPixels, false);

    std::vector<size_t> pixelRank;
    pixelRank.resize(numPixels, numPixels);

    STBNData data(dims);

    EXPECT_EQ(data.numPixels, numPixels);

    EXPECT_EQ(data.energy.size(), numPixels);
    EXPECT_EQ(data.pixelOn.size(), numPixels);
    EXPECT_EQ(data.pixelRank.size(), numPixels);

    EXPECT_EQ(data.energy, energy);
    EXPECT_EQ(data.pixelOn, pixelOn);
    EXPECT_EQ(data.pixelRank, pixelRank);
}

TEST(STBNData, Equality)
{
    Dimensions dims = { 128, 128, 64, 1 };

    STBNData a(dims);
    STBNData b(dims);

    EXPECT_EQ(a, a);
    EXPECT_EQ(a, b);
}