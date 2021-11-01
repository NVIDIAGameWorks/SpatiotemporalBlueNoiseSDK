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

// If true, will use the same random numbers each run
#define DETERMINISTIC() false

#include <chrono>
#include <string>
#include <vector>
#include <unordered_set>
#include <array>
#include <random>

#include "../Shared/pcg/pcg_basic.h"

// STB for image read/write
#define _CRT_SECURE_NO_WARNINGS
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

template <typename T>
T Clamp(const T& value, const T& minimum, const T& maximum)
{
    if (value <= minimum)
        return minimum;
    else if (value >= maximum)
        return maximum;
    else
        return value;
}

template <typename T>
T Lerp(const T& A, const T& B, float t)
{
    return A * (1.0f - t) + B * t;
}

// A helper for reporting progress over time
struct ProgressContext
{
    ProgressContext(int total)
    {
        m_start = std::chrono::high_resolution_clock::now();
        m_total = total;
    }

    std::chrono::high_resolution_clock::time_point m_start;
    int m_total;
    int m_lastPercent = -1;
    int m_lastBufferLength = 0;
    const char* m_label = "";
    float m_constantTimeSeconds = 0.0f;

    void ConstantTimeEnd()
    {
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(now - m_start);

        m_constantTimeSeconds = (float)timeSpan.count();
        m_start = now;
    }

    void SetLabel(const char* label)
    {
        m_label = label;
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

    void Show(int count)
    {
        // only update if the percent has changed
        int percent = int(100.0f * float(count) / float(m_total));
        if (m_lastPercent == percent)
            return;
        m_lastPercent = percent;

        // get time since start
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(now - m_start);

        std::string elapsed = MakeDurationString(m_constantTimeSeconds + (float)timeSpan.count());

        float estimateMultiplier = std::max(float(m_total) / float(count), 1.0f);
        std::string estimate = MakeDurationString(m_constantTimeSeconds + estimateMultiplier * (float)timeSpan.count());

        // make the message
        char buffer[1024];
        if (count < 0)
        {
            sprintf_s(buffer, "\r%s100%%  elapsed %s", m_label, elapsed.c_str());
        }
        else
        {
            sprintf_s(buffer, "\r%s%i%%  elapsed %s  estimated %s", m_label, percent, elapsed.c_str(), estimate.c_str());
        }
        int newBufferLength = (int)strlen(buffer);
        int newBufferLengthCopy = newBufferLength;

        // pad with spaces to erase whatever may be there from before
        while (newBufferLength < m_lastBufferLength)
        {
            buffer[newBufferLength] = ' ';
            newBufferLength++;
        }
        buffer[newBufferLength] = 0;
        if (count < 0)
            strcat_s(buffer, "\n");

        // show the message
        printf("%s", buffer);
        m_lastBufferLength = newBufferLengthCopy;
    }
};

struct Pixel
{
    bool on = false;
    int rank = -1;
    float energy = 0.0f;
};

struct BlueNoiseTexturesND
{
    std::vector<int> dims;
    std::vector<float> sigmas;
    std::vector<Pixel> pixels;
    std::vector<unsigned int> masks;
    std::vector<int> energyEmitMin;
    std::vector<int> energyEmitMax;

    /*
    Single Mask:
    A single mask is a bit mask with a bit per dimension.
    A 1 bit means that two points must match in that dimension to affect each other in the energy field.
    A 0 bit means they don't have to match, but they can

    Multiple Masks:
    If any mask is satisfied, the two points affect each other in the energy field

    Example: 1Dx1D blue noise
    1Dx1D blue noise is {0,1}, making masks: 2,1.
    This means that if A.y == B.y or A.x == B.x, then they affect each other, else they don't.

    Example: 2D blue noise
    2d blue noise is {0, 0} making mask: 0
    This means that no match is required, all points affect each other.

    Example: 2Dx1D blue noise
    2Dx1D blue noise is {0,0,1} making masks: 4, 3
    This means that if A.z == B.z  or A.xy == B.xy, then they affect each other, else they don't.

    Example: 1Dx1Dx1D blue noise
    This is {0,1,2} making masks: 6, 5, 3
    If A.yz == B.yz or A.xz == B.xz or A.xy == B.xy, then they affect each other, else they don't.
    */

