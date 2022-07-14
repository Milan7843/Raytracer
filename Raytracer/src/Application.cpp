#define STB_IMAGE_IMPLEMENTATION

#include "Application.h"


Application::Application(unsigned int WIDTH, unsigned int HEIGHT)
    : WINDOW_SIZE_X(WIDTH), WINDOW_SIZE_Y(HEIGHT), camera(glm::vec3(6.7f, 2.7f, -3.7f))
{

}

Application::~Application()
{
    std::cout << "Application instance destroyed" << std::endl;
}

int Application::Start()
{
    initialiseGLFW();

    // Making a GLFW window
    GLFWwindow* window = glfwCreateWindow(WINDOW_SIZE_X, WINDOW_SIZE_Y, "OpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    ImGuiUserInterface userInterface;

    userInterface.initialiseImGui(window);

    // GLAD manages function pointers for OpenGL, so we cannot run without it
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::cout << "max uniforms: " << GL_MAX_VERTEX_UNIFORM_VECTORS << std::endl;

    // Setting viewport size
    glViewport(0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y);

    // Enabling depth testing for rasterized view: makes sure objects get drawn on top of each other in the correct order
    glEnable(GL_DEPTH_TEST);

    // Setting the callback for window resizing
    Callbacks& callbacks = Callbacks::getInstance();
    callbacks.setCamera(&camera);

    // Change the viewport size if the window is resized
    glfwSetFramebufferSizeCallback(window, &Callbacks::framebuffer_size_callback);

    // Must instantiate the buffer to be able to render to it: otherwise continuous rendering is enabled
    //camera.instantiatePixelBuffer();

    // Making a scene
    Scene scene = Scene();

    // MATERIALS
    Material whiteMaterial(glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f, 0.0f);
    Material reflectiveMaterial(glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.0f, 0.0f);
    Material transparentMaterial(glm::vec3(0.0f, 1.0f, 0.0f), 0.5f, 1.0f, 0.01f);
    Material roseMaterial(glm::vec3(0.8f, 0.2f, 0.3f), 0.0f, 0.0f, 0.0f);

    scene.addMaterial(whiteMaterial);
    scene.addMaterial(reflectiveMaterial);
    scene.addMaterial(transparentMaterial);
    //scene.addMaterial(roseMaterial);


    // Adding our test models: !! MUST BE TRIANGULATED !!
    Model* plane = scene.addModel("src/models/plane.obj", 0);
    Model* icosphere = scene.addModel("src/models/icosphere.obj", 2);

    // Always first move, then rotate, then scale
    icosphere->move(glm::vec3(1.0f, 0.6f, 2.0f));
    icosphere->scale(0.6f);
    icosphere->applyTransformations();

    
    Sphere* sphere1 = scene.addSphere(glm::vec3(0.0f, 1.0f, 0.0f), 0.8f, 2);
    Sphere* sphere2 = scene.addSphere(glm::vec3(1.0f, 1.0f, -2.0f), 1.4f, 1);
    Sphere* sphere3 = scene.addSphere(glm::vec3(2.0f, 1.0f, 1.0f), 0.6f, 2);

    // LIGHTS
    PointLight pointLight1(glm::vec3(0.0f, 1.8f, 1.8f), glm::vec3(1.0f, 0.0f, 0.0f), 2.0f);
    PointLight pointLight2(glm::vec3(2.0f, 1.8f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 2.0f);

    scene.addPointLight(pointLight1);
    scene.addPointLight(pointLight2);



    Shader uvShader("src/Shaders/uvColorVertexShader.shader", "src/Shaders/uvColorFragmentShader.shader");
    Shader solidColorShader("src/Shaders/solidColorVertexShader.shader", "src/Shaders/solidColorFragmentShader.shader");
    Shader textureShader("src/Shaders/textureVertexShader.shader", "src/Shaders/textureFragmentShader.shader");
    /*
    Shader raymarchShader("src/Shaders/raymarchVertexShader.shader", 
        "src/Shaders/raymarchFragmentShader.shader", triangleCount, meshCount);
    */
    Shader raytracingShader("src/Shaders/raymarchVertexShader.shader", "src/Shaders/bufferedRaytracingFragmentShader.shader", &scene);
    Shader raytracedImageRendererShader("src/Shaders/raymarchVertexShader.shader", "src/Shaders/raytracedImageRendererShader.glsl", &scene);

    // Raytraced renderer
    Renderer raytracingRenderer("src/shaders/raytraceComputeShader.shader", WINDOW_SIZE_X, WINDOW_SIZE_Y, &scene);


    scene.writeLightsToShader(&raytracingShader);
    scene.writeMaterialsToShader(&raytracingShader);

    scene.generateTriangleBuffer();
    
    // Generating the screen quad on which the raytraced image is rendered
    generateScreenQuad();

    // Generating a VAO for the axes so that they can be rendered easily
    generateAxesVAO();

    unsigned int frame = 0;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (frame % 10 == 0)
            std::cout << "FPS: " << 1.0f / deltaTime << std::endl;

        // Input
        processInput(window);

        // Check whether the UI is enabled
        if (!userInterface.isEnabled() && !inRaytraceMode)
        {
            // Calling the mouse callback
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            camera.mouseCallback(window, xpos, ypos);

            // Process camera movement input
            camera.processInput(window, deltaTime);
        }

        //std::cout << camera.getInformation() << std::endl;
        userInterface.handleInput(window, &camera);

        // Rendering
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        raytracingRenderer.update();

        if (inRaytraceMode)
        {
            /* Raytraced result rendering */
            Shader* usedShader = &raytracedImageRendererShader;

            usedShader->use();
            usedShader->setVector2("screenSize", WINDOW_SIZE_X, WINDOW_SIZE_Y);
            usedShader->setVector3("cameraPosition", camera.getPosition());
            usedShader->setVector3("cameraRotation", camera.getRotation());

            // Making sure the pixel buffer is assigned for the raytracedImageRendererShader
            raytracingRenderer.bindPixelBuffer();

            glBindVertexArray(screenQuadVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }
        else
        {
            /* REGULAR RENDERING */
            solidColorShader.use();

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

            scene.draw(&solidColorShader);
        }

        userInterface.drawUserInterface(&scene, &camera, &raytracingRenderer, &inRaytraceMode);

        // Output
        glfwSwapBuffers(window);
        // Check for input
        glfwPollEvents();

        frame++;
    }

    glDeleteVertexArrays(1, &screenQuadVAO);
    glDeleteBuffers(1, &screenQuadVBO);
    glDeleteBuffers(1, &screenQuadEBO);

    glfwTerminate();

	return 0;
}

void Application::initialiseGLFW()
{
    // Initialising GLFW
    glfwInit();

    // Setting OpenGL version 4.6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    // OpenGL profile: core
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}


void Application::generateScreenQuad()
{
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

    // Generating the required objects
    glGenVertexArrays(1, &screenQuadVAO);
    glGenBuffers(1, &screenQuadVBO);
    glGenBuffers(1, &screenQuadEBO);

    // Making sure everything gets put on this specific VAO
    glBindVertexArray(screenQuadVAO);

    // Binding the buffers
    glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screenQuadEBO);

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

void Application::processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


void Application::generateAxesVAO()
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

    axesVAO = VAO;
}

void Application::drawAxes(unsigned int VAO, Shader* shader, Camera* camera)
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
