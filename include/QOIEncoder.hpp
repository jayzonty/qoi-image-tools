#ifndef QOIENCODER_HEADER
#define QOIENCODER_HEADER

#include <cstdint>
#include <string>
#include <vector>

namespace QOI
{
/**
 * Class containing functionality for encoding a buffer of pixel colors
 * to QOI format
 */
class QOIEncoder
{
public:
    /**
     * @brief Encodes the specified array of pixel colors to a QOI image file
     * @param[in] inPixelColors Array of pixel colors
     * @param[in] imageWidth Image width
     * @param[in] imageHeight Image height
     * @param[in] numChannels Number of channels in the image
     * @param[in] colorSpace Color space of the image
     * @param[in] outputFilePath File path of the output image file
     */
    static bool Encode(const std::vector<uint8_t> &inPixelColors, const uint32_t &imageWidth, const uint32_t &imageHeight, const uint8_t &numChannels, const uint8_t &colorSpace, const std::string &outputFilePath);

    /**
     * @brief Encodes the specified array of pixel colors to QOI format, and stores the result in an array of bytes
     * @param[in] inPixelColors Array of pixel colors
     * @param[in] imageWidth Image width
     * @param[in] imageHeight Image height
     * @param[in] numChannels Number of channels in the image
     * @param[in] colorSpace Color space of the image
     * @param[out] outBytes Array of bytes where the resulting bytes will be stored
     */
    static bool Encode(const std::vector<uint8_t> &inPixelColors, const uint32_t &imageWidth, const uint32_t &imageHeight, const uint8_t &numChannels, const uint8_t &colorSpace, std::vector<uint8_t> &outBytes);

private:
    /**
     * @brief Writes the bytes representation of the specified value to the specified buffer
     * @param[in] val Value
     * @param[in] buffer Buffer
     */
    static void WriteBytes(uint32_t val, std::vector<uint8_t> &buffer);

    /**
     * @brief Gets the byte representing the red component of the specified pixel color.
     * @param[in] color 32-bit representation of the color (RGBA)
     * @return Byte representing the red component of the specified color
     */
    static uint8_t GetRed(uint32_t color);

    /**
     * @brief Gets the byte representing the green component of the specified pixel color.
     * @param[in] color 32-bit representation of the color (RGBA)
     * @return Byte representing the green component of the specified color
     */
    static uint8_t GetGreen(uint32_t color);

    /**
     * @brief Gets the byte representing the blue component of the specified pixel color.
     * @param[in] color 32-bit representation of the color (RGBA)
     * @return Byte representing the blue component of the specified color
     */
    static uint8_t GetBlue(uint32_t color);

    /**
     * @brief Gets the byte representing the alpha component of the specified pixel color.
     * @param[in] color 32-bit representation of the color (RGBA)
     * @return Byte representing the alpha component of the specified color
     */
    static uint8_t GetAlpha(uint32_t color);

    /**
     * @brief Packs the specified set of 4 bytes into a single 32-bit unsigned integer
     * @param[in] b0 Byte 0
     * @param[in] b1 Byte 1
     * @param[in] b2 Byte 2
     * @param[in] b3 Byte 3
     * @return Packed value
     */
    static uint32_t BytesToUint32(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3);
};
}

#endif // QOIENCODER_HEADER 
