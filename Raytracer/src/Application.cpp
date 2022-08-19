#define STB_IMAGE_IMPLEMENTATION

#include "Application.h"


Application::Application(unsigned int WIDTH, unsigned int HEIGHT)
    : WINDOW_SIZE_X(WIDTH), WINDOW_SIZE_Y(HEIGHT), camera(glm::vec3(6.7f, 2.7f, -3.7f))
{

}

Application::~Application()
{
    Logger::log("Application instance destroyed");
}

int Application::Start()
{
    initialiseGLFW();

    // Making a GLFW window
    GLFWwindow* window = glfwCreateWindow(WINDOW_SIZE_X, WINDOW_SIZE_Y, "OpenGL", NULL, NULL);
    if (window == NULL)
    {
        Logger::logError("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    ImGuiUserInterface userInterface;

    userInterface.initialiseImGui(window);

    // GLAD manages function pointers for OpenGL, so we cannot run without it
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        Logger::logError("Failed to initialize GLAD");
        return -1;
    }

    // Setting viewport size
    glViewport(0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y);

    // Enabling depth testing for rasterized view: makes sure objects get drawn on top of each other in the correct order
    glEnable(GL_DEPTH_TEST);

    // Change the viewport size if the window is resized
    glfwSetFramebufferSizeCallback(window, &Callbacks::framebuffer_size_callback);

    /*
    // Making a scene
    Scene scene = Scene();

    // MATERIALS
    Material whiteMaterial("White", glm::vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.0f, 0.0f);
    Material reflectiveMaterial("Reflective", glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.0f, 0.0f);
    Material transparentMaterial("Transparent", glm::vec3(0.0f, 1.0f, 0.0f), 0.5f, 1.0f, 0.01f);
    Material roseMaterial("Rose matte", glm::vec3(0.8f, 0.2f, 0.3f), 0.00000001f, 0.0f, 0.0f);

    scene.addMaterial(whiteMaterial);
    scene.addMaterial(reflectiveMaterial);
    scene.addMaterial(transparentMaterial);
    scene.addMaterial(roseMaterial);

    // Adding our test models: !! MUST BE TRIANGULATED !!
    Model* plane = scene.addModel("src/models/plane.obj", 0);
    Model* icosphere = scene.addModel("src/models/axes.obj", 1);

    // Transforming the icosphere
    icosphere->setPosition(glm::vec3(1.0f, 0.6f, 2.0f));
    icosphere->scale(0.6f);
    
    Sphere* sphere1 = scene.addSphere(glm::vec3(0.0f, 1.0f, 0.0f), 0.8f, 2);
    Sphere* sphere2 = scene.addSphere(glm::vec3(3.0f, 1.0f, 0.0f), 1.4f, 1);
    Sphere* sphere3 = scene.addSphere(glm::vec3(2.0f, 1.0f, 1.0f), 0.6f, 3);

    // LIGHTS
    PointLight pointLight1(glm::vec3(0.0f, 1.8f, 1.8f), glm::vec3(1.0f, 0.0f, 0.0f), 2.0f);
    PointLight pointLight2(glm::vec3(2.0f, 1.8f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 2.0f);
    DirectionalLight directionalLight1(glm::vec3(0.707f, -0.707f, 0.0f), glm::vec3(1.0f, 1.0f, 0.9f), 0.3f);
    AmbientLight ambientLight1(glm::vec3(0.8f, 0.8f, 1.0f), 0.02f);

    scene.addLight(pointLight1);
    scene.addLight(pointLight2);
    scene.addLight(directionalLight1);
    scene.addLight(ambientLight1);
    
    SceneFileSaver::writeSceneToFile(scene, std::string("Scene 1 - testing"));
    */

    SceneManager sceneManager{};

    std::string lastLoadedSceneName{};
    FileUtility::readSavedSettings(lastLoadedSceneName);

    sceneManager.changeScene(lastLoadedSceneName);
    //sceneManager.getCurrentScene().addCamera(camera);
    //Scene scene{ SceneFileSaver::readSceneFromFile(std::string("Scene 1 - testing")) };

    // Setting the callback for window resizing and camera input
    Callbacks& callbacks = Callbacks::getInstance();
    callbacks.bindSceneManager(&sceneManager);

    // Unused shaders... (should remove)
    //Shader uvShader("src/Shaders/uvColorVertexShader.shader", "src/Shaders/uvColorFragmentShader.shader");
    //Shader textureShader("src/Shaders/textureVertexShader.shader", "src/Shaders/textureFragmentShader.shader");
    //Shader raytracingShader("src/Shaders/raymarchVertexShader.shader", "src/Shaders/bufferedRaytracingFragmentShader.shader");

    /* Old raymarching shader (no way it works still...)
    Shader raymarchShader("src/Shaders/raymarchVertexShader.shader", 
        "src/Shaders/raymarchFragmentShader.shader", triangleCount, meshCount);
    */

    // Shader for drawing axes
    Shader solidColorShader("src/Shaders/solidColorVertexShader.shader", "src/Shaders/solidColorFragmentShader.shader");
    // Shader for viewing rasterized view
    Shader rasterizedShader("src/Shaders/solidColorVertexShader.shader", "src/Shaders/rasterizedView.shader");
    // Shader for rendering the quad which shows the rendered image
    Shader raytracedImageRendererShader("src/Shaders/raymarchVertexShader.shader", "src/Shaders/raytracedImageRendererShader.glsl");

    // Raytraced renderer
    Renderer raytracingRenderer("src/shaders/raytraceComputeShaderSampled.shader", WINDOW_SIZE_X, WINDOW_SIZE_Y);

    HDRIRenderer hdriRenderer("src/shaders/hdriVertex.shader", "src/shaders/hdriFragment.shader");

    raytracingRenderer.bindSceneManager(&sceneManager);

    sceneManager.getCurrentScene().generateTriangleBuffer();
    
    // Generating the screen quad on which the raytraced image is rendered
    generateScreenQuad();

    // Generating a VAO for the axes so that they can be rendered easily
    generateAxesVAO();

    unsigned int frame = 0;

    Logger::logWarning("change pointer passes to references");

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // TODO: optimise the following lines by adding data changed checks for the lights and materials
        sceneManager.getCurrentScene().writeLightsToShader(&rasterizedShader, false);
        sceneManager.getCurrentScene().writeMaterialsToShader(&rasterizedShader);
        //raytracingRenderer.updateMeshData(&scene);
        //scene.checkObjectUpdates(&rasterizedShader);


        if (frame % 10 == 0 && false)
            std::cout << "FPS: " << 1.0f / deltaTime << std::endl;

        // Input
        processInput(window);

        // Check whether the UI is enabled
        if (!userInterface.isEnabled() && !inRaytraceMode)
        {
            // Calling the mouse callback
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            sceneManager.getCurrentScene().getActiveCamera().mouseCallback(window, xpos, ypos);

            // Process camera movement input
            sceneManager.getCurrentScene().getActiveCamera().processInput(window, deltaTime);
        }

        //std::cout << camera.getInformation() << std::endl;
        userInterface.handleInput(window, sceneManager.getCurrentScene().getActiveCamera());

        // Rendering
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        raytracingRenderer.update(deltaTime);

        if (inRaytraceMode)
        {
            /* Raytraced result rendering */
            Shader* usedShader = &raytracedImageRendererShader;

            usedShader->use();
            usedShader->setVector2("screenSize", WINDOW_SIZE_X, WINDOW_SIZE_Y);
            //usedShader->setVector3("cameraRotation", CoordinateUtility::vec3ToGLSLVec3(camera.getRotation()));

            // Making sure the pixel buffer is assigned for the raytracedImageRendererShader
            raytracingRenderer.bindPixelBuffer();

            glBindVertexArray(screenQuadVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }
        else
        {
            // Drawing the HDRI (skybox) if using it as a background is enabled
            if (*sceneManager.getCurrentScene().getUseHDRIAsBackgroundPointer())
                hdriRenderer.drawHDRI(sceneManager.getCurrentScene().getHDRI(), sceneManager.getCurrentScene().getActiveCamera(), WINDOW_SIZE_X, WINDOW_SIZE_Y);

            /* REGULAR RENDERING */
            solidColorShader.use();

            // Drawing axes
            drawAxes(axesVAO, &solidColorShader, &sceneManager.getCurrentScene().getActiveCamera());

            /* Rasterized scene rendering */
            rasterizedShader.use();

            // Uniforms
            float time = glfwGetTime();

            // View matrix
            glm::mat4 view = glm::mat4(1.0f);
            view = sceneManager.getCurrentScene().getActiveCamera().getViewMatrix();
            rasterizedShader.setMat4("view", view);

            // Projection matrix
            glm::mat4 projection;
            projection = sceneManager.getCurrentScene().getActiveCamera().getProjectionMatrix(WINDOW_SIZE_X, WINDOW_SIZE_Y);
            rasterizedShader.setMat4("projection", projection);

            sceneManager.getCurrentScene().draw(&rasterizedShader);
        }

        userInterface.drawUserInterface(window, sceneManager, sceneManager.getCurrentScene().getActiveCamera(), raytracingRenderer, &inRaytraceMode);

        // Output
        glfwSwapBuffers(window);
        // Check for input
        glfwPollEvents();

        frame++;
    }

    // Saving the last opened scene
    FileUtility::saveSettings(*sceneManager.getCurrentScene().getNamePointer());

    glDeleteVertexArrays(1, &screenQuadVAO);
    glDeleteBuffers(1, &screenQuadVBO);
    glDeleteBuffers(1, &screenQuadEBO);

    Logger::stop();

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
