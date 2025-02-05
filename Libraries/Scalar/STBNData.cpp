#include "STBNData.h"

bool operator==(const SigmaPerDimension& a, const SigmaPerDimension& b)
{
    return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w);
}

STBNData::STBNData(Dimensions _dimensions) :
    dimensions(_dimensions),
    numPixels(_dimensions.x* _dimensions.y* _dimensions.z* _dimensions.w)
{
    energy.resize(numPixels, 0.0f);
    pixelOn.resize(numPixels, false);
    pixelRank.resize(numPixels, numPixels);
}

bool operator==(const STBNData& a, const STBNData& b)
{
	return (a.dimensions == b.dimensions) &&
           (a.numPixels == b.numPixels) &&
           (a.energy == b.energy) &&
           (a.pixelOn == b.pixelOn) &&
           (a.pixelRank == b.pixelRank);
}