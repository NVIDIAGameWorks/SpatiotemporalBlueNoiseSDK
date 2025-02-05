#pragma once

union Dimensions
{
    struct
    {
        size_t x;
        size_t y;
        size_t z;
        size_t w;
    };
    size_t dim[4];
};

bool operator==(const Dimensions& a, const Dimensions& b);