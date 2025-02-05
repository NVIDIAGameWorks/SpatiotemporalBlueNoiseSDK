/*
* Copyright (c) 2024, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

/*
* Copyright (c) 2019-2021, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/
#include <direct.h>
#include <filesystem>
#include <iostream>

#include "cxxopts.hpp"

#include "BlueNoiseTexturesND.h"
#include "CMDOptionsParser.h"
#include "ProgressContext.h"
#include "VectorSTBNImplSelector.h"
#include "VectorProgramOptions.h"
#include "Utils/Dimensions3D.h"

int main(int argc, char** argv)
{
    VectorProgramOptions programOptions = ParseCmdArgs(argc, argv);

    VectorSTBNImplSelector runner(programOptions.makeType, programOptions.args);
    runner.Run();

    return 0;
}
