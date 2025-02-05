#include "BlueNoiseTexturesND.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void BlueNoiseTexturesND::Init(const std::vector<int>& _dims, const std::vector<float>& _sigmas, std::vector<int> groups)
{
    dims = _dims;
    sigmas = _sigmas;
    size_t pixelCount = 1;
    for (int i : dims)
    {
        pixelCount *= (size_t)i;
    }
    pixels.clear();
    pixels.resize(pixelCount);

    // make the masks
    if (groups.size() == 0)
        groups.resize(dims.size(), 0);
    std::unordered_set<int> uniqueGroupNumbers;
    for (int g : groups)
        uniqueGroupNumbers.insert(g);
    for (int group : uniqueGroupNumbers)
    {
        unsigned int mask = 0;
        for (size_t index = 0; index < dims.size(); ++index)
        {
            if (groups[index] != group)
                mask |= (1 << index);
        }
        masks.push_back(mask);
    }

    // calculate energy radii so we don't have to consider all pixels for energy changes
    energyEmitMin.resize(dims.size());
    energyEmitMax.resize(dims.size());
    static const float c_energyLoss = 0.005f; // 0.5% lost, so 99.5% accounted for
    for (size_t i = 0; i < dims.size(); ++i)
    {
        int radius = int(sqrtf(-2.0f * sigmas[i] * sigmas[i] * log(c_energyLoss)));

        if (radius * 2 + 1 >= dims[i])
        {
            radius = (dims[i] - 1) / 2;

            energyEmitMin[i] = -radius;
            energyEmitMax[i] = -radius + dims[i] - 1;
        }
        else
        {
            energyEmitMin[i] = -radius;
            energyEmitMax[i] = radius;
        }
    }
}

std::vector<int> BlueNoiseTexturesND::GetDims() const
{
    return dims;
}

std::vector<Pixel>& BlueNoiseTexturesND::GetPixels()
{
    return pixels;
}

const std::vector<Pixel>& BlueNoiseTexturesND::GetPixels() const
{
    return pixels;
}

// pattern of calculating pixel index
// x
// y * width + x
// z * width * height + y * width + x
// w * width * height * depth + z * width * height + y * width + x
// ...

size_t BlueNoiseTexturesND::GetPixelIndex(const std::vector<int>& indices) const
{
    if (indices.size() != dims.size())
    {
        ((int*)0)[0] = 0;
    }

    size_t pixelIndex = 0;
    size_t multiplier = 1;

    for (size_t i = 0; i < dims.size(); ++i)
    {
        pixelIndex += multiplier * indices[i];
        multiplier *= dims[i];
    }

    return pixelIndex;
}

Pixel& BlueNoiseTexturesND::GetPixel(const std::vector<int>& indices)
{
    return pixels[GetPixelIndex(indices)];
}

const Pixel& BlueNoiseTexturesND::GetPixel(const std::vector<int>& indices) const
{
    return pixels[GetPixelIndex(indices)];
}

// pattern of calculating pixel indices
// x = (index % width) / 1
// y = (index % width*height) / width
// z = (index % width*height*depth) / width*height
// w = (index % width*height*depth*other) / width*height*depth
// ...
// C = (index % A) / B

void BlueNoiseTexturesND::GetPixelIndices(int index, std::vector<int>& indices) const
{
    int A = 1;
    int B = 1;

    indices.resize(dims.size());
    for (size_t i = 0; i < dims.size(); ++i)
    {
        A *= dims[0];
        indices[i] = (index % A) / B;
        B = A;
    }
}

void SaveTextures(const BlueNoiseTexturesND& textures, const char* fileNamePattern)
{
    // turn ranks into pixel values
    auto& stbnPixels = textures.GetPixels();
    auto dims = textures.GetDims();
    std::vector<unsigned char> pixels(stbnPixels.size());
    for (size_t index = 0; index < pixels.size(); ++index)
    {
        unsigned char* dest = &pixels[index];
        const Pixel& src = stbnPixels[index];

        float rankPercent = float(src.rank) / float(pixels.size());
        *dest = (unsigned char)Clamp(rankPercent * 256.0f, 0.0f, 255.0f);
    }

    // save images
    {
        char fileName[1024];

        // 1D images are just a single strip
        if (dims.size() == 1)
        {
            sprintf_s(fileName, fileNamePattern, 0);
            stbi_write_png(fileName, dims[0], 1, 1, pixels.data(), 0);
        }
        // otherwise save x/y images. the other dimensions are flattened into 1d.
        else
        {
            int pixelStride = dims[0] * dims[1];
            size_t pixelStart = 0;
            size_t imageIndex = 0;
            while (pixelStart < stbnPixels.size())
            {
                sprintf_s(fileName, fileNamePattern, imageIndex);
                stbi_write_png(fileName, dims[0], dims[1], 1, &pixels[pixelStart], 0);
                pixelStart += pixelStride;
                imageIndex++;
            }
        }
    }
}