#pragma once

#include <unordered_set>
#include <vector>

#include "STBNMath.h"

struct Pixel
{
    bool on = false;
    int rank = -1;
    float energy = 0.0f;
};

class BlueNoiseTexturesND
{
public:
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

    void Init(const std::vector<int>& _dims, const std::vector<float>& _sigmas, std::vector<int> groups);

    std::vector<int> GetDims() const;
    std::vector<Pixel>& GetPixels();
    const std::vector<Pixel>& GetPixels() const;

    size_t GetPixelIndex(const std::vector<int>& indices) const;

    Pixel& GetPixel(const std::vector<int>& indices);

    const Pixel& GetPixel(const std::vector<int>& indices) const;

    void GetPixelIndices(int index, std::vector<int>& indices) const;

private:
    std::vector<int> dims;
    std::vector<float> sigmas;
    std::vector<Pixel> pixels;
    std::vector<unsigned int> masks;
    std::vector<int> energyEmitMin;
    std::vector<int> energyEmitMax;
};

void SaveTextures(const BlueNoiseTexturesND& textures, const char* fileNamePattern);