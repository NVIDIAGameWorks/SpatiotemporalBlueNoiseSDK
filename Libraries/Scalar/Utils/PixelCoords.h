#pragma once

union Dimensions;

union PixelCoords
{
    struct
    {
        size_t x;
        size_t y;
        size_t z;
        size_t w;
    };
    size_t coords[4];
    inline size_t operator[](size_t index) const { return coords[index]; }
    inline size_t& operator[](size_t index) { return coords[index]; }
};

size_t PixelCoordsToPixelIndex(const PixelCoords& pixelCoords, const Dimensions& dims);

PixelCoords PixelIndexToPixelCoords(size_t pixelIndex, const Dimensions& dims);