/*
* Copyright (c) 2019-2021, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

#include "../Shared/shared.h"

namespace _2Dx2D
{
    template <size_t dimsX, size_t dimsY, size_t dimsZ, size_t dimsW, float sigmaX, float sigmaY, float sigmaZ, float sigmaW>
    class Maker
    {
    public:
        Maker()
            : m_progress(c_numPixels)
        {
            // resize and initialze our data buffers
            m_energy.resize(c_numPixels, 0.0f);
            m_pixelOn.resize(c_numPixels, false);
            m_pixelRank.resize(c_numPixels, c_numPixels);

            // calculate the kernels
            {
                m_kernelX[0] = 1.0f;
                float sum = 0.0f;
                for (size_t index = 1; index <= c_KernelRadiusMaxX; ++index)
                {
                    float x = float(index);
                    m_kernelX[index] = exp(-(x * x) / (2.0f * sigmaX * sigmaX));
                    sum += m_kernelX[index];
                }
                for (float& f : m_kernelX)
                    f /= sum;
            }

            {
                m_kernelY[0] = 1.0f;
                float sum = 0.0f;
                for (size_t index = 1; index <= c_KernelRadiusMaxY; ++index)
                {
                    float x = float(index);
                    m_kernelY[index] = exp(-(x * x) / (2.0f * sigmaY * sigmaY));
                    sum += m_kernelY[index];
                }
                for (float& f : m_kernelY)
                    f /= sum;
            }

            {
                m_kernelZ[0] = 1.0f;
                float sum = 0.0f;
                for (size_t index = 1; index <= c_KernelRadiusMaxZ; ++index)
                {
                    float x = float(index);
                    m_kernelZ[index] = exp(-(x * x) / (2.0f * sigmaZ * sigmaZ));
                    sum += m_kernelZ[index];
                }
                for (float& f : m_kernelZ)
                    f /= sum;
            }

            {
                m_kernelW[0] = 1.0f;
                float sum = 0.0f;
                for (size_t index = 1; index <= c_KernelRadiusMaxW; ++index)
                {
                    float x = float(index);
                    m_kernelW[index] = exp(-(x * x) / (2.0f * sigmaW * sigmaW));
                    sum += m_kernelW[index];
                }
                for (float& f : m_kernelW)
                    f /= sum;
            }
        }

        BlueNoiseTexturesND Make()
        {
            // make the texture slices
            printf("creating 2Dx2D blue noise:  %zux%zux%zux%zu\n", dimsX, dimsY, dimsZ, dimsW);
            printf("  making initial binary pattern.\n");
            MakeInitialBinaryPattern();

            m_progress.ConstantTimeEnd();

            m_progress.SetLabel("  Phase 1: ");
            Phase1();
            m_progress.SetLabel("  Phase 2: ");
            Phase2();
            m_progress.SetLabel("  Phase 3: ");
            Phase3();

            // init the blue noise textures object
            BlueNoiseTexturesND textures;
            textures.Init({ dimsX, dimsY, dimsZ, dimsW }, { sigmaX, sigmaY, sigmaZ, sigmaW }, { 0, 0, 1, 1 });
            for (size_t pixelIndex = 0; pixelIndex < c_numPixels; ++pixelIndex)
            {
                Pixel& pixel = textures.pixels[pixelIndex];
                pixel.energy = 0.0f;
                pixel.on = true;
                pixel.rank = (int)m_pixelRank[pixelIndex];
            }

            // return them
            m_progress.SetLabel("  Done: ");
            m_progress.Show(-1);
            return textures;
        }

    private:

        typedef std::array<size_t, 4> PixelCoords;

        static PixelCoords PixelIndexToPixelCoords(size_t pixelIndex)
        {
            PixelCoords ret;

            ret[0] = (pixelIndex % (dimsX)) / (1);
            ret[1] = (pixelIndex % (dimsX * dimsY)) / (dimsX);
            ret[2] = (pixelIndex % (dimsX * dimsY * dimsZ)) / (dimsX * dimsY);
            ret[3] = (pixelIndex % (dimsX * dimsY * dimsZ * dimsY)) / (dimsX * dimsY * dimsZ);

            return ret;
        }

        static size_t PixelCoordsToPixelIndex(const PixelCoords& pixelCoords)
        {
            size_t ret = 0;

            ret += pixelCoords[3];
            ret *= dimsZ;
            ret += pixelCoords[2];
            ret *= dimsY;
            ret += pixelCoords[1];
            ret *= dimsX;
            ret += pixelCoords[0];

            return ret;
        }

        size_t GetTightestCluster()
        {
            size_t clusterPixelIndex = 0;
            float maxEnergy = -FLT_MAX;

            for (int pixelIndex = 0; pixelIndex < c_numPixels; ++pixelIndex)
            {
                if (!m_pixelOn[pixelIndex])
                    continue;

                if (m_energy[pixelIndex] > maxEnergy)
                {
                    maxEnergy = m_energy[pixelIndex];
                    clusterPixelIndex = pixelIndex;
                }
            }

            return clusterPixelIndex;
        }

        size_t GetLargestVoid()
        {
            size_t voidPixelIndex = 0;
            float minEnergy = FLT_MAX;

            for (int pixelIndex = 0; pixelIndex < c_numPixels; ++pixelIndex)
            {
                if (m_pixelOn[pixelIndex])
                    continue;

                if (m_energy[pixelIndex] < minEnergy)
                {
                    minEnergy = m_energy[pixelIndex];
                    voidPixelIndex = pixelIndex;
                }
            }

            return voidPixelIndex;
        }

        template <bool ON>
        void SplatEnergy(size_t pixelIndex)
        {
            const PixelCoords pixelCoords = PixelIndexToPixelCoords(pixelIndex);

            // splat the 2D gaussian across XY
            {
                size_t imageBegin = PixelCoordsToPixelIndex({ 0, 0, pixelCoords[2], pixelCoords[3] });
                for (int iy = c_KernelRadiusStartY; iy <= c_KernelRadiusEndY; ++iy)
                {
                    float kernelY = m_kernelY[abs(iy)];
                    int pixelY = ((int)pixelCoords[1] + iy + (int)dimsY) % (int)dimsY;
                    for (int ix = c_KernelRadiusStartX; ix < c_KernelRadiusEndX; ++ix)
                    {
                        float kernelX = m_kernelX[abs(ix)];
                        int pixelX = (int)(pixelCoords[0] + ix + (int)dimsX) % (int)dimsX;

                        if (ON)
                            m_energy[imageBegin + pixelY * dimsX + pixelX] += kernelX * kernelY;
                        else
                            m_energy[imageBegin + pixelY * dimsX + pixelX] -= kernelX * kernelY;
                    }
                }
            }

            // splat the 2D gaussian across ZW
            {
                for (int iw = c_KernelRadiusStartW; iw <= c_KernelRadiusEndY; ++iw)
                {
                    float kernelW = m_kernelW[abs(iw)];
                    int pixelW = ((int)pixelCoords[3] + iw + (int)dimsW) % (int)dimsW;
                    for (int iz = c_KernelRadiusStartZ; iz < c_KernelRadiusEndZ; ++iz)
                    {
                        float kernelZ = m_kernelZ[abs(iz)];
                        int pixelZ = (int)(pixelCoords[2] + iz + (int)dimsZ) % (int)dimsZ;

                        size_t pixelIndex = PixelCoordsToPixelIndex({ pixelCoords[0], pixelCoords[1], (size_t)pixelZ, (size_t)pixelW });

                        if (ON)
                            m_energy[pixelIndex] += kernelZ * kernelW;
                        else
                            m_energy[pixelIndex] -= kernelZ * kernelW;
                    }
                }
            }
        }

        void MakeInitialBinaryPattern()
        {
            // generate an initial set of on pixels, with a max density of c_initialBinaryPatternDensity
            // If we get duplicate random numbers, we won't get the full targetCount of on pixels, but that is ok.
            {
                printf("    Initializing to white noise\n");
                std::mt19937 rng = GetRNG();
                std::uniform_int_distribution<int> distPixel(0, (int)c_numPixels - 1);
                size_t targetCount = std::max(size_t(float(c_numPixels) * c_initialBinaryPatternDensity), (size_t)2);
                for (size_t index = 0; index < targetCount; ++index)
                {
                    size_t pixelIndex = distPixel(rng);
                    m_pixelOn[pixelIndex] = true;
                    SplatEnergy<true>(pixelIndex);
                }
            }

            // Make these into blue noise distributed points by removing the point at the tightest
            // cluster and placing it into the largest void. Repeat until those are the same location.
            printf("    reorganizing to blue noise\n");
            while (1)
            {
                size_t tightestClusterIndex = GetTightestCluster();
                m_pixelOn[tightestClusterIndex] = false;
                SplatEnergy<false>(tightestClusterIndex);

                size_t largestVoidIndex = GetLargestVoid();
                m_pixelOn[largestVoidIndex] = true;
                SplatEnergy<true>(largestVoidIndex);

                if (tightestClusterIndex == largestVoidIndex)
                    break;
            }
        }

        size_t GetOnesCount()
        {
            size_t ret = 0;
            for (bool b : m_pixelOn)
            {
                if (b)
                    ret++;
            }
            return ret;
        }

        void Phase1()
        {
            // Make the initial pattern progressive.
            // Find the tightest cluster and remove it. The rank for that pixel
            // is the number of ones left in the pattern.
            // Go until no more ones are in the texture.

            size_t onesCount = GetOnesCount();
            size_t initialOnesCount = onesCount;

            while (onesCount > 0)
            {
                size_t tightestClusterIndex = GetTightestCluster();

                onesCount--;

                m_pixelOn[tightestClusterIndex] = false;
                m_pixelRank[tightestClusterIndex] = onesCount;

                SplatEnergy<false>(tightestClusterIndex);

                m_progress.Show(int(initialOnesCount - onesCount));
            }

            // restore the "on" states
            for (int pixelIndex = 0; pixelIndex < c_numPixels; ++pixelIndex)
            {
                if (m_pixelRank[pixelIndex] < c_numPixels)
                {
                    m_pixelOn[pixelIndex] = true;
                    SplatEnergy<true>(pixelIndex);
                }
            }
        }

        void Phase2()
        {
            // Add new samples until half are ones.
            // Do this by repeatedly inserting a one into the largest void, and the
            // rank is the number of ones before you added it.

            size_t onesCount = GetOnesCount();
            const size_t targetOnesCount = c_numPixels / 2;

            while (onesCount < targetOnesCount)
            {
                size_t largestVoidIndex = GetLargestVoid();

                m_pixelOn[largestVoidIndex] = true;
                m_pixelRank[largestVoidIndex] = onesCount;

                SplatEnergy<true>(largestVoidIndex);

                onesCount++;

                m_progress.Show((int)onesCount);
            }
        }

        void Phase3()
        {
            // Reverse the meaning of zeros and ones.
            // Remove the tightest cluster and give it the rank of the number of zeros in the binary pattern before you removed it.
            // Go until there are no more ones
            std::fill(m_energy.begin(), m_energy.end(), 0.0f);
            for (size_t pixelIndex = 0; pixelIndex < c_numPixels; ++pixelIndex)
            {
                m_pixelOn[pixelIndex] = !m_pixelOn[pixelIndex];
                if (m_pixelOn[pixelIndex])
                    SplatEnergy<true>(pixelIndex);
            }

            size_t onesCount = GetOnesCount();

            while (onesCount > 0)
            {
                size_t tightestClusterIndex = GetTightestCluster();

                m_pixelOn[tightestClusterIndex] = false;
                m_pixelRank[tightestClusterIndex] = c_numPixels - onesCount;

                onesCount--;

                SplatEnergy<false>(tightestClusterIndex);

                m_progress.Show((int)(c_numPixels - onesCount));
            }
        }

    private:

        static double constexpr sqrtNewtonRaphson(double x, double curr, double prev)
        {
            return curr == prev
                ? curr
                : sqrtNewtonRaphson(x, 0.5 * (curr + x / curr), curr);
        }

        static double constexpr sqrtNewtonRaphson(double x)
        {
            return sqrtNewtonRaphson(x, x, 0.0);
        }

        static int constexpr KernelRadius(float sigma)
        {
            float energyLoss = 0.005f;
            float logEnergyLoss = -5.29831736655f;// log(energyLoss);  not a constexpr unfortunately!
            return int(sqrtNewtonRaphson(-2.0f * sigma * sigma * logEnergyLoss));
        }

        template <bool END>
        static int constexpr KernelRadius(float sigma, int dims)
        {
            int radius = KernelRadius(sigma);
            int start = -radius;
            int end = radius;
            int diameter = radius * 2 + 1;

            if (diameter > dims)
            {
                start = -dims / 2;
                end = start + dims;
            }

            if (END)
                return end;
            else
                return start;
        }

        static const int c_KernelRadiusStartX = KernelRadius<false>(sigmaX, dimsX);
        static const int c_KernelRadiusEndX = KernelRadius<true>(sigmaX, dimsX);
        static const int c_KernelRadiusMaxX = std::max(-c_KernelRadiusStartX, c_KernelRadiusEndX);

        static const int c_KernelRadiusStartY = KernelRadius<false>(sigmaY, dimsY);
        static const int c_KernelRadiusEndY = KernelRadius<true>(sigmaY, dimsY);
        static const int c_KernelRadiusMaxY = std::max(-c_KernelRadiusStartY, c_KernelRadiusEndY);

        static const int c_KernelRadiusStartZ = KernelRadius<false>(sigmaZ, dimsZ);
        static const int c_KernelRadiusEndZ = KernelRadius<true>(sigmaZ, dimsZ);
        static const int c_KernelRadiusMaxZ = std::max(-c_KernelRadiusStartZ, c_KernelRadiusEndZ);

        static const int c_KernelRadiusStartW = KernelRadius<false>(sigmaW, dimsW);
        static const int c_KernelRadiusEndW = KernelRadius<true>(sigmaW, dimsW);
        static const int c_KernelRadiusMaxW = std::max(-c_KernelRadiusStartW, c_KernelRadiusEndW);

        static const size_t c_numPixels = dimsX * dimsY * dimsZ * dimsW;

        std::vector<float> m_energy;
        std::vector<bool> m_pixelOn;
        std::vector<size_t> m_pixelRank;

        ProgressContext m_progress;

        std::array<float, c_KernelRadiusMaxX + 1> m_kernelX;
        std::array<float, c_KernelRadiusMaxY + 1> m_kernelY;
        std::array<float, c_KernelRadiusMaxZ + 1> m_kernelZ;
        std::array<float, c_KernelRadiusMaxW + 1> m_kernelW;
    };
};
