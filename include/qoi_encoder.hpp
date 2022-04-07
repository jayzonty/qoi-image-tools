#ifndef QOI_ENCODER_HEADER
#define QOI_ENCODER_HEADER

#include <cstdint>
#include <string>
#include <vector>

namespace qoi
{
/**
 * @brief Encodes the specified array of pixel colors to a QOI image file
 * @param[in] inPixelColors Array of pixel colors
 * @param[in] imageWidth Image width
 * @param[in] imageHeight Image height
 * @param[in] numChannels Number of channels in the image
 * @param[in] colorSpace Color space of the image
 * @param[in] outputFilePath File path of the output image file
 */
extern bool Encode(const std::vector<uint8_t> &inPixelColors, const uint32_t &imageWidth, const uint32_t &imageHeight, const uint8_t &numChannels, const uint8_t &colorSpace, const std::string &outputFilePath);

/**
 * @brief Encodes the specified array of pixel colors to QOI format, and stores the result in an array of bytes
 * @param[in] inPixelColors Array of pixel colors
 * @param[in] imageWidth Image width
 * @param[in] imageHeight Image height
 * @param[in] numChannels Number of channels in the image
 * @param[in] colorSpace Color space of the image
 * @param[out] outBytes Array of bytes where the resulting bytes will be stored
 */
extern bool Encode(const std::vector<uint8_t> &inPixelColors, const uint32_t &imageWidth, const uint32_t &imageHeight, const uint8_t &numChannels, const uint8_t &colorSpace, std::vector<uint8_t> &outBytes);
}

#endif // QOI_ENCODER_HEADER 
