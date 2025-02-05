#pragma once

#include <vector>
#include <vector>

#include "Utils/Dimensions.h"

struct SigmaPerDimension
{
    float x;
    float y;
    float z;
    float w;
};

bool operator==(const SigmaPerDimension& a, const SigmaPerDimension& b);

struct STBNData
{
    STBNData(Dimensions _dimensions);

    std::vector<float> energy;
    std::vector<bool> pixelOn;
    std::vector<size_t> pixelRank;

    Dimensions dimensions;
    const size_t numPixels;
};

bool operator==(const STBNData& a, const STBNData& b);