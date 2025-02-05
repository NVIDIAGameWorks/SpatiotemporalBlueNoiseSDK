#include "gtest/gtest.h"

#include <algorithm>
#include <vector>

#include "Kernel/SymmetricKernel.h"
#include "Kernel/ConstantKernel.h"
#include "Utils/PixelCoords.h"
#include "STBNData.h"

#include "VoidAndCluster/Reference/ReferenceFuncs.h"

TEST(ReferenceImpl, Splat1DOnConstantR1At0)
{
    size_t width = 8;
    Dimensions dimensions{ width, 1, 1, 1 };
    STBNData data(dimensions);

    PixelCoords pixelCoords = { 0, 0, 0, 0 };
    float kernelValue = 2.0f;
    size_t radius = 1;
    ConstantKernel constantKernel(kernelValue, radius);
    ReferenceFuncs::SplatOn1D(data, pixelCoords, 0, constantKernel);

    std::vector<float> correctEnergy = { kernelValue, kernelValue, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, kernelValue };
    EXPECT_EQ(data.energy, correctEnergy);
}

TEST(ReferenceImpl, Splat1DOnConstantR2At3)
{
    size_t width = 8;
    Dimensions dimensions{ width, 1, 1, 1 };
    STBNData data(dimensions);

    PixelCoords pixelCoords = { 3, 0, 0, 0 };
    float kernelValue = 2.2f;
    size_t radius = 2;
    ConstantKernel constantKernel(kernelValue, radius);
    ReferenceFuncs::SplatOn1D(data, pixelCoords, 0, constantKernel);

    std::vector<float> correctEnergy = { 0.0f, kernelValue, kernelValue, kernelValue, kernelValue, kernelValue, 0.0f, 0.0f };
    EXPECT_EQ(data.energy, correctEnergy);
}

TEST(ReferenceImpl, Splat2DOnConstantR1At22)
{
    size_t widthX = 5;
    size_t widthY = 5;
    Dimensions dimensions{ widthX, widthY, 1, 1 };
    STBNData data(dimensions);

    PixelCoords pixelCoords = { 2, 2, 0, 0 };
    float kernelValueX = 2.0f;
    size_t radiusX = 1;
    ConstantKernel constantKernelX(kernelValueX, radiusX);
    float kernelValueY = 3.0f;
    size_t radiusY = 1;
    ConstantKernel constantKernelY(kernelValueY, radiusY);
    ReferenceFuncs::SplatOn2D(data, pixelCoords, 1, constantKernelY, 0, constantKernelX);

    float kv = kernelValueX * kernelValueY;

    std::vector<float> correctEnergy = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                         0.0f,   kv,   kv,   kv, 0.0f,
                                         0.0f,   kv,   kv,   kv, 0.0f,
                                         0.0f,   kv,   kv,   kv, 0.0f,
                                         0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    EXPECT_EQ(data.energy, correctEnergy);
}

TEST(ReferenceImpl, Splat2DOnConstantR2R1At22)
{
    size_t widthX = 5;
    size_t widthY = 5;
    Dimensions dimensions{ widthX, widthY, 1, 1 };
    STBNData data(dimensions);

    PixelCoords pixelCoords = { 2, 2, 0, 0 };
    float kernelValueX = 2.0f;
    size_t radiusX = 1;
    ConstantKernel constantKernelX(kernelValueX, radiusX);
    float kernelValueY = 3.0f;
    size_t radiusY = 2;
    ConstantKernel constantKernelY(kernelValueY, radiusY);
    ReferenceFuncs::SplatOn2D(data, pixelCoords, 1, constantKernelY, 0, constantKernelX);

    float kv = kernelValueX * kernelValueY;

    std::vector<float> correctEnergy = { 0.0f,   kv,   kv,   kv, 0.0f,
                                         0.0f,   kv,   kv,   kv, 0.0f,
                                         0.0f,   kv,   kv,   kv, 0.0f,
                                         0.0f,   kv,   kv,   kv, 0.0f,
                                         0.0f,   kv,   kv,   kv, 0.0f };
    EXPECT_EQ(data.energy, correctEnergy);
}

