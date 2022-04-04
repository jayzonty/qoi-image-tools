#include "ImageViewerApp.hpp"

#include "QOIDecoder.hpp"

#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstddef>
#include <fstream>
#include <iostream>

/**
 * @brief Constructor
 */
ImageViewerApp::ImageViewerApp()
{
}

/**
 * @brief Destructor
 */
ImageViewerApp::~ImageViewerApp()
{
}

/**
 * @brief Runs the application
 * @param[in] qoiImagePath Path to the QOI format image
 * @param[in] isVerbose Flag indicating whether to run the viewer in verbose mode
 */
void ImageViewerApp::Run(const std::string &qoiImagePath, bool isVerbose)
{
    std::vector<uint8_t> data = {};
    uint32_t imageWidth, imageHeight;
    uint8_t imageChannels;
    QOI::ColorSpace imageColorSpace;
    if (!QOI::QOIDecoder::Decode(qoiImagePath, data, imageWidth, imageHeight, imageChannels, imageColorSpace))
    {
        std::cerr << "Failed to decode " << qoiImagePath << std::endl;
        return;
    }

	if (isVerbose)
	{
		std::cout << "Image file: " << qoiImagePath << "\n";
		std::cout << "Dimensions: " << imageWidth << " x " << imageHeight << "\n";
		std::cout << "Channels: " << static_cast<uint32_t>(imageChannels) << std::endl;
	}

    // Initialize GLFW
	int glfwInitStatus = glfwInit();
	if (glfwInitStatus == GLFW_FALSE)
	{
		std::cerr << "Failed to initialize GLFW!" << std::endl;
		return;
	}

	// Tell GLFW that we prefer to use OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Tell GLFW that we prefer to use the modern OpenGL
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Tell GLFW to create a window
	int windowWidth = 800;
	int windowHeight = 600;
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Image Viewer", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cerr << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
        return;
	}

	// Tell GLFW to use the OpenGL context that was assigned to the window that we just created
	glfwMakeContextCurrent(window);

	// Register the callback function that handles when the framebuffer size has changed
	glfwSetFramebufferSizeCallback(window, FramebufferSizeChangedCallback);

	// Tell GLAD to load the OpenGL function pointers
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		std::cerr << "Failed to initialize GLAD!" << std::endl;
        glfwTerminate();
        return;
	}

    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0 ,0, framebufferWidth, framebufferHeight);

    Vertex quadVertices[6];
    quadVertices[0] = { { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f } };
    quadVertices[1] = { {  0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f } };
    quadVertices[2] = { {  0.5f,  0.5f, 0.0f }, { 1.0f, 1.0f } };
    quadVertices[3] = { {  0.5f,  0.5f, 0.0f }, { 1.0f, 1.0f } };
    quadVertices[4] = { { -0.5f,  0.5f, 0.0f }, { 0.0f, 1.0f } };
    quadVertices[5] = { { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f } };

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));
    // UV
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));
    
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    GLenum texFormat = GL_RGB;
    if (imageChannels == 4)
    {
        texFormat = GL_RGBA;
    }
	glTexImage2D(GL_TEXTURE_2D, 0, texFormat, imageWidth, imageHeight, 0, texFormat, GL_UNSIGNED_BYTE, data.data());

    GLuint program = CreateShaderProgram("main.vsh", "main.fsh");

    glm::vec3 cameraPosition(0.0f, 0.0f, 1.0f);

    float zoomLevel = 1.0f;
    if (imageWidth >= imageHeight)
    {
        zoomLevel = windowWidth * 1.0f / imageWidth;
    }
    else
    {
        zoomLevel = windowHeight * 1.0f / imageHeight;
    }

    glm::mat4 projMatrix = glm::ortho(-framebufferWidth / 2.0f, framebufferWidth / 2.0f, -framebufferHeight / 2.0f, framebufferHeight / 2.0f, 0.1f, 10.0f);

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		// Clear the colors in our off-screen framebuffer
		glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        glUniform1i(glGetUniformLocation(program, "tex"), 0);

        glm::mat4 viewMatrix(1.0f);
        viewMatrix = glm::scale(viewMatrix, glm::vec3(zoomLevel, zoomLevel, 1.0f));
        viewMatrix = glm::translate(viewMatrix, -cameraPosition);

        glm::mat4 modelMatrix(1.0f);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(imageWidth * 1.0f, imageHeight * 1.0f, 1.0f));

        glm::mat4 mvpMatrix = projMatrix * viewMatrix * modelMatrix;
        GLint mvpMatrixUniform = glGetUniformLocation(program, "mvp");
        glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

        glBindVertexArray(vao);

        glDrawArrays(GL_TRIANGLES, 0 , 6);

		// Tell GLFW to swap the screen buffer with the offscreen buffer
		glfwSwapBuffers(window);

		// Tell GLFW to process window events (e.g., input events, window closed events, etc.)
		glfwPollEvents();
	}

	// --- Cleanup ---

	// Remember to tell GLFW to clean itself up before exiting the application
	glfwTerminate();
}

