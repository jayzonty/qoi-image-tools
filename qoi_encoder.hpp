#ifndef QOI_ENCODER_HEADER
#define QOI_ENCODER_HEADER

#include <array>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

#ifndef QOI_CHUNK_TAGS
#define QOI_CHUNK_TAGS

#define QOI_OP_RGB      0b11111110
#define QOI_OP_RGBA     0b11111111
#define QOI_OP_INDEX    0b00000000
#define QOI_OP_DIFF     0b01000000
#define QOI_OP_LUMA     0b10000000
#define QOI_OP_RUN      0b11000000

#endif // QOI_CHUNK_TAGS

namespace qoi
{
/**
 * @brief Writes the bytes representation of the specified value to the specified buffer
 * @param[in] val Value
 * @param[in] buffer Buffer
 */
inline void WriteBytes(uint32_t val, std::vector<uint8_t> &buffer)
{
    uint8_t b0 = static_cast<uint8_t>(val >> 24);
    uint8_t b1 = static_cast<uint8_t>((val & 0x00FF0000) >> 16);
    uint8_t b2 = static_cast<uint8_t>((val & 0x0000FF00) >> 8);
    uint8_t b3 = static_cast<uint8_t>(val & 0x000000FF);

    buffer.push_back(b0);
    buffer.push_back(b1);
    buffer.push_back(b2);
    buffer.push_back(b3);
}

/**
 * @brief Encodes the specified array of pixel colors to QOI format, and stores the result in an array of bytes
 * @param[in] inPixelColors Array of pixel colors
 * @param[in] imageWidth Image width
 * @param[in] imageHeight Image height
 * @param[in] numChannels Number of channels in the image
 * @param[in] colorSpace Color space of the image
 * @param[out] outBytes Array of bytes where the resulting bytes will be stored
 */
inline bool Encode(const std::vector<uint8_t> &inPixelColors, const uint32_t &imageWidth, const uint32_t &imageHeight, const uint8_t &numChannels, const uint8_t &colorSpace, std::vector<uint8_t> &outBytes)
{
    // --- Header ---
    outBytes.push_back('q');
    outBytes.push_back('o');
    outBytes.push_back('i');
    outBytes.push_back('f');
    WriteBytes(imageWidth, outBytes);
    WriteBytes(imageHeight, outBytes);
    outBytes.push_back(numChannels);
    outBytes.push_back(colorSpace);

    // --- Data ---
    uint32_t prevColor = 0x000000FF;
    std::array<uint32_t, 64> seenPixels = {};
    bool isRun = false;

    for (size_t i = 0; i < inPixelColors.size(); i += numChannels)
    {
        uint8_t red   = inPixelColors[i];
        uint8_t green = inPixelColors[i + 1];
        uint8_t blue  = inPixelColors[i + 2];
        uint8_t alpha = (numChannels == 4) ? inPixelColors[i + 3] : 255;

        uint32_t currentColor = 
            (static_cast<uint32_t>(red)   << 24) |
            (static_cast<uint32_t>(green) << 16) |
            (static_cast<uint32_t>(blue)  << 8) |
            alpha;
        uint32_t hash = (static_cast<uint32_t>(red) * 3 + static_cast<uint32_t>(green) * 5 + static_cast<uint32_t>(blue) * 7 + static_cast<uint32_t>(alpha) * 11) % 64;
        if (currentColor == prevColor)
        {
            if (!isRun)
            {
                // Push a run tag, and since there's a bias of -1,
                // actual + -1 = encoded. Since actual=1, then
                // encoded=0
                outBytes.push_back(QOI_OP_RUN);
                isRun = true;
            }
            else
            {
                uint8_t run = (outBytes.back() & (~QOI_OP_RUN)) + 1;
                if (run < 62)
                {
                    outBytes.back() = run | QOI_OP_RUN;
                }
                else
                {
                    outBytes.push_back(QOI_OP_RUN);
                }
            }
        }
        else 
        {
            isRun = false;

            if (currentColor == seenPixels[hash])
            {
                uint8_t indexChunk = static_cast<uint8_t>(hash);
                outBytes.push_back(indexChunk);
            }
            else
            {
                uint8_t prevRed   = static_cast<uint8_t>(prevColor >> 24);
                uint8_t prevGreen = static_cast<uint8_t>((prevColor & 0x00FF0000) >> 16);
                uint8_t prevBlue  = static_cast<uint8_t>((prevColor & 0x0000FF00) >> 8);
                uint8_t prevAlpha = static_cast<uint8_t>(prevColor);
                
                // TODO: There's most likely a better way of doing this, maybe exploting the bias?
                int32_t dr = static_cast<int32_t>(red) - static_cast<int32_t>(prevRed);
                int32_t dg = static_cast<int32_t>(green) - static_cast<int32_t>(prevGreen);
                int32_t db = static_cast<int32_t>(blue) - static_cast<int32_t>(prevBlue);
                int32_t dr_dg = dr - dg;
                int32_t db_dg = db - dg;
                if ((alpha == prevAlpha) && (-2 <= dr && dr <= 1) && (-2 <= dg && dg <= 1) && (-2 <= db && db <= 1))
                {
                    uint8_t chunk = QOI_OP_DIFF;
                    chunk |= (dr + 2) << 4;
                    chunk |= (dg + 2) << 2;
                    chunk |= (db + 2);
                    outBytes.push_back(chunk);
                }
                else if ((alpha == prevAlpha) && (-32 <= dg && dg <= 31) && (-8 <= dr_dg && dr_dg <= 7) && (-8 <= db_dg && db_dg <= 7))
                {
                    uint8_t chunk0 = QOI_OP_LUMA;
                    chunk0 |= (dg + 32);

                    uint8_t chunk1 = 0;
                    chunk1 |= (dr_dg + 8) << 4;
                    chunk1 |= (db_dg + 8);

                    outBytes.push_back(chunk0);
                    outBytes.push_back(chunk1);
                }
                else
                {
                    if ((numChannels == 3) || (alpha == prevAlpha))
                    {
                        outBytes.push_back(QOI_OP_RGB);
                        outBytes.push_back(red);
                        outBytes.push_back(green);
                        outBytes.push_back(blue);
                    }
                    else
                    {
                        outBytes.push_back(QOI_OP_RGBA);
                        outBytes.push_back(red);
                        outBytes.push_back(green);
                        outBytes.push_back(blue);
                        outBytes.push_back(alpha);
                    }
                }
            }
        }

        seenPixels[hash] = currentColor;
        prevColor = currentColor;
    }

    // --- End marker ---
    outBytes.push_back(0x00);
    outBytes.push_back(0x00);
    outBytes.push_back(0x00);
    outBytes.push_back(0x00);
    outBytes.push_back(0x00);
    outBytes.push_back(0x00);
    outBytes.push_back(0x00);
    outBytes.push_back(0x01);

    return true;
}

/**
 * @brief Encodes the specified array of pixel colors to a QOI image file
 * @param[in] inPixelColors Array of pixel colors
 * @param[in] imageWidth Image width
 * @param[in] imageHeight Image height
 * @param[in] numChannels Number of channels in the image
 * @param[in] colorSpace Color space of the image
 * @param[in] outputFilePath File path of the output image file
 */
inline bool Encode(const std::vector<uint8_t> &inPixelColors, const uint32_t &imageWidth, const uint32_t &imageHeight, const uint8_t &numChannels, const uint8_t &colorSpace, const std::string &outputFilePath)
{
    std::vector<uint8_t> bytesToWrite = {};
    if (!Encode(inPixelColors, imageWidth, imageHeight, numChannels, colorSpace, bytesToWrite))
    {
        return false;
    }

    std::ofstream file(outputFilePath, std::ios::binary);
    if (file.fail())
    {
        return false;
    }

    file.write(reinterpret_cast<char*>(bytesToWrite.data()), bytesToWrite.size());

    return true;
}
}

#endif // QOI_ENCODER_HEADER 
