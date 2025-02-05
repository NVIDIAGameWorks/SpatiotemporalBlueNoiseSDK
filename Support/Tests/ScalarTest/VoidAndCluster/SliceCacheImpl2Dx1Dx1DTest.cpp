#include "gtest/gtest.h"

#include <algorithm>
#include <random>
#include <vector>

#include "Kernel/BlueNoiseGaussianKernel.h"
#include "Kernel/SymmetricKernel.h"
#include "Kernel/ConstantKernel.h"
#include "Utils/PixelCoords.h"
#include "STBNData.h"
#include "STBNRandom.h"

#include "VoidAndCluster/Reference/ReferenceImpl.h"
#include "VoidAndCluster/Reference/ReferenceController2Dx1Dx1D.h"
#include "VoidAndCluster/SliceCache/SliceCacheImpl.h"
#include "VoidAndCluster/SliceCache/SliceCacheController2Dx1Dx1D.h"
#include "VoidAndCluster/VoidAndCluster.h"

#define TEST_IMPLS()   STBNData dataSC(dimensions); \
                       SliceCacheImpl updaterSC(dataSC); \
                       STBNData dataRef(dimensions); \
                       ReferenceImpl2Dx1Dx1D updaterRef(dataRef); \
                       std::vector<VCImpl*> impls = { &updaterSC, &updaterRef };

TEST(SliceCacheImpl, Constructor)
{
    STBNData data({ 1, 2, 3, 4 });
    SliceCacheImpl impl(data);

    EXPECT_EQ(impl.GetSTBNData(), data);
}

TEST(SliceCacheImpl, SplatZOnConstantR1At0)
{
    Dimensions dimensions{ 8, 1, 1, 1 };
    PixelCoords pixelCoords = { 0, 0, 0, 0 };
    ConstantKernel constantKernel(2.0f, 1);

    TEST_IMPLS();

    for (auto impl : impls)
    {
        impl->SplatOnZ(pixelCoords, constantKernel);
    }

    EXPECT_EQ(dataSC.energy, dataRef.energy);
}

TEST(SliceCacheImpl, SplatXYOnConstantR1At0)
{
    size_t width = 8;
    Dimensions dimensions{ width, 1, 1, 1 };
    PixelCoords pixelCoords = { 2, 2, 0, 0 };
    ConstantKernel constantKernelX(2.0f, 1);
    ConstantKernel constantKernelY(3.0f, 1);

    TEST_IMPLS();

    for (auto impl : impls)
    {
        impl->SplatOnXY(pixelCoords, constantKernelY, constantKernelX);
    }

    EXPECT_EQ(dataSC.energy, dataRef.energy);
}

TEST(SliceCacheImpl, GetTightestClusterLargestVoid2D)
{
    Dimensions dimensions{ 5, 5, 1, 1 };
    PixelCoords pixelCoords = { 2, 2, 0, 0 };
    ConstantKernel constantKernelX(2.0f, 1);
    ConstantKernel constantKernelY(3.0f, 1);

    TEST_IMPLS();

    for (auto impl : impls)
    {
        impl->SplatOnXY(pixelCoords, constantKernelY, constantKernelX);
        impl->SetPixelOn(PixelCoordsToPixelIndex(pixelCoords, dimensions), false);
    }

    EXPECT_EQ(updaterSC.GetTightestCluster(), updaterRef.GetTightestCluster());
    EXPECT_EQ(updaterSC.GetLargestVoid(), updaterRef.GetLargestVoid());
}

TEST(SliceCacheImpl, SplatCacheTest1Slice)
{
    Dimensions dimensions{ 5, 5, 5, 1 };
    PixelCoords pixelCoords = { 2, 2, 0, 0 };
    ConstantKernel constantKernelX(2.0f, 1);
    ConstantKernel constantKernelY(3.0f, 1);

    TEST_IMPLS();

    for (auto& impl : impls)
    {
        impl->SplatOnXY(pixelCoords, constantKernelY, constantKernelX);
        impl->SetPixelOn(PixelCoordsToPixelIndex(pixelCoords, dimensions), false);
    }

    EXPECT_EQ(updaterSC.GetTightestCluster(), updaterRef.GetTightestCluster());
    EXPECT_EQ(updaterSC.GetLargestVoid(), updaterRef.GetLargestVoid());
}

TEST(SliceCacheImpl, RandomSplatClusterVoidTest)
{
    size_t cubeSide = 16;
    Dimensions dimensions{ cubeSide, cubeSide, cubeSide, 1 };
    ConstantKernel constantKernelX(2.0f, 2);
    ConstantKernel constantKernelY(3.0f, 2);

    TEST_IMPLS();

    for (auto& impl : impls)
    {
        std::default_random_engine rng(1337);
        std::uniform_int_distribution dist(0, static_cast<int>(cubeSide-1));
        for (int i = 0; i < cubeSide*cubeSide*cubeSide*3; i++)
        {
            PixelCoords pixelCoords = { dist(rng), dist(rng), dist(rng), 0 };
            impl->SplatOnXY(pixelCoords, constantKernelY, constantKernelX);
            impl->SetPixelOn(PixelCoordsToPixelIndex(pixelCoords, dimensions), true);

            pixelCoords.x = dist(rng); pixelCoords.y = dist(rng); pixelCoords.z = dist(rng);
            impl->SplatOffXY(pixelCoords, constantKernelY, constantKernelX);
            impl->SetPixelOn(PixelCoordsToPixelIndex(pixelCoords, dimensions), false);
        }
    }

    EXPECT_EQ(updaterSC.GetTightestCluster(), updaterRef.GetTightestCluster());
    EXPECT_EQ(updaterSC.GetLargestVoid(), updaterRef.GetLargestVoid());
}