TEST(ReferenceImpl, Splat2DOnConstantR2R1At42)
{
    size_t widthX = 5;
    size_t widthY = 5;
    Dimensions dimensions{ widthX, widthY, 1, 1 };
    STBNData data(dimensions);

    PixelCoords pixelCoords = { 4, 2, 0, 0 };
    float kernelValueX = 2.0f;
    size_t radiusX = 1;
    ConstantKernel constantKernelX(kernelValueX, radiusX);
    float kernelValueY = 3.0f;
    size_t radiusY = 2;
    ConstantKernel constantKernelY(kernelValueY, radiusY);
    ReferenceFuncs::SplatOn2D(data, pixelCoords, 1, constantKernelY, 0, constantKernelX);

    float kv = kernelValueX * kernelValueY;

    std::vector<float> correctEnergy = { kv, 0.0f, 0.0f,   kv,   kv,
                                         kv, 0.0f, 0.0f,   kv,   kv,
                                         kv, 0.0f, 0.0f,   kv,   kv,
                                         kv, 0.0f, 0.0f,   kv,   kv,
                                         kv, 0.0f, 0.0f,   kv,   kv };

    EXPECT_EQ(data.energy, correctEnergy);
}

TEST(ReferenceImpl, Splat2DOnConstantR1R1At221Splat1DOnConstantR1R1At221)
{
    size_t widthX = 5;
    size_t widthY = 5;
    size_t widthZ = 3;
    Dimensions dimensions{ widthX, widthY, widthZ, 1 };
    STBNData data(dimensions);

    PixelCoords pixelCoords1 = { 2, 2, 1, 0 };
    float kernelValueX = 2.0f;
    size_t radiusX = 1;
    ConstantKernel constantKernelX(kernelValueX, radiusX);
    float kernelValueY = 3.0f;
    size_t radiusY = 1;
    ConstantKernel constantKernelY(kernelValueY, radiusY);
    ReferenceFuncs::SplatOn2D(data, pixelCoords1, 1, constantKernelY, 0, constantKernelX);

    PixelCoords pixelCoords2 = { 2, 2, 1, 0 };
    float kernelValueZ = 5.0f;
    size_t radiusZ = 1;
    ConstantKernel constantKernelZ(kernelValueZ, radiusZ);
    ReferenceFuncs::SplatOn1D(data, pixelCoords2, 2, constantKernelZ);

    float kv1 = kernelValueX * kernelValueY;
    float kv2 = kernelValueX * kernelValueY + kernelValueZ;
    float kv3 = kernelValueZ;

    std::vector<float> correctEnergy = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                         0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                         0.0f, 0.0f,  kv3, 0.0f, 0.0f,
                                         0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                         0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

                                         0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                         0.0f,  kv1,  kv1,  kv1, 0.0f,
                                         0.0f,  kv1,  kv2,  kv1, 0.0f,
                                         0.0f,  kv1,  kv1,  kv1, 0.0f,
                                         0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

                                         0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                         0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                         0.0f, 0.0f,  kv3, 0.0f, 0.0f,
                                         0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                         0.0f, 0.0f, 0.0f, 0.0f, 0.0f
    };

    EXPECT_EQ(data.energy, correctEnergy);
}

TEST(ReferenceImpl, Splat1DOffConstantR1At0)
{
    size_t width = 8;
    Dimensions dimensions{ width, 1, 1, 1 };
    STBNData data(dimensions);

    PixelCoords pixelCoords = { 0, 0, 0, 0 };
    float kernelValue = 2.0f;
    size_t radius = 1;
    ConstantKernel constantKernel(kernelValue, radius);
    ReferenceFuncs::SplatOff1D(data, pixelCoords, 0, constantKernel);

    float sv = -kernelValue;

    std::vector<float> correctEnergy = { sv, sv, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, sv };
    EXPECT_EQ(data.energy, correctEnergy);
}

