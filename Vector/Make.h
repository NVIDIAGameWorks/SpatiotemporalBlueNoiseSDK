/*
* Copyright (c) 2019-2021, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

#pragma once

#define DO_ENERGY_RECORD_CSV() false
#define MAKE_DEBUG_TEXTURES() false
#define OUTPUT_INITIAL() false

typedef std::array<float, 1> Vec1;
typedef std::array<float, 2> Vec2;
typedef std::array<float, 3> Vec3;

static const float c_pi = 3.14159265359f;
static const float c_twoPi = 2.0f * c_pi;

template <typename T, size_t N>
std::array<T, N>& operator +=(std::array<T, N>& A, const std::array<T, N>& B)
{
    for (size_t i = 0; i < N; ++i)
        A[i] += B[i];
    return A;
}

template <typename T, size_t N>
std::array<T, N>& operator -=(std::array<T, N>& A, const std::array<T, N>& B)
{
    for (size_t i = 0; i < N; ++i)
        A[i] -= B[i];
    return A;
}

template <typename T, size_t N>
std::array<T, N>& operator *=(std::array<T, N>& A, const std::array<T, N>& B)
{
    for (size_t i = 0; i < N; ++i)
        A[i] *= B[i];
    return A;
}

template <typename T, size_t N>
std::array<T, N>& operator /=(std::array<T, N>& A, const std::array<T, N>& B)
{
    for (size_t i = 0; i < N; ++i)
        A[i] *= B[i];
    return A;
}

template <size_t N>
std::array<float, N> operator *(const std::array<float, N>& v, float f)
{
    std::array<float, N> ret;
    for (size_t i = 0; i < N; ++i)
        ret[i] = v[i] * f;
    return ret;
}

template <size_t N>
std::array<float, N> operator +(const std::array<float, N>& v, float f)
{
    std::array<float, N> ret;
    for (size_t i = 0; i < N; ++i)
        ret[i] = v[i] + f;
    return ret;
}

template <typename T, size_t N>
std::array<T, N> operator +(const std::array<T, N>& A, const std::array<T, N>& B)
{
    std::array<T, N> ret;
    for (size_t i = 0; i < N; ++i)
        ret[i] = A[i] + B[i];
    return ret;
}

template <typename T, size_t N>
std::array<T, N> operator -(const std::array<T, N>& A, const std::array<T, N>& B)
{
    std::array<T, N> ret;
    for (size_t i = 0; i < N; ++i)
        ret[i] = A[i] - B[i];
    return ret;
}

template <size_t N>
float Dot(const std::array<float, N>& A, const std::array<float, N>& B)
{
    float ret = 0.0f;
    for (size_t i = 0; i < N; ++i)
        ret += A[i] * B[i];
    return ret;
}

template <size_t N>
float Length(const std::array<float, N>& A)
{
    return sqrtf(Dot(A, A));
}

Vec3 Vec2ToUnitVec3(const Vec2 rnd)
{
    float z = rnd[0] * 2.0f - 1.0f;
    float t = rnd[1] * c_twoPi;
    float r = (float)sqrtf(1.0f - z * z);
    float x = r * (float)cos(t);
    float y = r * (float)sin(t);
    return Vec3{ x, y, z };
}

#include "MakeBase.h"

// https://en.wikipedia.org/wiki/Equirectangular_projection
// https://stackoverflow.com/a/33790309
// https://learnopengl.com/PBR/IBL/Diffuse-irradiance
inline Vec2 SphericalMapDirectionToUV(Vec3 direction)
{
    float pitch = asin(-direction[1]);
    float yaw = atan2(direction[2], direction[0]);

    Vec2 uv = Vec2{ yaw, pitch };
    uv *= Vec2{ 0.1591f, 0.3183f };
    uv += Vec2{ 0.5f, 0.5f };
    return uv;
}

inline Vec3 SphericalMapUVToDirection(Vec2 uv)
{
    uv -= Vec2{ 0.5f, 0.5f };
    uv /= Vec2{ 0.1591f, 0.3183f };

    float yaw = uv[0];
    float pitch = uv[1];

    Vec3 ret;
    ret[1] = -(sin(pitch));
    ret[0] = cos(yaw) * cos(pitch);
    ret[2] = sin(yaw) * cos(pitch);

    float len = Length(ret);

    return ret;
}

std::string MakeDurationString(float durationInSeconds)
{
    std::string ret;

    static const float c_oneMinute = 60.0f;
    static const float c_oneHour = c_oneMinute * 60.0f;

    int hours = int(durationInSeconds / c_oneHour);
    durationInSeconds -= float(hours) * c_oneHour;

    int minutes = int(durationInSeconds / c_oneMinute);
    durationInSeconds -= float(minutes) * c_oneMinute;

    int seconds = int(durationInSeconds);

    char buffer[1024];
    if (hours < 10)
        sprintf_s(buffer, "0%i:", hours);
    else
        sprintf_s(buffer, "%i:", hours);
    ret = buffer;

    if (minutes < 10)
        sprintf_s(buffer, "0%i:", minutes);
    else
        sprintf_s(buffer, "%i:", minutes);
    ret += buffer;

    if (seconds < 10)
        sprintf_s(buffer, "0%i", seconds);
    else
        sprintf_s(buffer, "%i", seconds);
    ret += buffer;

    return ret;
}

bool ReportProgress(int& lastPercent, size_t swapCount, size_t swapsTotal, std::chrono::high_resolution_clock::time_point& timeStart, int& lastBufferLength, float avgRollCount)
{
    float percentF = 100.0f * float(double(swapCount) / double(swapsTotal));
    int percentx10 = int(percentF*10.0f);

    if (lastPercent == percentx10)
        return false;
    lastPercent = percentx10;

    // get time since start
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(now - timeStart);

    std::string elapsed = MakeDurationString((float)timeSpan.count());

    float estimateMultiplier = std::max(100.0f / percentF, 1.0f);
    std::string estimate = MakeDurationString(estimateMultiplier * (float)timeSpan.count());

    // make the message
    char buffer[1024];
    if (swapCount == swapsTotal)
    {
        if (avgRollCount != 1.0f)
            sprintf_s(buffer, "\r100%%  elapsed %s (%0.2f avg rolls)", elapsed.c_str(), avgRollCount);
        else
            sprintf_s(buffer, "\r100%%  elapsed %s", elapsed.c_str());
    }
    else
    {
        if (avgRollCount != 1.0f)
            sprintf_s(buffer, "\r%0.1f%%  elapsed %s  estimated %s (%0.2f avg rolls)", float(percentx10) / 10.0f, elapsed.c_str(), estimate.c_str(), avgRollCount);
        else
            sprintf_s(buffer, "\r%0.1f%%  elapsed %s  estimated %s", float(percentx10) / 10.0f, elapsed.c_str(), estimate.c_str());
    }

    int newBufferLength = (int)strlen(buffer);
    int newBufferLengthCopy = newBufferLength;

    // pad with spaces to erase whatever may be there from before
    while (newBufferLength < lastBufferLength)
    {
        buffer[newBufferLength] = ' ';
        newBufferLength++;
    }
    buffer[newBufferLength] = 0;
    if (swapCount < 0)
        strcat_s(buffer, "\n");

    // show the message
    printf("%s", buffer);
    lastBufferLength = newBufferLengthCopy;

    return true;
}

template <typename TMAKER>
void Swap(TMAKER& maker, uint32_t indexA, uint32_t indexB)
{
    typename TMAKER::TVec A = maker.m_cells[indexA];
    typename TMAKER::TVec B = maker.m_cells[indexB];
    float PDFA = maker.m_pdf[indexA];
    float PDFB = maker.m_pdf[indexB];

    maker.SplatEnergy<false>(indexA);
    maker.SplatEnergy<false>(indexB);

    maker.m_cells[indexA] = B;
    maker.m_cells[indexB] = A;

    maker.m_pdf[indexA] = PDFB;
    maker.m_pdf[indexB] = PDFA;

    maker.SplatEnergy<true>(indexA);
    maker.SplatEnergy<true>(indexB);

    // The energy that other pixels give to indexA and indexB are based
    // on the vector at indexA and indexB so we need to recalculate
    // these energy cells completely.
    maker.RecalculateEnergy(indexA);
    maker.RecalculateEnergy(indexB);
}

template <typename TMAKER>
void MakeMask(float swapCountFactor, float coolingFactor, const char* baseFileName, const char* importanceMap = nullptr, bool importanceMapIsSRGB = false)
{
    TMAKER maker;

    const size_t c_numSwaps = std::max(size_t(swapCountFactor * float(TMAKER::c_numPixels) * float(TMAKER::c_numPixels)), size_t(1)); // how many swaps per trial
    const float c_coolingRate = 1.0f / (coolingFactor * float(TMAKER::c_numPixels) * float(TMAKER::c_numPixels)); // how much the temperature cools each iteration

    // initial report
    printf("%s_%ix%ix%i\n", baseFileName, TMAKER::c_width, TMAKER::c_height, TMAKER::c_depth);
    printf("%zu swaps total, simulated annealing for %zu swaps\n", c_numSwaps, size_t(1.0f / c_coolingRate));
    std::chrono::high_resolution_clock::time_point timeStart = std::chrono::high_resolution_clock::now();

    // load or create importance map
    pcg32_random_t rng = GetRNG();
    float isavg = 1.0f;
    std::vector<float> isf;
    int isw = 0;
    int ish = 0;
    float lummax = 0.0f;
    if (importanceMap)
    {
        if (!importanceMapIsSRGB)
            stbi_hdr_to_ldr_gamma(1.0f);

        int isc;
        float* isfraw = stbi_loadf(importanceMap, &isw, &ish, &isc, 0);

        if (!importanceMapIsSRGB)
            stbi_hdr_to_ldr_gamma(2.2f);

        // Convert to greyscale and get the max value
        isf.resize(isw * ish);
        for (int index = 0; index < isw * ish; ++index)
        {
            float lum;

            if (isc == 1)
                lum = isfraw[index * isc];
            else
            {
                lum =
                    isfraw[index * isc + 0] * 0.3f +
                    isfraw[index * isc + 1] * 0.59f +
                    isfraw[index * isc + 2] * 0.11f;
            }
            lummax = std::max(lummax, lum);
            isf[index] = lum;
        }

        // scale to 1.0 max and keep track of isavg
        for (int index = 0; index < isw * ish; ++index)
        {
            isf[index] /= lummax;
            isavg = Lerp(isavg, isf[index], 1.0f / float(index + 1));
        }

        stbi_image_free(isfraw);
    }

    TMAKER::GenerateStartingTexture(maker, rng, importanceMap != nullptr, isf, isw, ish, isavg, baseFileName);

    // write out the pdf min and max
    float pdfmin = FLT_MAX;
    float pdfmax = -FLT_MAX;
    {
        for (float f : maker.m_pdf)
        {
            pdfmin = std::min(pdfmin, f);
            pdfmax = std::max(pdfmax, f);
        }

        char fileName[1024];
        sprintf_s(fileName, "%s_%ix%ix%i.dat", baseFileName, TMAKER::c_width, TMAKER::c_height, TMAKER::c_depth);
        FILE* file = nullptr;
        fopen_s(&file, fileName, "w+b");
        fwrite(&pdfmin, sizeof(float), 1, file);
        fwrite(&pdfmax, sizeof(float), 1, file);
        fwrite(&isavg, sizeof(float), 1, file);
        fwrite(&lummax, sizeof(float), 1, file);
        fclose(file);
    }

    // Show UV and dirs of texture
    if (importanceMap && MAKE_DEBUG_TEXTURES())
    {
        std::vector<unsigned char> output_uv(isw * ish * 4, 0);
        std::vector<unsigned char> output_dir(isw * ish * 4, 0);
        for (size_t index = 0; index < isw * ish; ++index)
        {
            int ix = int(index % isw);
            int iy = int(index / isw);

            float uvx = (float(ix) + 0.5f) / float(isw);
            float uvy = (float(iy) + 0.5f) / float(ish);

            output_uv[index * 4 + 0] = (unsigned char)Clamp(uvx * 256.0f, 0.0f, 255.0f);
            output_uv[index * 4 + 1] = (unsigned char)Clamp(uvy * 256.0f, 0.0f, 255.0f);
            output_uv[index * 4 + 2] = 0;
            output_uv[index * 4 + 3] = 255;

            Vec3 dir = SphericalMapUVToDirection(Vec2{ uvx, uvy });
            output_dir[index * 4 + 0] = (unsigned char)Clamp(dir[0] * 256.0f, 0.0f, 255.0f);
            output_dir[index * 4 + 1] = (unsigned char)Clamp(dir[1] * 256.0f, 0.0f, 255.0f);
            output_dir[index * 4 + 2] = (unsigned char)Clamp(dir[2] * 256.0f, 0.0f, 255.0f);
            output_dir[index * 4 + 3] = 255;
        }

        char fileName[1024];
        sprintf_s(fileName, "%s_debuguv_%ix%ix%i.png", baseFileName, TMAKER::c_width, TMAKER::c_height, TMAKER::c_depth);
        stbi_write_png(fileName, isw, ish, 4, output_uv.data(), 0);

        sprintf_s(fileName, "%s_debugdir_%ix%ix%i.png", baseFileName, TMAKER::c_width, TMAKER::c_height, TMAKER::c_depth);
        stbi_write_png(fileName, isw, ish, 4, output_dir.data(), 0);
    }

    // convert and save initial images.
    // Useful for comparing white vs blue importance sampling.
    // Also useful for comparing warm started textures vs what they warm started from.
    {
        // convert image
        std::vector<unsigned char> output(TMAKER::c_numPixels * 4, 0);
        for (size_t index = 0; index < TMAKER::c_numPixels; ++index)
        {
            TMAKER::Convert(maker.m_cells[index], &output[index * 4]);

            float pdfpercent = 1.0f;
            if (pdfmax != pdfmin)
                pdfpercent = (maker.m_pdf[index] - pdfmin) / (pdfmax - pdfmin);

            output[index * 4 + 3] = (unsigned char)Clamp(pdfpercent * 256.0f, 0.0f, 255.0f);
        }

        #if OUTPUT_INITIAL()
            char initialBaseFileName[1024];
            sprintf_s(initialBaseFileName, "%s_initial", baseFileName);

            // save PNG
            for (int i = 0; i < TMAKER::c_depth; ++i)
            {
                char fileName[1024];
                sprintf_s(fileName, "%s_%ix%ix%i_%i.png", initialBaseFileName, TMAKER::c_width, TMAKER::c_height, TMAKER::c_depth, i);
                stbi_write_png(fileName, TMAKER::c_width, TMAKER::c_height, 4, &output[i * TMAKER::c_width * TMAKER::c_height * 4], 0);
            }

            // run the python script to make DFTs and such
            {
                char buffer[1024];
                sprintf_s(buffer, "python ../analyzeVVSTBN.py %s_%ix%ix%i %i", initialBaseFileName, TMAKER::c_width, TMAKER::c_height, TMAKER::c_depth, TMAKER::c_depth);
                printf("\n");
                printf(buffer);
                system(buffer);
                printf("\n\n");
            }
        #endif
    }

    // calculate starting energy
    for (size_t index = 0; index < TMAKER::c_numPixels; ++index)
        maker.SplatEnergy<true>(index);

    float energy = maker.Energy();

#if DO_ENERGY_RECORD_CSV()
    std::vector<float> energyRecord;
    energyRecord.push_back(energy / float(TMAKER::c_numPixels));
#endif

    // perform the swaps
    int lastBufferLength = 0;
    float avgRollCount = 0.0f;
    {
        int lastPercent = -1;
        size_t swapsDone = 0;
        float temperature = 1.0f;
        for (size_t swapIndex = 0; swapIndex < c_numSwaps; ++swapIndex)
        {
            temperature = std::max(temperature - c_coolingRate, 0.0f);

            // Do a random swap
            uint32_t indexA;
            uint32_t indexB;
            int rollCount = 0;
            do
            {
                rollCount++;
                indexA = pcg32_boundedrand_r(&rng, TMAKER::c_numPixels);
                indexB = pcg32_boundedrand_r(&rng, TMAKER::c_numPixels);
            }             while (!maker.CanSwap(indexA, indexB));
            avgRollCount = Lerp(avgRollCount, float(rollCount), 1.0f / float(swapIndex + 1));

            Swap(maker, indexA, indexB);

            // Keep this swap if it was an improvement, or the simulated annealing says we should take it
            float swapEnergy = maker.Energy();
            if (swapEnergy <= energy || RandomFloat01(rng) < temperature)
            {
                energy = swapEnergy;
            }
            // else, revert the swap by swapping again
            else
            {
                Swap(maker, indexA, indexB);
            }

            swapsDone++;
            if (ReportProgress(lastPercent, swapsDone, c_numSwaps, timeStart, lastBufferLength, avgRollCount))
            {
#if DO_ENERGY_RECORD_CSV()
                if (lastPercent % 10 == 0)
                    energyRecord.push_back(energy);
#endif
            }
        }
    }

    // make CSV of energy records
#if DO_ENERGY_RECORD_CSV()
    {
        FILE* file = nullptr;
        char fileName[1024];
        sprintf_s(fileName, "%s_%ix%ix%i.energy.csv", baseFileName, TMAKER::c_width, TMAKER::c_height, TMAKER::c_depth);
        fopen_s(&file, fileName, "wt");
        for (float f : energyRecord)
            fprintf(file, "\"%f\"\n", f);
        fclose(file);
    }
#endif

    // convert and save image
    {
        // convert image
        std::vector<unsigned char> output(TMAKER::c_numPixels * 4, 0);
        for (size_t index = 0; index < TMAKER::c_numPixels; ++index)
        {
            TMAKER::Convert(maker.m_cells[index], &output[index * 4]);

            float pdfpercent = 1.0f;
            if (pdfmax != pdfmin)
                pdfpercent = (maker.m_pdf[index] - pdfmin) / (pdfmax - pdfmin);

            output[index * 4 + 3] = (unsigned char)Clamp(pdfpercent * 256.0f, 0.0f, 255.0f);
        }

        // save PNG
        for (int i = 0; i < TMAKER::c_depth; ++i)
        {
            char fileName[1024];
            sprintf_s(fileName, "%s_%ix%ix%i_%i.png", baseFileName, TMAKER::c_width, TMAKER::c_height, TMAKER::c_depth, i);
            stbi_write_png(fileName, TMAKER::c_width, TMAKER::c_height, 4, &output[i * TMAKER::c_width * TMAKER::c_height * 4], 0);
        }
    }

    // report completion and execution time
    int dummy = 0;
    ReportProgress(dummy, dummy, c_numSwaps, timeStart, lastBufferLength, avgRollCount);

    // run the python script to make DFTs and such
    {
        char buffer[1024];
        sprintf_s(buffer, "python ../analyzeVVSTBN.py %s_%ix%ix%i %i", baseFileName, TMAKER::c_width, TMAKER::c_height, TMAKER::c_depth, TMAKER::c_depth);
        printf("\n");
        printf(buffer);
        system(buffer);
        printf("\n\n");
    }
}

template <int dimsX, int dimsY, int dimsZ, float sigmaX, float sigmaY, float sigmaZ, float sigmaValue>
struct Make_Float : public MakeBase<1, dimsX, dimsY, dimsZ, sigmaX, sigmaY, sigmaZ, sigmaValue, true, true, false>
{
    static void Generate(Vec1& v, pcg32_random_t& rng)
    {
        v[0] = RandomFloat01(rng) * 2.0f - 1.0f;
    }

    static void Convert(const Vec1& in, unsigned char* out)
    {
        out[0] = (unsigned char)Clamp((in[0] * 0.5f + 0.5f) * 256.0f, 0.0f, 255.0f);
    }
};

template <int dimsX, int dimsY, int dimsZ, float sigmaX, float sigmaY, float sigmaZ, float sigmaValue>
struct Make_UnitFloat : public MakeBase<1, dimsX, dimsY, dimsZ, sigmaX, sigmaY, sigmaZ, sigmaValue, true, true, false>
{
    static void Generate(Vec1& v, pcg32_random_t& rng)
    {
        v[0] = (RandomFloat01(rng) <= 0.5f) ? -1.0f : 1.0f;
    }

    static void Convert(const Vec1& in, unsigned char* out)
    {
        out[0] = (unsigned char)Clamp((in[0] * 0.5f + 0.5f) * 256.0f, 0.0f, 255.0f);
    }
};

template <int dimsX, int dimsY, int dimsZ, float sigmaX, float sigmaY, float sigmaZ, float sigmaValue>
struct Make_Float2 : public MakeBase<2, dimsX, dimsY, dimsZ, sigmaX, sigmaY, sigmaZ, sigmaValue, true, true, false>
{
    static void Generate(Vec2& v, pcg32_random_t& rng)
    {
        v[0] = RandomFloat01(rng) * 2.0f - 1.0f;
        v[1] = RandomFloat01(rng) * 2.0f - 1.0f;
    }

    static void Convert(const Vec2& in, unsigned char* out)
    {
        out[0] = (unsigned char)Clamp((in[0] * 0.5f + 0.5f) * 256.0f, 0.0f, 255.0f);
        out[1] = (unsigned char)Clamp((in[1] * 0.5f + 0.5f) * 256.0f, 0.0f, 255.0f);
    }
};

template <int dimsX, int dimsY, int dimsZ, float sigmaX, float sigmaY, float sigmaZ, float sigmaValue>
struct Make_UnitFloat2 : public MakeBase<2, dimsX, dimsY, dimsZ, sigmaX, sigmaY, sigmaZ, sigmaValue, true, true, false>
{
    static void Generate(Vec2& v, pcg32_random_t& rng)
    {
        float angle = RandomFloat01(rng) * c_pi * 2.0f;
        v = Vec2{ sin(angle), cos(angle) };
    }

    static void Convert(const Vec2& in, unsigned char* out)
    {
        out[0] = (unsigned char)Clamp((in[0] * 0.5f + 0.5f) * 256.0f, 0.0f, 255.0f);
        out[1] = (unsigned char)Clamp((in[1] * 0.5f + 0.5f) * 256.0f, 0.0f, 255.0f);
    }
};

template <int dimsX, int dimsY, int dimsZ, float sigmaX, float sigmaY, float sigmaZ, float sigmaValue>
struct Make_Float3 : public MakeBase<3, dimsX, dimsY, dimsZ, sigmaX, sigmaY, sigmaZ, sigmaValue, true, true, false>
{
    static void Generate(Vec3& v, pcg32_random_t& rng)
    {
        v[0] = RandomFloat01(rng) * 2.0f - 1.0f;
        v[1] = RandomFloat01(rng) * 2.0f - 1.0f;
        v[2] = RandomFloat01(rng) * 2.0f - 1.0f;
    }

    static void Convert(const Vec3& in, unsigned char* out)
    {
        out[0] = (unsigned char)Clamp((in[0] * 0.5f + 0.5f) * 256.0f, 0.0f, 255.0f);
        out[1] = (unsigned char)Clamp((in[1] * 0.5f + 0.5f) * 256.0f, 0.0f, 255.0f);
        out[2] = (unsigned char)Clamp((in[2] * 0.5f + 0.5f) * 256.0f, 0.0f, 255.0f);
    }
};

template <int dimsX, int dimsY, int dimsZ, float sigmaX, float sigmaY, float sigmaZ, float sigmaValue>
struct Make_UnitFloat3 : public MakeBase<3, dimsX, dimsY, dimsZ, sigmaX, sigmaY, sigmaZ, sigmaValue, true, true, false>
{
    static void Generate(Vec3& v, pcg32_random_t& rng)
    {
        v = Vec2ToUnitVec3(Vec2{ RandomFloat01(rng), RandomFloat01(rng) });;
    }

    static void Convert(const Vec3& in, unsigned char* out)
    {
        out[0] = (unsigned char)Clamp((in[0] * 0.5f + 0.5f) * 256.0f, 0.0f, 255.0f);
        out[1] = (unsigned char)Clamp((in[1] * 0.5f + 0.5f) * 256.0f, 0.0f, 255.0f);
        out[2] = (unsigned char)Clamp((in[2] * 0.5f + 0.5f) * 256.0f, 0.0f, 255.0f);
    }

    static void GenerateStartingTexture(Make_UnitFloat3& maker, pcg32_random_t& rng, bool importanceSampling, std::vector<float>& isf, int isw, int ish, float isavg, const char* baseFileName)
    {
        // if we have a 2d importance sampling map, importance sample it as normal to get a 2d non unit vector per pixel.
        // Then, treat those 2d non unit vectors as if they come from a spherical map, and calculate the 3d unit vector they go with, as the actual pixel value.
        if (importanceSampling)
        {
            #if MAKE_DEBUG_TEXTURES()
                std::vector<int> Histogram2DInt(isw * ish, 0);
            #endif

            for (size_t index = 0; index < Make_UnitFloat3::c_numPixels; ++index)
            {
                while (1)
                {
                    float uvx = RandomFloat01(rng);
                    float uvy = RandomFloat01(rng);

                    int isx = Clamp(int(uvx * float(isw)), 0, isw - 1);
                    int isy = Clamp(int(uvy * float(ish)), 0, ish - 1);

                    float isvalue = isf[isy * isw + isx];
                    float randValue = RandomFloat01(rng);
                    if (randValue < isvalue)
                    {
                        #if MAKE_DEBUG_TEXTURES()
                            Histogram2DInt[isy * isw + isx]++;
                        #endif

                        maker.m_cells[index] = SphericalMapUVToDirection(Vec2{ uvx, uvy });
                        maker.m_pdf[index] = isvalue / isavg;
                        break;
                    }
                }
            }

            #if MAKE_DEBUG_TEXTURES()
                {
                    int maxValue = 0;
                    for (int i : Histogram2DInt)
                        maxValue = std::max(i, maxValue);

                    std::vector<unsigned char> Histogram2D(isw * ish, 0);
                    for (size_t index = 0; index < Histogram2D.size(); ++index)
                    {
                        float percent = float(Histogram2DInt[index]) / float(maxValue);
                        Histogram2D[index] = (unsigned char)Clamp(percent * 256.0f, 0.0f, 255.0f);
                    }

                    char fileName[1024];
                    sprintf_s(fileName, "%s_debughist2d_%ix%ix%i.png", baseFileName, TSuper::c_width, TSuper::c_height, TSuper::c_depth);
                    stbi_write_png(fileName, isw, ish, 1, Histogram2D.data(), 0);
                }
            #endif
        }
        // If we don't have an importance sampling map, just do uniform random (white noise) unit vec3s.
        else
        {
            for (size_t index = 0; index < Make_UnitFloat3::c_numPixels; ++index)
            {
                Make_UnitFloat3::Generate(maker.m_cells[index], rng);
                maker.m_pdf[index] = 1.0f;
            }
        }
    }
};

template <int dimsX, int dimsY, int dimsZ, float sigmaX, float sigmaY, float sigmaZ, float sigmaValue>
struct Make_UnitFloat3_CosineWeightedHemisphere : public MakeBase<3, dimsX, dimsY, dimsZ, sigmaX, sigmaY, sigmaZ, sigmaValue, true, true, false>
{
    static void Generate(Vec3& v, pcg32_random_t& rng)
    {
        v = Vec2ToUnitVec3(Vec2{ RandomFloat01(rng), RandomFloat01(rng) });;
    }

    static void Convert(const Vec3& in, unsigned char* out)
    {
        out[0] = (unsigned char)Clamp((in[0] * 0.5f + 0.5f) * 256.0f, 0.0f, 255.0f);
        out[1] = (unsigned char)Clamp((in[1] * 0.5f + 0.5f) * 256.0f, 0.0f, 255.0f);
        out[2] = (unsigned char)Clamp((in[2] * 0.5f + 0.5f) * 256.0f, 0.0f, 255.0f);
    }

    static void GenerateStartingTexture(Make_UnitFloat3_CosineWeightedHemisphere& maker, pcg32_random_t& rng, bool importanceSampling, std::vector<float>& isf, int isw, int ish, float isavg, const char* baseFileName)
    {
        for (size_t index = 0; index < Make_UnitFloat3_CosineWeightedHemisphere::c_numPixels; ++index)
        {
            float theta = RandomFloat01(rng) * 2.0f * c_pi;
            float radius = sqrtf(RandomFloat01(rng));

            float x = cos(theta) * radius;
            float y = sin(theta) * radius;
            float z = sqrtf(1.0f - x * x - y * y);

            maker.m_cells[index] = Vec3{ x, y, z };
            maker.m_pdf[index] = Dot(maker.m_cells[index], Vec3{ 0.0f, 0.0f, 1.0f });
        }
    }
};
