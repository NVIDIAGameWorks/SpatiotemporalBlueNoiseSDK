/*
* Copyright (c) 2024, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

#include "Utils/Dimensions3D.h"

bool operator==(const Dimensions3D& a, const Dimensions3D& b)
{
	return (a.x == b.x) &&
	       (a.y == b.y) &&
	       (a.z == b.z);
}

size_t Dimensions3D::NumPixels() const
{
    return x * y * z;
}
