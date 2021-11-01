/*
* Copyright (c) 2019-2021, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

// Size of the texture generated
static const int c_imageSizeX = 128;
static const int c_imageSizeY = 128;
static const int c_imageSizeZ = 64;
static const int c_imageSizeW = 1;

// Settings for the void and cluster algorithm
static const float c_initialBinaryPatternDensity = 0.1f;
static const float c_energySigma = 1.9f;

// blue noise generation helpers
#include "2Dx2D.inl"
#include "2Dx1Dx1D.inl"
#include <direct.h>

int main(int argc, char** argv)
{
    _mkdir("out");

    {
        // make spatiotemporal blue noise
        _2Dx1Dx1D::Maker<c_imageSizeX, c_imageSizeY, c_imageSizeZ, c_imageSizeW, c_energySigma, c_energySigma, c_energySigma, c_energySigma> object;
        BlueNoiseTexturesND textures = object.Make();

        // save it out as pngs
        char buffer[1024];
        sprintf_s(buffer, "out/stbn_scalar_2Dx1Dx1D_%ix%ix%ix%i", c_imageSizeX, c_imageSizeY, c_imageSizeZ, c_imageSizeW);
        strcat_s(buffer, "_%i.png");
        SaveTextures(textures, buffer);

        sprintf_s(buffer, "python ../analyzeVVSTBN.py out/stbn_scalar_2Dx1Dx1D_%ix%ix%ix%i %i", c_imageSizeX, c_imageSizeY, c_imageSizeZ, c_imageSizeW, c_imageSizeZ);
        system(buffer);
    }

    // If making 4D blue noise, comment out the above and uncomment this for 2Dx2D blue noise instead of 2Dx1Dx1D
    /*
    {
        // make spatiotemporal blue noise
        _2Dx2D::Maker<c_imageSizeX, c_imageSizeY, c_imageSizeZ, c_imageSizeW, c_energySigma, c_energySigma, c_energySigma, c_energySigma> object;
        BlueNoiseTexturesND textures = object.Make();

        // save it out as pngs
        char filename[1024];
        sprintf_s(filename, "out/stbn_scalar_2Dx2D_%ix%ix%ix%i", c_imageSizeX, c_imageSizeY, c_imageSizeZ, c_imageSizeW);
        strcat_s(filename, "_%i.png");
        SaveTextures(textures, filename);

        // make XYZ DFTs
        sprintf_s(buffer, "python ../analyzeVVSTBN.py out/stbn_scalar_2Dx1Dx1D_%ix%ix%ix%i %i", c_imageSizeX, c_imageSizeY, c_imageSizeZ, c_imageSizeW, c_imageSizeZ);
        system(buffer);
    }
    */

    return 0;
}
