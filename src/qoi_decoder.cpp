#include "qoi_decoder.hpp"

#include <array>
#include <cstddef>
#include <fstream>

#define QOI_OP_RGB      0b11111110
#define QOI_OP_RGBA     0b11111111
#define QOI_OP_INDEX    0b00000000
#define QOI_OP_DIFF     0b01000000
#define QOI_OP_LUMA     0b10000000
#define QOI_OP_RUN      0b11000000

namespace qoi
{
/**
 * @brief Gets the byte representing the red component of the specified pixel color.
 * @param[in] color 32-bit representation of the color (RGBA)
 * @return Byte representing the red component of the specified color
 */
uint8_t GetRed(uint32_t pixel)
{
    return static_cast<uint8_t>(pixel >> 24);
}

/**
 * @brief Gets the byte representing the green component of the specified pixel color.
 * @param[in] color 32-bit representation of the color (RGBA)
 * @return Byte representing the green component of the specified color
 */
uint8_t GetGreen(uint32_t pixel)
{
    return static_cast<uint8_t>((pixel & 0x00FF0000) >> 16);
}

/**
 * @brief Gets the byte representing the blue component of the specified pixel color.
 * @param[in] color 32-bit representation of the color (RGBA)
 * @return Byte representing the blue component of the specified color
 */
uint8_t GetBlue(uint32_t pixel)
{
    return static_cast<uint8_t>((pixel & 0x0000FF00) >> 8);
}

/**
 * @brief Gets the byte representing the alpha component of the specified pixel color.
 * @param[in] color 32-bit representation of the color (RGBA)
 * @return Byte representing the alpha component of the specified color
 */
uint8_t GetAlpha(uint32_t pixel)
{
    return static_cast<uint8_t>(pixel);
}

/**
 * @brief Packs the specified set of 4 bytes into a single 32-bit unsigned integer
 * @param[in] b0 Byte 0
 * @param[in] b1 Byte 1
 * @param[in] b2 Byte 2
 * @param[in] b3 Byte 3
 * @return Packed value
 */
uint32_t BytesToUint32(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3)
{
    uint32_t ret = 0;
    ret = (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;
    return ret;
}
/**
 * @brief Decodes a QOI format image given data from a given file path.
 * @param[in] inFilePath Path to the QOI file to decode
 * @param[out] outPixelColors Vector where the decoded pixel colors will be placed
 * @param[out] outImageWidth Width of the decoded image
 * @param[out] outImageHeight Height of the decoded image
 * @param[out] outNumChannels Number of color channels in the decoded image
 * @param[out] outColorSpace Colorspace of the decoded image
 * @return Flag indicating whether the decoding process was successful or not.
 */
bool Decode(const std::string &inFilePath, std::vector<uint8_t> &outPixelColors, uint32_t &outImageWidth, uint32_t &outImageHeight, uint8_t &outNumChannels, ColorSpace &outColorSpace)
{
    std::ifstream file(inFilePath, std::ios::binary);
    if (file.fail())
    {
        return false;
    }

    std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return Decode(bytes, outPixelColors, outImageWidth, outImageHeight, outNumChannels, outColorSpace);
}

/**
 * @brief Decodes a QOI format image given data from a stream.
 * @param[in] inStream Byte stream for the QOI format image
 * @param[out] outPixelColors Vector where the decoded pixel colors will be placed
 * @param[out] outImageWidth Width of the decoded image
 * @param[out] outImageHeight Height of the decoded image
 * @param[out] outNumChannels Number of color channels in the decoded image
 * @param[out] outColorSpace Colorspace of the decoded image
 * @return Flag indicating whether the decoding process was successful or not.
 */
bool Decode(std::vector<uint8_t> &inStream, std::vector<uint8_t> &outPixelColors, uint32_t &outImageWidth, uint32_t &outImageHeight, uint8_t &outNumChannels, ColorSpace &outColorSpace)
{
    outPixelColors.clear();

    if (inStream.size() < 22) // Minimum: 14-byte header + 8-byte end marker
    {
        return false;
    }

    // --- Header ---
    if ((inStream[0] != 'q') || (inStream[1] != 'o') || (inStream[2] != 'i') || (inStream[3] != 'f'))
    {
        return false;
    }
    outImageWidth = BytesToUint32(inStream[4], inStream[5], inStream[6], inStream[7]);
    outImageHeight = BytesToUint32(inStream[8], inStream[9], inStream[10], inStream[11]);
    outNumChannels = inStream[12];
    if (inStream[13] > 2)
    {
        return false;
    }
    outColorSpace = inStream[13] == 0 ? ColorSpace::SRGB : ColorSpace::LINEAR;

    // --- Data ---
    uint32_t remainingPixels = outImageWidth * outImageHeight; 

    uint32_t prevPixel = BytesToUint32(0, 0, 0, 255);
    std::array<uint32_t, 64> seenPixels = {};

    size_t offset = 14;
    while ((offset < inStream.size()) && (remainingPixels > 0))
    {
        size_t tagOffset = offset;
        uint8_t chunkTag = inStream[offset++];
        if ((chunkTag == QOI_OP_RGB) || (chunkTag == QOI_OP_RGBA))
        {
            uint8_t red = inStream[offset++];
            uint8_t green = inStream[offset++];
            uint8_t blue = inStream[offset++];
            uint8_t alpha = GetAlpha(prevPixel);
            if (chunkTag == QOI_OP_RGBA)
            {
                alpha = inStream[offset++];
            }

            outPixelColors.push_back(red);
            outPixelColors.push_back(green);
            outPixelColors.push_back(blue);
            if (outNumChannels == 4)
            {
                outPixelColors.push_back(alpha);
            }

            uint32_t hash = (static_cast<uint32_t>(red) * 3 + static_cast<uint32_t>(green) * 5 + static_cast<uint32_t>(blue) * 7 + static_cast<uint32_t>(alpha) * 11) % 64;
            seenPixels[hash] = BytesToUint32(red, green, blue, alpha);

            prevPixel = BytesToUint32(red, green, blue, alpha);
            --remainingPixels;
        }
        else if ((chunkTag & 0b11000000) == QOI_OP_INDEX)
        {
            uint8_t index = chunkTag & 0b00111111;
            uint32_t pixel = seenPixels[index];

            outPixelColors.push_back(GetRed(pixel));
            outPixelColors.push_back(GetGreen(pixel));
            outPixelColors.push_back(GetBlue(pixel));
            if (outNumChannels == 4)
            {
                outPixelColors.push_back(GetAlpha(pixel));
            }

            prevPixel = pixel;
            --remainingPixels;
        }
        else if ((chunkTag & 0b11000000) == QOI_OP_DIFF)
        {
            int8_t dr = static_cast<int8_t>((chunkTag & 0b00110000) >> 4) - 2;
            int8_t dg = static_cast<int8_t>((chunkTag & 0b00001100) >> 2) - 2;
            int8_t db = static_cast<int8_t>(chunkTag & 0b00000011) - 2;

            uint8_t red = GetRed(prevPixel) + dr;
            uint8_t green = GetGreen(prevPixel) + dg;
            uint8_t blue = GetBlue(prevPixel) + db;
            uint8_t alpha = GetAlpha(prevPixel);

            outPixelColors.push_back(red);
            outPixelColors.push_back(green);
            outPixelColors.push_back(blue);
            if (outNumChannels == 4)
            {
                outPixelColors.push_back(alpha);
            }

            uint32_t hash = (static_cast<uint32_t>(red) * 3 + static_cast<uint32_t>(green) * 5 + static_cast<uint32_t>(blue) * 7 + static_cast<uint32_t>(alpha) * 11) % 64;
            seenPixels[hash] = BytesToUint32(red, green, blue, alpha);

            prevPixel = BytesToUint32(red, green, blue, alpha);
            --remainingPixels;
        }
        else if ((chunkTag & 0b11000000) == QOI_OP_LUMA)
        {
            int8_t dg = static_cast<int8_t>(chunkTag & 0b00111111) - 32;

            uint8_t nextChunk = inStream[offset++];
            int8_t dr_dg = static_cast<int8_t>((nextChunk & 0b11110000) >> 4) - 8;
            int8_t db_dg = static_cast<int8_t>(nextChunk & 0b00001111) - 8;

            int8_t dr = dr_dg + dg;
            int8_t db = db_dg + dg;

            uint8_t red = GetRed(prevPixel) + dr;
            uint8_t green = GetGreen(prevPixel) + dg;
            uint8_t blue = GetBlue(prevPixel) + db;
            uint8_t alpha = GetAlpha(prevPixel);

            outPixelColors.push_back(red);
            outPixelColors.push_back(green);
            outPixelColors.push_back(blue);
            if (outNumChannels == 4)
            {
                outPixelColors.push_back(alpha);
            }

            uint32_t hash = (static_cast<uint32_t>(red) * 3 + static_cast<uint32_t>(green) * 5 + static_cast<uint32_t>(blue) * 7 + static_cast<uint32_t>(alpha) * 11) % 64;
            seenPixels[hash] = BytesToUint32(red, green, blue, alpha);

            prevPixel = BytesToUint32(red, green, blue, alpha);
            --remainingPixels;
        }
        else if ((chunkTag & 0b11000000) == QOI_OP_RUN)
        {
            uint8_t run = (chunkTag & 0b00111111);
            run += 1; // Apply bias of -1, run -= -1, which is just run += 1
            if (run > 62)
            {
                outPixelColors.clear();
                return false;
            }

            for (uint8_t i = 0; i < run; i++)
            {
                outPixelColors.push_back(GetRed(prevPixel));
                outPixelColors.push_back(GetGreen(prevPixel));
                outPixelColors.push_back(GetBlue(prevPixel));
                if (outNumChannels == 4)
                {
                    outPixelColors.push_back(GetAlpha(prevPixel));
                }
                --remainingPixels;
            }
        }
        else
        {
            outPixelColors.clear();
            return false;
        }
    }

    return true;
}
}
