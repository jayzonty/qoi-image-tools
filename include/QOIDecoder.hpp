#ifndef QOIDECODER_HEADER
#define QOIDECODER_HEADER

#include <cstdint>
#include <string>
#include <vector>

namespace QOI
{
/**
 * Colorspace enum
 */
enum class ColorSpace
{
    SRGB,
    LINEAR
};

/**
 * Class for managing data and functionality related to
 * decoding QOI format images.
 */
class QOIDecoder
{
public:
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
    static bool Decode(const std::string &inFilePath, std::vector<uint8_t> &outPixelColors, uint32_t &outImageWidth, uint32_t &outImageHeight, uint8_t &outNumChannels, ColorSpace &outColorSpace);

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
    static bool Decode(std::vector<uint8_t> &inStream, std::vector<uint8_t> &outPixelColors, uint32_t &outImageWidth, uint32_t &outImageHeight, uint8_t &outNumChannels, ColorSpace &outColorSpace);

private:
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

#endif // QOIDECODER_HEADER
