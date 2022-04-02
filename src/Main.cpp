#include <bitset>
#include <fstream>
#include <iostream>
#include <iterator>

#include "QOIDecoder.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " [qoi file name]" << std::endl;
        return 1;
    }
    
    std::vector<uint8_t> pixels = {};

    uint32_t width, height;
    uint8_t numChannels;
    QOI::ColorSpace colorSpace;
    QOI::QOIDecoder::Decode(argv[1], pixels, width, height, numChannels, colorSpace);

    if (pixels.size() != (width * height * numChannels))
    {
        std::cout << "[QOI] Vector size does not equal the number of pixel colors!\n";
        std::cout << "[QOI] Expected: " << width * height * numChannels << " Actual: " << pixels.size() << std::endl;
    }

    std::cout << "[QOI] Image size: " << width << " x " << height << "\n";
    std::cout << "[QOI] Image channels: " << static_cast<uint32_t>(numChannels) << std::endl;

    int w, h, channels;
    unsigned char *data = stbi_load(argv[2], &w, &h, &channels, 0);

    std::cout << "[STBI] Image size: " << w << " x " << h << "\n";
    std::cout << "[STBI] Image channels: " << channels << std::endl;

    stbi_image_free(data);
    data = nullptr;

    return 0;
}
