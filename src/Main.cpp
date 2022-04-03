#include <iostream>
#include <iterator>

#include "ImageViewerApp.hpp"
#include "QOIDecoder.hpp"
#include "QOIEncoder.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " [qoi file name]" << std::endl;
        return 1;
    }

    {
        ImageViewerApp viewerApp;
        viewerApp.Run(argv[1]);
    }

    return 0;
}