/**
 * @brief GLFW callback function for when the framebuffer size has changed
 * @param[in] window GLFW window whose framebuffer changed
 * @param[in] width New width
 * @param[in] height new height
 */
void ImageViewerApp::FramebufferSizeChangedCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

/**
 * @brief Creates a shader program based on the provided file paths for the vertex and fragment shaders
 * @param[in] vertexShaderFilePath Vertex shader file path
 * @param[in] fragmentShaderFilePath Fragment shader file path
 * @return OpenGL handle to the created shader program
 */
GLuint ImageViewerApp::CreateShaderProgram(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
{
	GLuint vertexShader = CreateShaderFromFile(GL_VERTEX_SHADER, vertexShaderFilePath);
	GLuint fragmentShader = CreateShaderFromFile(GL_FRAGMENT_SHADER, fragmentShaderFilePath);

	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);

	glDetachShader(program, vertexShader);
	glDeleteShader(vertexShader);
	glDetachShader(program, fragmentShader);
	glDeleteShader(fragmentShader);

	// Check shader program link status
	GLint linkStatus;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	if (linkStatus != GL_TRUE) {
		char infoLog[512];
		GLsizei infoLogLen = sizeof(infoLog);
		glGetProgramInfoLog(program, infoLogLen, &infoLogLen, infoLog);
		std::cerr << "program link error: " << infoLog << std::endl;
	}

	return program;
}

/**
 * @brief Creates a shader based on the provided shader type and the path to the file containing the shader source.
 * @param[in] shaderType Shader type
 * @param[in] shaderFilePath Path to the file containing the shader source
 * @return OpenGL handle to the created shader
 */
GLuint ImageViewerApp::CreateShaderFromFile(const GLuint& shaderType, const std::string& shaderFilePath)
{
	std::ifstream shaderFile(shaderFilePath);
	if (shaderFile.fail())
	{
		std::cerr << "Unable to open shader file: " << shaderFilePath << std::endl;
		return 0;
	}

	std::string shaderSource;
	std::string temp;
	while (std::getline(shaderFile, temp))
	{
		shaderSource += temp + "\n";
	}
	shaderFile.close();

	return CreateShaderFromSource(shaderType, shaderSource);
}

/**
 * @brief Creates a shader based on the provided shader type and the string containing the shader source
 * @param[in] shaderType Shader type
 * @param[in] shaderSource Shader source string
 * @return OpenGL handle to the created shader
 */
GLuint ImageViewerApp::CreateShaderFromSource(const GLuint& shaderType, const std::string& shaderSource)
{
	GLuint shader = glCreateShader(shaderType);

	const char* shaderSourceCStr = shaderSource.c_str();
	GLint shaderSourceLen = static_cast<GLint>(shaderSource.length());
	glShaderSource(shader, 1, &shaderSourceCStr, &shaderSourceLen);
	glCompileShader(shader);

	// Check compilation status
	GLint compileStatus;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE)
	{
		char infoLog[512];
		GLsizei infoLogLen = sizeof(infoLog);
		glGetShaderInfoLog(shader, infoLogLen, &infoLogLen, infoLog);
		std::cerr << "shader compilation error: " << infoLog << std::endl;
	}

	return shader;
}
