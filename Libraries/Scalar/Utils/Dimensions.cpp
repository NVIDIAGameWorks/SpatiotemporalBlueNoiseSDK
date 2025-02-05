#include "Dimensions.h"

bool operator==(const Dimensions& a, const Dimensions& b)
{
    return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w);
}