TEST(ReferenceImpl, Splat1DOffConstantR2At3)
{
    size_t width = 8;
    Dimensions dimensions{ width, 1, 1, 1 };
    STBNData data(dimensions);

    PixelCoords pixelCoords = { 3, 0, 0, 0 };
    float kernelValue = 2.2f;
    size_t radius = 2;
    ConstantKernel constantKernel(kernelValue, radius);
    ReferenceFuncs::SplatOff1D(data, pixelCoords, 0, constantKernel);

    float sv = -kernelValue;

    std::vector<float> correctEnergy = { 0.0f, sv, sv, sv, sv, sv, 0.0f, 0.0f };
    EXPECT_EQ(data.energy, correctEnergy);
}

TEST(ReferenceImpl, Splat2DOffConstantR1At22)
{
    size_t widthX = 5;
    size_t widthY = 5;
    Dimensions dimensions{ widthX, widthY, 1, 1 };
    STBNData data(dimensions);

    PixelCoords pixelCoords = { 2, 2, 0, 0 };
    float kernelValueX = 2.0f;
    size_t radiusX = 1;
    ConstantKernel constantKernelX(kernelValueX, radiusX);
    float kernelValueY = 3.0f;
    size_t radiusY = 1;
    ConstantKernel constantKernelY(kernelValueY, radiusY);
    ReferenceFuncs::SplatOff2D(data, pixelCoords, 1, constantKernelY, 0, constantKernelX);

    float kv = -kernelValueX * kernelValueY;

    std::vector<float> correctEnergy = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                         0.0f,   kv,   kv,   kv, 0.0f,
                                         0.0f,   kv,   kv,   kv, 0.0f,
                                         0.0f,   kv,   kv,   kv, 0.0f,
                                         0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    EXPECT_EQ(data.energy, correctEnergy);
}

TEST(ReferenceImpl, Splat2DOffConstantR2R1At22)
{
    size_t widthX = 5;
    size_t widthY = 5;
    Dimensions dimensions{ widthX, widthY, 1, 1 };
    STBNData data(dimensions);

    PixelCoords pixelCoords = { 2, 2, 0, 0 };
    float kernelValueX = 2.0f;
    size_t radiusX = 1;
    ConstantKernel constantKernelX(kernelValueX, radiusX);
    float kernelValueY = 3.0f;
    size_t radiusY = 2;
    ConstantKernel constantKernelY(kernelValueY, radiusY);
    ReferenceFuncs::SplatOff2D(data, pixelCoords, 1, constantKernelY, 0, constantKernelX);

    float kv = -kernelValueX * kernelValueY;

    std::vector<float> correctEnergy = { 0.0f,   kv,   kv,   kv, 0.0f,
                                         0.0f,   kv,   kv,   kv, 0.0f,
                                         0.0f,   kv,   kv,   kv, 0.0f,
                                         0.0f,   kv,   kv,   kv, 0.0f,
                                         0.0f,   kv,   kv,   kv, 0.0f };
    EXPECT_EQ(data.energy, correctEnergy);
}

TEST(ReferenceImpl, Splat2DOffConstantR2R1At42)
{
    size_t widthX = 5;
    size_t widthY = 5;
    Dimensions dimensions{ widthX, widthY, 1, 1 };
    STBNData data(dimensions);

    PixelCoords pixelCoords = { 4, 2, 0, 0 };
    float kernelValueX = 2.0f;
    size_t radiusX = 1;
    ConstantKernel constantKernelX(kernelValueX, radiusX);
    float kernelValueY = 3.0f;
    size_t radiusY = 2;
    ConstantKernel constantKernelY(kernelValueY, radiusY);
    ReferenceFuncs::SplatOff2D(data, pixelCoords, 1, constantKernelY, 0, constantKernelX);

    float kv = -kernelValueX * kernelValueY;

    std::vector<float> correctEnergy = { kv, 0.0f, 0.0f,   kv,   kv,
                                         kv, 0.0f, 0.0f,   kv,   kv,
                                         kv, 0.0f, 0.0f,   kv,   kv,
                                         kv, 0.0f, 0.0f,   kv,   kv,
                                         kv, 0.0f, 0.0f,   kv,   kv };

    EXPECT_EQ(data.energy, correctEnergy);
}

