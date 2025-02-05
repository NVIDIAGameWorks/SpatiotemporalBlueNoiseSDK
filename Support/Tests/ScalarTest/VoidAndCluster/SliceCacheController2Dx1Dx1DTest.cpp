#include "gtest/gtest.h"

#include "Kernel/BlueNoiseGaussianKernel.h"
#include "STBNData.h"

#include "VoidAndCluster/Reference/ReferenceController2Dx1Dx1D.h"
#include "VoidAndCluster/SliceCache/SliceCacheController2Dx1Dx1D.h"
#include "VoidAndCluster/VoidAndCluster.h"

static Dimensions dims = { 64, 64, 16, 1 };
static SigmaPerDimension sigmas = { 1.9f, 1.9f, 1.9f, 1.9f };
static float ibpd = 0.1;
static BlueNoiseGaussianKernel kx(sigmas.x, dims.x);
static BlueNoiseGaussianKernel ky(sigmas.y, dims.y);
static BlueNoiseGaussianKernel kz(sigmas.z, dims.z);
static BlueNoiseGaussianKernel kw(sigmas.w, dims.w);

static VoidAndCluster* get_Global_VC_SC_64x64x16x1()
{
    static STBNData dataSC(dims);
    static SliceCacheController2Dx1Dx1D vccSC(dataSC, kx, ky, kz, kw);
    static VoidAndCluster vcSC(ibpd, &vccSC);

    return &vcSC;
}

static VoidAndCluster* get_Global_VC_Ref_64x64x16x1()
{
    static STBNData dataSC(dims);
    static ReferenceController2Dx1Dx1D vccRef(dataSC, kx, ky, kz, kw);
    static VoidAndCluster vcRef(ibpd, &vccRef);

    return &vcRef;
}

TEST(SliceCacheController2Dx1Dx1D, WhiteNoise64x64x16)
{
    auto vcSC = get_Global_VC_SC_64x64x16x1();
    auto vcRef = get_Global_VC_Ref_64x64x16x1();

    vcSC->InitializeToWhiteNoise();
    vcRef->InitializeToWhiteNoise();

    EXPECT_EQ(vcSC->GetSTBNData().energy, vcRef->GetSTBNData().energy);
}

TEST(SliceCacheController2Dx1Dx1D, ReorganizeToBlueNoise64x64x16)
{
    auto vcSC = get_Global_VC_SC_64x64x16x1();
    auto vcRef = get_Global_VC_Ref_64x64x16x1();

    vcSC->ReorganizeToBlueNoise();
    vcRef->ReorganizeToBlueNoise();

    EXPECT_EQ(vcSC->GetSTBNData().energy, vcRef->GetSTBNData().energy);
}

TEST(SliceCacheController2Dx1Dx1D, Phase1)
{
    auto vcSC = get_Global_VC_SC_64x64x16x1();
    auto vcRef = get_Global_VC_Ref_64x64x16x1();

    vcSC->Phase1();
    vcRef->Phase1();

    EXPECT_EQ(vcSC->GetSTBNData().energy, vcRef->GetSTBNData().energy);
}

TEST(SliceCacheController2Dx1Dx1D, Phase2)
{
    auto vcSC = get_Global_VC_SC_64x64x16x1();
    auto vcRef = get_Global_VC_Ref_64x64x16x1();

    vcSC->Phase2();
    vcRef->Phase2();

    EXPECT_EQ(vcSC->GetSTBNData().energy, vcRef->GetSTBNData().energy);
}

TEST(SliceCacheController2Dx1Dx1D, Phase3)
{
    auto vcSC = get_Global_VC_SC_64x64x16x1();
    auto vcRef = get_Global_VC_Ref_64x64x16x1();

    vcSC->Phase3();
    vcRef->Phase3();

    EXPECT_EQ(vcSC->GetSTBNData().energy, vcRef->GetSTBNData().energy);
}