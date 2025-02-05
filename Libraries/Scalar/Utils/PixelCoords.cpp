#include "PixelCoords.h"

#include "Utils/Dimensions.h"

size_t PixelCoordsToPixelIndex(const PixelCoords& pixelCoords, const Dimensions& dims)
{
    size_t ret = 0;

    ret += pixelCoords[3];
    ret *= dims.z;
    ret += pixelCoords[2];
    ret *= dims.y;
    ret += pixelCoords[1];
    ret *= dims.x;
    ret += pixelCoords[0];

    return ret;
}

PixelCoords PixelIndexToPixelCoords(size_t pixelIndex, const Dimensions& dims)
{
    PixelCoords ret;

    ret[0] = (pixelIndex % (dims.x)) / (1);
    ret[1] = (pixelIndex % (dims.x * dims.y)) / (dims.x);
    ret[2] = (pixelIndex % (dims.x * dims.y * dims.z)) / (dims.x * dims.y);
    ret[3] = (pixelIndex % (dims.x * dims.y * dims.z * dims.y)) / (dims.x * dims.y * dims.z);

    return ret;
}