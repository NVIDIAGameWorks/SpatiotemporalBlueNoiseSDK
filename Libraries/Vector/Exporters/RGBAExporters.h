/*
* Copyright (c) 2024, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

#pragma

#include <string>
#include <vector>

#include "STBNMath.h"

union Dimensions3D;
union Float;
union Float2;
union Float3;

template<typename T>
void ToRGBA32(T in, unsigned char* out)
{

}

template<>
void ToRGBA32(Float in, unsigned char* out);

template<>
void ToRGBA32(Float2 in, unsigned char* out);

template<>
void ToRGBA32(Float3 in, unsigned char* out);

template<typename T>
std::vector<unsigned char> ConvertCellsAndPDFToRGBA32Texture(const std::vector<T>& cells, const std::vector<float>& pdf, float pdfMin, float pdfMax)
{
    std::vector<unsigned char> output(cells.size() * 4, 0);
    for (size_t index = 0; index < cells.size(); ++index)
    {
        ToRGBA32(cells[index], &output[index * 4]);

        float pdfpercent = 1.0f;
        if (pdfMax != pdfMin)
            pdfpercent = (pdf[index] - pdfMin) / (pdfMax - pdfMin);

        output[index * 4 + 3] = (unsigned char)Clamp(pdfpercent * 256.0f, 0.0f, 255.0f);
    }
    return output;
}

void WriteRGBA32CellsToPNG(const std::string& filePath, const std::vector<unsigned char> output, const Dimensions3D& dims);
