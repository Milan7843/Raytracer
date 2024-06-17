#include "HDRIRenderer.h"

HDRIRenderer::HDRIRenderer(const char* vertexShaderPath, const char* fragmentShaderPath)
    : hdriShader(vertexShaderPath, fragmentShaderPath)
{
    generateSkyboxVAO();
}

HDRIRenderer::~HDRIRenderer()
{
}

void HDRIRenderer::drawHDRI(HDRITexture* hdri, Camera& camera)
{
    glBindVertexArray(skyboxVAO);

    // Setting variables
    hdriShader.use();
    glm::mat4 view = camera.getViewMatrix();
    // Removing the translation part of the camera view matrix so that skybox is always around the camera
    view[3][0] = 0.0f;
    view[3][1] = 0.0f;
    view[3][2] = 0.0f;
    hdriShader.setMat4("view", view);
    hdriShader.setMat4("projection", camera.getProjectionMatrix());

    // Binding the hdri
    hdriShader.setInt("hdri", 0);
    //hdriShader.setInt("blurredhdri", 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdri->textureID);

    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, hdri->blurredTextureID);

    // Drawing
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // Unbinding
    glBindVertexArray(0);
}

void HDRIRenderer::generateSkyboxVAO()
{
    // Big cube vertices and indices
    float s = 1.0f;
    float vertices[]{
         s,  s,  s,
         s,  s, -s,
         s, -s,  s,
         s, -s, -s,
        -s,  s,  s,
        -s,  s, -s,
        -s, -s,  s,
        -s, -s, -s,
    };
    int indices[] = {
        7, 3, 1,
        1, 5, 7,

        6, 2, 0,
        0, 4, 6,

        4, 5, 7,
        7, 6, 4,

        0, 1, 3,
        3, 2, 0,

        7, 3, 2,
        2, 6, 7,

        5, 1, 0,
        0, 4, 5
    };

    // Generating the required objects
    unsigned int skyboxVBO, skyboxEBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);

    // Making sure everything gets put on this specific VAO
    glBindVertexArray(skyboxVAO);

    // Binding the buffers
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);

    // Putting the indices and vertices into the buffers
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Letting OpenGL know how to interpret the data: just 3 floats for position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbinding
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
