#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// Change the viewport size if the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// Initiating GLFW
void init_glfw();
// Callback for when the mouse is moved
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
// Draws the axes
void drawAxes(unsigned int VAO, Shader* shader, Camera* camera);
// Generates a VAO for the axes
unsigned int generateAxesVAO();

void processInput(GLFWwindow* window);


const unsigned int WINDOW_SIZE_X = 1200, WINDOW_SIZE_Y = 700;

// Making a camera
Camera camera(glm::vec3(-3.0f, 2.0f, -2.0f));

float cameraSpeed = 0.01f;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float timeSinceSwitchingModes = 100.0f;

bool inRaytraceMode = false;

int main()
{
    init_glfw();

    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Making a GLFW window
    GLFWwindow* window = glfwCreateWindow(WINDOW_SIZE_X, WINDOW_SIZE_Y, "OpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // GLAD manages function pointers for OpenGL, so we cannot run without it
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::cout << "max uniforms: " << GL_MAX_VERTEX_UNIFORM_VECTORS << std::endl;

    // Setting viewport size
    glViewport(0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y);

    // Change the viewport size if the window is resized
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    Model testModel("src/models/icosphere.obj");

    Shader uvShader("src/Shaders/uvColorVertexShader.shader", "src/Shaders/uvColorFragmentShader.shader");
    Shader solidColorShader("src/Shaders/solidColorVertexShader.shader", "src/Shaders/solidColorFragmentShader.shader");
    Shader textureShader("src/Shaders/textureVertexShader.shader", "src/Shaders/textureFragmentShader.shader");
    std::string triangleCount = std::to_string(Model::triangleCount);
    std::string meshCount = std::to_string(Mesh::meshCount);
    Shader raymarchShader("src/Shaders/raymarchVertexShader.shader", 
        "src/Shaders/raymarchFragmentShader.shader", triangleCount, meshCount);
    Shader raytracingShader("src/Shaders/raymarchVertexShader.shader",
        "src/Shaders/raytracingFragmentShader.shader", triangleCount, meshCount);
    
    // Object 1: simple uv coords
    float s = 1.0f;
    float vertices[] = {
        // Positions
         s,  s, 0.0f, // top right
         s, -s, 0.0f, // bottom right
        -s, -s, 0.0f, // bottom left
        -s,  s, 0.0f, // top left 
    };

    unsigned int indices[] = {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    // Vertex Buffer Object, stores the vertices with all their data on the GPU
    unsigned int vao1, vbo1, ebo1;

    glGenVertexArrays(1, &vao1);
    glGenBuffers(1, &vbo1);
    glGenBuffers(1, &ebo1);

    glBindVertexArray(vao1);
    glBindBuffer(GL_ARRAY_BUFFER, vbo1); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo1);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);



    // Uniform Buffer Object for the vertices
    unsigned int trisUniformBlockIndex = glGetUniformBlockIndex(raytracingShader.ID, "Tris");
    glUniformBlockBinding(raytracingShader.ID, trisUniformBlockIndex, 0);

    // Generating the buffer to put the vertices in
    unsigned int uboTris;
    glGenBuffers(1, &uboTris);
    glBindBuffer(GL_UNIFORM_BUFFER, uboTris);
    glBufferData(GL_UNIFORM_BUFFER, Model::triangleCount * Mesh::getTriangleSize(), NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, trisUniformBlockIndex, uboTris);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);


    /*
    glBindBuffer(GL_UNIFORM_BUFFER, uboTris);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(triangles), &triangles);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);*/

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Input
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);

    bool shaderModelDataNeedsUpdate = true;

    unsigned int axesVAO = generateAxesVAO();


    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        timeSinceSwitchingModes += deltaTime;

        // Input
        processInput(window);
        camera.processInput(window, deltaTime);

        // Rendering
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        if (inRaytraceMode)
        {
            /* RAYTRACED RENDERING */
            Shader* usedShader = &raytracingShader;

            usedShader->use();
            usedShader->setVector2("screenSize", WINDOW_SIZE_X, WINDOW_SIZE_Y);
            usedShader->setVector3("cameraPosition", camera.getPosition());
            usedShader->setVector3("cameraRotation", camera.getRotation());
            if (shaderModelDataNeedsUpdate)
            {
                testModel.writeToShader(usedShader, uboTris);
                shaderModelDataNeedsUpdate = false;
            }

            glBindVertexArray(vao1);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        else
        {
            /* REGULAR RENDERING */
            solidColorShader.use();
            solidColorShader.setVector3("inputColor", glm::vec3(0.8f, 0.3f, 0.8f));

            // Drawing axes
            drawAxes(axesVAO, &solidColorShader, &camera);

            // Uniforms
            float time = glfwGetTime();

            // View matrix
            glm::mat4 view = glm::mat4(1.0f);
            view = camera.getViewMatrix();
            solidColorShader.setMat4("view", view);

            // Projection matrix
            glm::mat4 projection;
            projection = camera.getProjectionMatrix(WINDOW_SIZE_X, WINDOW_SIZE_Y);
            solidColorShader.setMat4("projection", projection);

            testModel.draw(&solidColorShader);
        }
        


        /*
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 1.0f));
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        


        //glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        //transform = transform * view;
        
        textureShader.Activate();
        textureShader.SetMat4f("model", glm::value_ptr(model));
        textureShader.SetMat4f("view", glm::value_ptr(view));
        textureShader.SetMat4f("projection", glm::value_ptr(projection));
        
        //glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(vao2);
        //glBindTexture(GL_TEXTURE_2D, texture);

        //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        */

        // Binding the VAO
        /*
        glBindVertexArray(VAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawArrays(GL_TRIANGLES, 0, 6 * 6);*/

        // Output
        glfwSwapBuffers(window);
        // Check for input
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao1);
    glDeleteBuffers(1, &vbo1);
    glDeleteBuffers(1, &ebo1);
    //glDeleteVertexArrays(1, &vao2);
    //glDeleteBuffers(1, &vbo2);
    //glDeleteBuffers(1, &ebo2);
    glDeleteProgram(solidColorShader.ID);
    glDeleteProgram(uvShader.ID);
    glDeleteProgram(textureShader.ID);


	return 0;
}

