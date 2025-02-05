/*
* Copyright (c) 2024, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

#include "Exporters/RGBAExporters.h"

#include <sstream>

#include "stb_image_write.h"

#include "STBNMath.h"
#include "Types/Float.h"
#include "Types/Float2.h"
#include "Types/Float3.h"
#include "Utils/Dimensions3D.h"

template<>
void ToRGBA32(Float in, unsigned char* out)
{
    out[0] = (unsigned char)Clamp((in[0] * 0.5f + 0.5f) * 256.0f, 0.0f, 255.0f);
}

template<>
void ToRGBA32(Float2 in, unsigned char* out)
{
    out[0] = (unsigned char)Clamp((in[0] * 0.5f + 0.5f) * 256.0f, 0.0f, 255.0f);
    out[1] = (unsigned char)Clamp((in[1] * 0.5f + 0.5f) * 256.0f, 0.0f, 255.0f);
}

template<>
void ToRGBA32(Float3 in, unsigned char* out)
{
    out[0] = (unsigned char)Clamp((in[0] * 0.5f + 0.5f) * 256.0f, 0.0f, 255.0f);
    out[1] = (unsigned char)Clamp((in[1] * 0.5f + 0.5f) * 256.0f, 0.0f, 255.0f);
    out[2] = (unsigned char)Clamp((in[2] * 0.5f + 0.5f) * 256.0f, 0.0f, 255.0f);
}

void WriteRGBA32CellsToPNG(const std::string& filePath, const std::vector<unsigned char> output, const Dimensions3D& dims)
{
    for (int i = 0; i < dims.z; ++i)
    {
        std::stringstream ss;
        ss << filePath << "_" << dims.x << "x" << dims.y << "x" << dims.z << "_" << i << ".png";
        stbi_write_png(ss.str().c_str(), (int)dims.x, (int)dims.y, 4, &output[i * dims.x * dims.y * 4], 0);
    }
}