    void Init(const std::vector<int>& _dims, const std::vector<float>& _sigmas, std::vector<int> groups)
    {
        dims = _dims;
        sigmas = _sigmas;
        int pixelCount = 1;
        for (int i : dims)
        {
            pixelCount *= i;
        }
        pixels.clear();
        pixels.resize(pixelCount);

        // make the masks
        if (groups.size() == 0)
            groups.resize(dims.size(), 0);
        std::unordered_set<int> uniqueGroupNumbers;
        for (int g : groups)
            uniqueGroupNumbers.insert(g);
        for (int group : uniqueGroupNumbers)
        {
            unsigned int mask = 0;
            for (int index = 0; index < dims.size(); ++index)
            {
                if (groups[index] != group)
                    mask |= (1 << index);
            }
            masks.push_back(mask);
        }

        // calculate energy radii so we don't have to consider all pixels for energy changes
        energyEmitMin.resize(dims.size());
        energyEmitMax.resize(dims.size());
        static const float c_energyLoss = 0.005f; // 0.5% lost, so 99.5% accounted for
        for (int i = 0; i < dims.size(); ++i)
        {
            int radius = int(sqrtf(-2.0f * sigmas[i] * sigmas[i] * log(c_energyLoss)));

            if (radius * 2 + 1 >= dims[i])
            {
                radius = (dims[i] - 1) / 2;

                energyEmitMin[i] = -radius;
                energyEmitMax[i] = -radius + dims[i] - 1;
            }
            else
            {
                energyEmitMin[i] = -radius;
                energyEmitMax[i] = radius;
            }
        }
    }

    // pattern of calculating pixel index
    // x
    // y * width + x
    // z * width * height + y * width + x
    // w * width * height * depth + z * width * height + y * width + x
    // ...

    int GetPixelIndex(const std::vector<int>& indices) const
    {
        if (indices.size() != dims.size())
        {
            ((int*)0)[0] = 0;
        }

        int pixelIndex = 0;
        int multiplier = 1;

        for (int i = 0; i < dims.size(); ++i)
        {
            pixelIndex += multiplier * indices[i];
            multiplier *= dims[i];
        }

        return pixelIndex;
    }

    Pixel& GetPixel(const std::vector<int>& indices)
    {
        return pixels[GetPixelIndex(indices)];
    }

    const Pixel& GetPixel(const std::vector<int>& indices) const
    {
        return pixels[GetPixelIndex(indices)];
    }

    // pattern of calculating pixel indices
    // x = (index % width) / 1
    // y = (index % width*height) / width
    // z = (index % width*height*depth) / width*height
    // w = (index % width*height*depth*other) / width*height*depth
    // ...
    // C = (index % A) / B

    void GetPixelIndices(int index, std::vector<int>& indices) const
    {
        int A = 1;
        int B = 1;

        indices.resize(dims.size());
        for (int i = 0; i < dims.size(); ++i)
        {
            A *= dims[0];
            indices[i] = (index % A) / B;
            B = A;
        }
    }
};


inline pcg32_random_t GetRNG()
{
    pcg32_random_t rng;
#if DETERMINISTIC()
    pcg32_srandom_r(&rng, 0x1337FEED, 0);
#else
    std::random_device device;
    std::mt19937 generator(device());
    std::uniform_int_distribution<uint32_t> dist;
    pcg32_srandom_r(&rng, dist(generator), 0);
#endif
    return rng;
}

inline float RandomFloat01(pcg32_random_t& rng)
{
    return float(pcg32_random_r(&rng)) / 4294967295.0f;
}

inline void SaveTextures(const BlueNoiseTexturesND& textures, const char* fileNamePattern)
{
    // turn ranks into pixel values
    std::vector<unsigned char> pixels(textures.pixels.size());
    for (size_t index = 0; index < pixels.size(); ++index)
    {
        unsigned char* dest = &pixels[index];
        const Pixel& src = textures.pixels[index];

        float rankPercent = float(src.rank) / float(pixels.size());
        *dest = (unsigned char)Clamp(rankPercent * 256.0f, 0.0f, 255.0f);
    }

    // save images
    {
        char fileName[1024];

        // 1D images are just a single strip
        if (textures.dims.size() == 1)
        {
            sprintf_s(fileName, fileNamePattern, 0);
            stbi_write_png(fileName, textures.dims[0], 1, 1, pixels.data(), 0);
        }
        // otherwise save x/y images. the other dimensions are flattened into 1d.
        else
        {
            int pixelStride = textures.dims[0] * textures.dims[1];
            int pixelStart = 0;
            int imageIndex = 0;
            while (pixelStart < textures.pixels.size())
            {
                sprintf_s(fileName, fileNamePattern, imageIndex);
                stbi_write_png(fileName, textures.dims[0], textures.dims[1], 1, &pixels[pixelStart], 0);
                pixelStart += pixelStride;
                imageIndex++;
            }
        }
    }
}