void init_glfw() 
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    camera.mouseCallback(window, xpos, ypos);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS && timeSinceSwitchingModes > 0.3f)
    {
        timeSinceSwitchingModes = 0.0f;
        inRaytraceMode = !inRaytraceMode;
    }
}


unsigned int generateAxesVAO()
{
    // Creating our vertex array object
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Putting the vertices into the array buffer
    float vertices[] = {
        // Position
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f
    };
    unsigned int indices[] = {
        0, 1
    };

    unsigned int VBO;
    // Making a buffer with the ID in VBO
    glGenBuffers(1, &VBO);
    // Binding our new buffer to the GL_ARRAY_BUFFER target
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Binding our custom data into the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Binding the element buffer object
    unsigned int EBO;
    // Generating a buffer for the EBO
    glGenBuffers(1, &EBO);
    // Binding the EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // Inserting data into the buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    // Telling OpenGL how to interpret the data
    // Position data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    return VAO;
}

void drawAxes(unsigned int VAO, Shader* shader, Camera* camera)
{
    glLineWidth(0.01f);
    float s = 1.0f;

    shader->use();

    // Model matrix
    glm::mat4 model = glm::mat4(1.0f);
    shader->setMat4("model", model);

    // View matrix
    glm::mat4 view = glm::mat4(1.0f);
    view = camera->getViewMatrix();
    shader->setMat4("view", view);

    // Projection matrix
    glm::mat4 projection;
    projection = camera->getProjectionMatrix(WINDOW_SIZE_X, WINDOW_SIZE_Y);
    shader->setMat4("projection", projection);

    // Binding the VAO
    glBindVertexArray(VAO);

    // Drawing the x-axis (red)
    shader->setVector3("inputColor", 194 / 255.0f, 31 / 255.0f, 19 / 255.0f);
    glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT /* index type */, 0);

    // Drawing the y-axis (up) (green)
    model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    shader->setMat4("model", model);
    shader->setVector3("inputColor", 29 / 255.0f, 194 / 255.0f, 57 / 255.0f);
    glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT /* index type */, 0);

    // Drawing the z-axis (blue)
    model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    shader->setMat4("model", model);
    shader->setVector3("inputColor", 19 / 255.0f, 37 / 255.0f, 194 / 255.0f);
    glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT /* index type */, 0);
    glBindVertexArray(0);
}