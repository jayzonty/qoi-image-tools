#ifndef QOI_DECODER_HEADER
#define QOI_DECODER_HEADER

#include <cstdint>
#include <string>
#include <vector>

namespace qoi
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
 * @brief Decodes a QOI format image given data from a given file path.
 * @param[in] inFilePath Path to the QOI file to decode
 * @param[out] outPixelColors Vector where the decoded pixel colors will be placed
 * @param[out] outImageWidth Width of the decoded image
 * @param[out] outImageHeight Height of the decoded image
 * @param[out] outNumChannels Number of color channels in the decoded image
 * @param[out] outColorSpace Colorspace of the decoded image
 * @return Flag indicating whether the decoding process was successful or not.
 */
extern bool Decode(const std::string &inFilePath, std::vector<uint8_t> &outPixelColors, uint32_t &outImageWidth, uint32_t &outImageHeight, uint8_t &outNumChannels, ColorSpace &outColorSpace);

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
extern bool Decode(std::vector<uint8_t> &inStream, std::vector<uint8_t> &outPixelColors, uint32_t &outImageWidth, uint32_t &outImageHeight, uint8_t &outNumChannels, ColorSpace &outColorSpace);
}

#endif // QOI_DECODER_HEADER