void initializeToWhiteNoise(VCImpl* impl, const Dimensions& dims)
{
    static BlueNoiseGaussianKernel kernel(1.9f, std::min(dims.x, std::min(dims.y, dims.z)));
    // generate an initial set of on pixels, with a max density of m_initialBinaryPatternDensity
    // If we get duplicate random numbers, we won't get the full targetCount of on pixels, but that is ok.
    const float ibpd = 0.1f;
    auto numPixels = dims.x * dims.y * dims.z * dims.w;
    pcg32_random_t rng = GetRNG();
    int count = std::max(size_t(float(numPixels) * ibpd), (size_t)2);
    for (int i = 0; i < count; ++i)
    {
        size_t pixelIndex = pcg32_boundedrand_r(&rng, (int)numPixels - 1);
        impl->SetPixelOn(pixelIndex, true);
        impl->SplatOnXY(PixelIndexToPixelCoords(pixelIndex, dims), kernel, kernel);
    }
}

TEST(SliceCacheImpl, QuasiBlueNoiseTest2D1D)
{
    size_t cubeSide = 16;
    Dimensions dimensions{ cubeSide, cubeSide, cubeSide, 1 };
    ConstantKernel kernelX(2.0f, 2);
    ConstantKernel kernelY(3.0f, 2);
    ConstantKernel kernelZ(1.0f, 1);

    TEST_IMPLS();

    initializeToWhiteNoise(&updaterSC, dimensions);
    initializeToWhiteNoise(&updaterRef, dimensions);

    for (auto& impl : impls)
    {
        std::default_random_engine rng(1337);
        std::uniform_int_distribution dist(0, static_cast<int>(cubeSide - 1));
        int numIters = static_cast<int>(cubeSide * cubeSide * cubeSide * 3);
        for (int i = 0; i < numIters; i++)
        {
            size_t tightestClusterIndex = impl->GetTightestCluster();
            impl->SetPixelOn(tightestClusterIndex, false);
            PixelCoords clusterCoords = PixelIndexToPixelCoords(tightestClusterIndex, dimensions);
            impl->SplatOffXY(clusterCoords, kernelY, kernelX);
            impl->SplatOffZ(clusterCoords, kernelZ);

            size_t largestVoidIndex = impl->GetLargestVoid();
            impl->SetPixelOn(largestVoidIndex, true);
            PixelCoords voidCoords = PixelIndexToPixelCoords(largestVoidIndex, dimensions);
            impl->SplatOnXY(voidCoords, kernelY, kernelX);
            impl->SplatOnZ(voidCoords, kernelZ);
        }
    }

    EXPECT_EQ(updaterSC.GetTightestCluster(), updaterRef.GetTightestCluster());
    EXPECT_EQ(updaterSC.GetLargestVoid(), updaterRef.GetLargestVoid());
}

TEST(SliceCacheImpl, QuasiBlueNoiseTest2D2D)
{
    size_t cubeSide = 16;
    Dimensions dimensions{ cubeSide, cubeSide, cubeSide, 1 };
    ConstantKernel kernelX(2.0f, 2);
    ConstantKernel kernelY(3.0f, 2);
    ConstantKernel kernelZ(1.0f, 1);
    ConstantKernel kernelW(6.0f, 0);

    TEST_IMPLS();

    initializeToWhiteNoise(&updaterSC, dimensions);
    initializeToWhiteNoise(&updaterRef, dimensions);

    for (auto& impl : impls)
    {
        std::default_random_engine rng(1337);
        std::uniform_int_distribution dist(0, static_cast<int>(cubeSide - 1));
        int numIters = static_cast<int>(cubeSide * cubeSide * cubeSide * 3);
        for (int i = 0; i < numIters; i++)
        {
            size_t tightestClusterIndex = impl->GetTightestCluster();
            impl->SetPixelOn(tightestClusterIndex, false);
            PixelCoords clusterCoords = PixelIndexToPixelCoords(tightestClusterIndex, dimensions);
            impl->SplatOffXY(clusterCoords, kernelY, kernelX);
            impl->SplatOffZW(clusterCoords, kernelW, kernelZ);

            size_t largestVoidIndex = impl->GetLargestVoid();
            impl->SetPixelOn(largestVoidIndex, true);
            PixelCoords voidCoords = PixelIndexToPixelCoords(largestVoidIndex, dimensions);
            impl->SplatOnXY(voidCoords, kernelY, kernelX);
            impl->SplatOnZW(voidCoords, kernelW, kernelZ);
        }
    }

    EXPECT_EQ(updaterSC.GetTightestCluster(), updaterRef.GetTightestCluster());
    EXPECT_EQ(updaterSC.GetLargestVoid(), updaterRef.GetLargestVoid());
}