TEST(ReferenceImpl, Splat2DOffConstantR1R1At221Splat1DOffConstantR1R1At221)
{
    size_t widthX = 5;
    size_t widthY = 5;
    size_t widthZ = 3;
    Dimensions dimensions{ widthX, widthY, widthZ, 1 };
    STBNData data(dimensions);

    PixelCoords pixelCoords1 = { 2, 2, 1, 0 };
    float kernelValueX = 2.0f;
    size_t radiusX = 1;
    ConstantKernel constantKernelX(kernelValueX, radiusX);
    float kernelValueY = 3.0f;
    size_t radiusY = 1;
    ConstantKernel constantKernelY(kernelValueY, radiusY);
    ReferenceFuncs::SplatOff2D(data, pixelCoords1, 1, constantKernelY, 0, constantKernelX);

    PixelCoords pixelCoords2 = { 2, 2, 1, 0 };
    float kernelValueZ = 5.0f;
    size_t radiusZ = 1;
    ConstantKernel constantKernelZ(kernelValueZ, radiusZ);
    ReferenceFuncs::SplatOff1D(data, pixelCoords2, 2, constantKernelZ);

    float kv1 = -kernelValueX * kernelValueY;
    float kv2 = -kernelValueX * kernelValueY - kernelValueZ;
    float kv3 = -kernelValueZ;

    std::vector<float> correctEnergy = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                         0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                         0.0f, 0.0f,  kv3, 0.0f, 0.0f,
                                         0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                         0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

                                         0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                         0.0f,  kv1,  kv1,  kv1, 0.0f,
                                         0.0f,  kv1,  kv2,  kv1, 0.0f,
                                         0.0f,  kv1,  kv1,  kv1, 0.0f,
                                         0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

                                         0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                         0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                         0.0f, 0.0f,  kv3, 0.0f, 0.0f,
                                         0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                         0.0f, 0.0f, 0.0f, 0.0f, 0.0f
    };

    EXPECT_EQ(data.energy, correctEnergy);
}

TEST(ReferenceImpl, Splat2DOnConstantR1R1At221Splat1DOffConstantR1R1At221)
{
    size_t widthX = 5;
    size_t widthY = 5;
    size_t widthZ = 3;
    Dimensions dimensions{ widthX, widthY, widthZ, 1 };
    STBNData data(dimensions);

    PixelCoords pixelCoords1 = { 2, 2, 1, 0 };
    float kernelValueX = 2.0f;
    size_t radiusX = 1;
    ConstantKernel constantKernelX(kernelValueX, radiusX);
    float kernelValueY = 3.0f;
    size_t radiusY = 1;
    ConstantKernel constantKernelY(kernelValueY, radiusY);
    ReferenceFuncs::SplatOn2D(data, pixelCoords1, 1, constantKernelY, 0, constantKernelX);

    PixelCoords pixelCoords2 = { 2, 2, 1, 0 };
    float kernelValueZ = 5.0f;
    size_t radiusZ = 1;
    ConstantKernel constantKernelZ(kernelValueZ, radiusZ);
    ReferenceFuncs::SplatOff1D(data, pixelCoords2, 2, constantKernelZ);

    float kv1 = kernelValueX * kernelValueY;
    float kv2 = kernelValueX * kernelValueY - kernelValueZ;
    float kv3 = -kernelValueZ;

    std::vector<float> correctEnergy = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                         0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                         0.0f, 0.0f,  kv3, 0.0f, 0.0f,
                                         0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                         0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

                                         0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                         0.0f,  kv1,  kv1,  kv1, 0.0f,
                                         0.0f,  kv1,  kv2,  kv1, 0.0f,
                                         0.0f,  kv1,  kv1,  kv1, 0.0f,
                                         0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

                                         0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                         0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                         0.0f, 0.0f,  kv3, 0.0f, 0.0f,
                                         0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                         0.0f, 0.0f, 0.0f, 0.0f, 0.0f
    };

    EXPECT_EQ(data.energy, correctEnergy);
}