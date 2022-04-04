#ifndef IMAGE_VIEWER_APP_HEADER
#define IMAGE_VIEWER_APP_HEADER

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>

/**
 * Class for the image viewer application
 */
class ImageViewerApp
{
private:
    /**
     * Vertex struct
     */
    struct Vertex
    {
        /**
         * Position
         */
        glm::vec3 position;

        /**
         * UV
         */
        glm::vec2 uv;
    };

public:
    /**
     * @brief Constructor
     */
    ImageViewerApp();

    /**
     * @brief Destructor
     */
    ~ImageViewerApp();

    /**
     * @brief Runs the application
     * @param[in] qoiImagePath Path to the QOI format image
     * @param[in] isVerbose Flag indicating whether to run the viewer in verbose mode
     */
    void Run(const std::string &qoiImagePath, bool isVerbose = false);

private:
    /**
     * @brief GLFW callback function for when the framebuffer size has changed
     * @param[in] window GLFW window whose framebuffer changed
     * @param[in] width New width
     * @param[in] height new height
     */
    static void FramebufferSizeChangedCallback(GLFWwindow *window, int width, int height);

    /**
     * @brief Creates a shader program based on the provided file paths for the vertex and fragment shaders
     * @param[in] vertexShaderFilePath Vertex shader file path
     * @param[in] fragmentShaderFilePath Fragment shader file path
     * @return OpenGL handle to the created shader program
     */
    GLuint CreateShaderProgram(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);

    /**
     * @brief Creates a shader based on the provided shader type and the path to the file containing the shader source.
     * @param[in] shaderType Shader type
     * @param[in] shaderFilePath Path to the file containing the shader source
     * @return OpenGL handle to the created shader
     */
    GLuint CreateShaderFromFile(const GLuint& shaderType, const std::string& shaderFilePath);

    /**
     * @brief Creates a shader based on the provided shader type and the string containing the shader source
     * @param[in] shaderType Shader type
     * @param[in] shaderSource Shader source string
     * @return OpenGL handle to the created shader
     */
    GLuint CreateShaderFromSource(const GLuint& shaderType, const std::string& shaderSource);
};

#endif // IMAGE_VIEWER_APP_HEADER
