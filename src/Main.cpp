
#include "ImageViewerApp.hpp"
#include "QOIDecoder.hpp"
#include "QOIEncoder.hpp"
#include <fstream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <cstring>
#include <iostream>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " [qoi file name]" << std::endl;
        return 1;
    }

    const char* ENCODE_OPTION = "-e";
    const char* OUTPUT_OPTION = "-o";
    const char* VIEWER_OPTION = "-v";
    const char* VERBOSE_FLAG = "--verbose";

    std::string inputFilePath = {};
    std::string outputFilePath = {};
    bool isEncode = false;
    bool isViewer = false;
    bool isVerbose = false;

    for (size_t i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], ENCODE_OPTION) == 0)
        {
            isEncode = true;
            if (i + 1 < argc)
            {
                inputFilePath = argv[++i];
            }
        }
        else if (strcmp(argv[i], OUTPUT_OPTION) == 0)
        {
            if (i + 1 < argc)
            {
                outputFilePath = argv[++i];
            }
        }
        else if (strcmp(argv[i], VIEWER_OPTION) == 0)
        {
            isViewer = true;
            if (i + 1 < argc)
            {
                inputFilePath = argv[++i];
            }
        }
        else if (strcmp(argv[i], VERBOSE_FLAG) == 0)
        {
            isVerbose = true;
        }
    }

    if (isViewer)
    {
        ImageViewerApp viewerApp;
        viewerApp.Run(inputFilePath, isVerbose);
    }
    else if (isEncode)
    {
        if (inputFilePath.empty())
        {
            std::cerr << "No input file specified!" << std::endl;
            return 1;
        }
        if (outputFilePath.empty())
        {
            std::cerr << "No output file specified!" << std::endl;
            return 1;
        }

        char buffer[4] = {};
        std::ifstream inputFile(inputFilePath, std::ios::binary);
        inputFile.read(buffer, 4);

        // Check if the input file happens to be in QOI format already by checking the first four bytes.
        // If it is, then we don't do anything.
        if ((buffer[0] == 'q') && (buffer[1] == 'o') && (buffer[2] == 'i') && (buffer[3] == 'f'))
        {
            std::cout << "Input image file is already in QOI format!" << std::endl;
            return 1;
        }

        int inputImageWidth = 0, inputImageHeight = 0, inputImageNumChannels = 0;
        unsigned char *pixels = stbi_load(inputFilePath.c_str(), &inputImageWidth, &inputImageHeight, &inputImageNumChannels, 0);
        if (pixels == nullptr)
        {
            std::cerr << "Cannot read input image file!" << std::endl;
            return 1;
        }

        std::vector<uint8_t> pixelsVector;
        pixelsVector.resize(inputImageWidth * inputImageHeight * inputImageNumChannels);
        memcpy(pixelsVector.data(), pixels, pixelsVector.size());
        stbi_image_free(pixels);

        if (!QOI::QOIEncoder::Encode(pixelsVector, inputImageWidth, inputImageHeight, inputImageNumChannels, 0, outputFilePath))
        {
            std::cerr << "Failed to encode " << inputFilePath << " to QOI format!" << std::endl;
        }
    }

    return 0;
}
