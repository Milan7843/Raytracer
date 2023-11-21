#define STB_IMAGE_IMPLEMENTATION

#include "Application.h"


Application::Application(unsigned int WIDTH, unsigned int HEIGHT, bool useShaderCache)
    : WINDOW_SIZE_X(WIDTH), WINDOW_SIZE_Y(HEIGHT), camera(glm::vec3(6.7f, 2.7f, -3.7f))
    , useShaderCache(useShaderCache)
{

}

Application::~Application()
{
    deleteFramebuffer();
    Logger::log("Application instance destroyed");
}

int Application::Start()
{
    Logger::log("Initializing GLFW");

    initialiseGLFW();

    Logger::log("Initializing cache");
    Cache::initialise(useShaderCache);

    // Making a GLFW window
    GLFWwindow* window = glfwCreateWindow(WINDOW_SIZE_X, WINDOW_SIZE_Y, "OpenGL", NULL, NULL);
    if (window == NULL)
    {
        Logger::logError("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    WindowUtility::setWindow(window);

    ImGuiUserInterface userInterface;

    Logger::log("Initializing ImGui");

    userInterface.initialiseImGui(window);

    InputManager::initialise(window);

    Logger::log("Initializing GLAD");
    // GLAD manages function pointers for OpenGL, so we cannot run without it
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        Logger::logError("Failed to initialize GLAD");
        return -1;
    }

    // Enabling depth testing for rasterized view: makes sure objects get drawn on top of each other in the correct order
    glEnable(GL_DEPTH_TEST);

    // Change the viewport size if the window is resized
    glfwSetFramebufferSizeCallback(window, &Callbacks::framebuffer_size_callback);

    Random::initialise();

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

    glm::ivec2 renderedScreenSize{ glm::ivec2(0) };

    Logger::log("Initializing scene");
    SceneManager sceneManager{};

    sceneManager.setAspectRatio(renderedScreenSize.x, renderedScreenSize.y);

    std::string lastLoadedSceneName{};
    FileUtility::readSavedSettings(lastLoadedSceneName);

    Logger::log("Loading scene");
    sceneManager.changeScene(lastLoadedSceneName);
    //sceneManager.getCurrentScene().addCamera(camera);
    //Scene scene{ SceneFileSaver::readSceneFromFile(std::string("Scene 1 - testing")) };

    // Setting the callback for window resizing and camera input
    Callbacks& callbacks = Callbacks::getInstance();
    callbacks.bindSceneManager(&sceneManager);
    glfwSetScrollCallback(window, Callbacks::scrollCallback);

    TextureHandler::loadTexture("couch.png", false);
    TextureHandler::loadTexture("normal_map_test.png", false);
    TextureHandler::loadTexture("test.png", false);

    // Unused shaders... (should remove)
    //Shader uvShader("src/Shaders/uvColorVertexShader.shader", "src/Shaders/uvColorFragmentShader.shader");
    //Shader textureShader("src/Shaders/textureVertexShader.shader", "src/Shaders/textureFragmentShader.shader");
    //Shader raytracingShader("src/Shaders/raymarchVertexShader.shader", "src/Shaders/bufferedRaytracingFragmentShader.shader");

    /* Old raymarching shader (no way it works still...)
    Shader raymarchShader("src/Shaders/raymarchVertexShader.shader", 
        "src/Shaders/raymarchFragmentShader.shader", triangleCount, meshCount);
    */

    Logger::log("Loading shaders");

    Logger::log("Loading axes shader");
    // Shader for drawing axes
    Shader solidColorShader("src/shader_src/solidColorVertexShader.shader", "src/shader_src/solidColorFragmentShader.shader");
    Logger::log("Loading rasterized view shader");
    // Shader for viewing rasterized view
    Shader rasterizedShader("src/shader_src/solidColorVertexShader.shader", "src/shader_src/rasterizedView.shader");
    Logger::log("Loading rendered image shader");
    // Shader for rendering the quad which shows the rendered image
    Shader raytracedImageRendererShader("src/shader_src/raymarchVertexShader.shader", "src/shader_src/raytracedImageRendererShader.glsl");

    // Raytraced renderer
    Logger::log("Loading raytracing shader"); 
    //MultiComputeShader raytracingComputeShader(2, "src/shader_src/raytraceComputeShaderSampledUpdated.shader", "src/shader_src/indirectLightingCalculation.shader");
    MultiComputeShader raytracingComputeShader(1, "src/shader_src/raytraceComputeShaderProbabilistic.shader");
    //ComputeShader raytracingComputeShader("src/shader_src/raytraceComputeShaderSampledUpdated.shader");
    Renderer raytracingRenderer(raytracingComputeShader, WINDOW_SIZE_X, WINDOW_SIZE_Y);

    Logger::log("Loading HDRI renderer");
    HDRIRenderer hdriRenderer("src/shader_src/hdriVertex.shader", "src/shader_src/hdriFragment.shader");

    GizmoRenderer gizmoRenderer("src/shader_src/gizmo_shaders/gizmoVertex.shader",
        "src/shader_src/gizmo_shaders/gizmoGeometry.shader",
        "src/shader_src/gizmo_shaders/gizmoFragment.shader");

    GizmoRenderer objectClickGizmoRenderer("src/shader_src/gizmo_shaders/gizmoClickSelectVertex.shader",
        "src/shader_src/gizmo_shaders/gizmoClickSelectGeometry.shader",
        "src/shader_src/gizmo_shaders/gizmoClickSelectFragment.shader");

    ObjectScreenSelector objectScreenSelector(WINDOW_SIZE_X, WINDOW_SIZE_Y);

    raytracingRenderer.bindSceneManager(&sceneManager);


    Logger::log("Generating triangle buffer");

    sceneManager.getCurrentScene().generateTriangleBuffer();
    
    // Generating the screen quad on which the raytraced image is rendered
    generateScreenQuad();

    OutlineRenderer outlineRenderer(WINDOW_SIZE_X, WINDOW_SIZE_Y, screenQuadVAO);

    // Generating a VAO for the axes so that they can be rendered easily
    generateAxesVAO();

    unsigned int frame = 0;

    Logger::log("Creating BVH handler");
    BVHHandler bvhHandler(
        "src/shader_src/BVH visualisation/bvhVisualisationVertex.shader",
        "src/shader_src/BVH visualisation/bvhVisualisationGeometry.shader",
        "src/shader_src/BVH visualisation/bvhVisualisationFragment.shader"
    );

    Logger::log("Generating initial BVH");
    // Generating the initial BVH
    sceneManager.getCurrentScene().updateBVH();

    //BVHHandler::writeIntoSSBOs(0, 0, 0);

    //bvh.generateFromModel(sceneManager.getCurrentScene().getModels()[0]);

    // TODO change pointer passes to references

    int leftMouseButtonState{ GLFW_RELEASE };
    int rightMouseButtonState{ GLFW_RELEASE };

    ContextMenuSource* contextMenuSource{ nullptr };

    setupFramebuffer(glm::ivec2(0));

    bool popupOpenOnStartClick{ false };

    while (true)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        // Setting viewport size to the rendered screen size
        glViewport(0, 0, renderedScreenSize.x, renderedScreenSize.y);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        bool shouldReRender{ false };

        // TODO: optimise the following lines by adding data changed checks for the lights and materials
        shouldReRender |= sceneManager.getCurrentScene().writeLightsToShader(&rasterizedShader, false);
        shouldReRender |= sceneManager.getCurrentScene().writeMaterialsToShader(&rasterizedShader);

        if (frame % 10 == 0 && false)
            std::cout << "FPS: " << 1.0f / deltaTime << std::endl;

        if (userInterface.getRenderedScreenSize() != renderedScreenSize)
        {
            renderedScreenSize = userInterface.getRenderedScreenSize();
            // The screen size was updated
            sceneManager.setAspectRatio(renderedScreenSize.x, renderedScreenSize.y);
            outlineRenderer.setResolution(renderedScreenSize.x, renderedScreenSize.y);
            objectScreenSelector.setResolution(renderedScreenSize.x, renderedScreenSize.y);
            raytracingRenderer.setResolution(renderedScreenSize.x, renderedScreenSize.y);
            setupFramebuffer(renderedScreenSize);
        }

        int mouseX{ userInterface.getMouseCoordinates().x };
        int mouseY{ userInterface.getMouseCoordinates().y };

        if (sceneManager.hasUnsavedChanges())
        {
            WindowUtility::markUnsavedChanges();
        }
        else
        {
            WindowUtility::markSavedChanges();
        }

        // When the user requests to exit the application
        if (glfwWindowShouldClose(window))
        {
            // First check if there are unsaved changes
            // This will close the window if there aren't any
            // And ask the user to save if there are
            userInterface.requestExit();

            // Set it back to false
            glfwSetWindowShouldClose(window, GLFW_FALSE);
        }

        // Input
        processInput(window);

        InputManager::takeInput(sceneManager.getCurrentScene());

        // Check whether the UI is enabled
        if (userInterface.isMouseOnRenderedScreen() && currentRenderMode != ApplicationRenderMode::RAYTRACED)
        {
            // Process camera movement input
            shouldReRender |= sceneManager.getCurrentScene().getActiveCamera().processInput(window,
                sceneManager.getCurrentScene(), (double)mouseX, (double)mouseY, deltaTime);
        }

        //std::cout << camera.getInformation() << std::endl;
        userInterface.handleInput(window, sceneManager.getCurrentScene().getActiveCamera());

        // Rendering
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        raytracingRenderer.update(deltaTime, currentRenderMode == ApplicationRenderMode::REALTIME_RAYTRACED, shouldReRender, sceneManager.getCurrentScene());

        if (currentRenderMode == ApplicationRenderMode::RAYTRACED || currentRenderMode == ApplicationRenderMode::REALTIME_RAYTRACED)
        {
            /* Raytraced result rendering */
            Shader* usedShader = &raytracedImageRendererShader;

            usedShader->use();
            usedShader->setVector2("screenSize", (float)renderedScreenSize.x, (float)renderedScreenSize.y);
            //usedShader->setVector3("cameraRotation", CoordinateUtility::vec3ToGLSLVec3(camera.getRotation()));

            // Making sure the pixel buffer is assigned for the raytracedImageRendererShader
            raytracingRenderer.bindPixelBuffer();

            glBindVertexArray(screenQuadVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }
        else
        {
            bool popupOpen{ ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopup) };
            if (InputManager::keyPressed(InputManager::InputKey::CLICK))
            {
                popupOpenOnStartClick = popupOpen;
            }
            if (InputManager::keyUp(InputManager::InputKey::CLICK))
            {
                popupOpenOnStartClick = false;
            }

            // Checking for a click on an object on mouse click and mouse not on GUI
            if (!popupOpen && !popupOpenOnStartClick && !userInterface.isMouseOnGUI() && userInterface.isEnabled() && userInterface.isMouseOnRenderedScreen())
            {
                bool leftMouse = leftMouseButtonState == GLFW_PRESS &&
                    glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE;

                bool rightMouse = rightMouseButtonState == GLFW_PRESS && 
                    glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE;

                leftMouseButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
                rightMouseButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

                if (leftMouse)
                {
                    unsigned int objectClickedID = objectScreenSelector.checkObjectClicked(sceneManager.getCurrentScene(), mouseX, mouseY, objectClickGizmoRenderer);

                    // An object was clicked
                    // Using the data to select/deselect an object
                    sceneManager.getCurrentScene().markSelected(objectClickedID);
                }
                // Attempting to open the context menu of the selected object
                if (rightMouse) {
                    ContextMenuSource* source{ sceneManager.getCurrentScene().getContextMenuSourceFromSelected() };
                    if (source != nullptr)
                        source->openContextMenu();
                }
            }

            glDisable(GL_DEPTH_TEST);
            // Drawing the HDRI (skybox) if using it as a background is enabled
            if (*sceneManager.getCurrentScene().getUseHDRIAsBackgroundPointer() && sceneManager.getCurrentScene().hasHDRI())
                hdriRenderer.drawHDRI(sceneManager.getCurrentScene().getHDRI()->textureID, sceneManager.getCurrentScene().getActiveCamera());

            glEnable(GL_DEPTH_TEST);

            /* REGULAR RENDERING */
            solidColorShader.use();

            // Drawing axes
            drawAxes(axesVAO, &solidColorShader, &sceneManager.getCurrentScene().getActiveCamera());

            /* Rasterized scene rendering */
            sceneManager.getCurrentScene().draw(&rasterizedShader, currentRasterizedDebugMode);

            sceneManager.getCurrentScene().drawGizmos(gizmoRenderer);

            // Rendering the outlines for selected objects
            outlineRenderer.render(sceneManager.getCurrentScene());

            bvhHandler.draw(sceneManager.getCurrentScene(), raytracingRenderer.getBVHRenderMode());

            // Rendering a preview of the colours used to select an object by clicking on it
            //objectScreenSelector.renderTexturePreview(sceneManager.getCurrentScene(), screenQuadVAO, objectClickGizmoRenderer);
        }


        // Render to the screen again
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Setting viewport size to full screen
        glViewport(0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y);

        userInterface.drawUserInterface(window,
            sceneManager,
            sceneManager.getCurrentScene().getActiveCamera(),
            raytracingRenderer,
            currentRenderMode,
            currentRasterizedDebugMode,
            contextMenuSource,
            screenTexture
        );

        InputManager::updateKeyBindsPreviousValues();

        // Output
        glfwSwapBuffers(window);
        // Check for input
        glfwPollEvents();

        frame++;

        // If an exit is requested and everything is okay to exit
        if (userInterface.isExitOkay())
        {
            break;
        }
    }

    // Saving the last opened scene
    sceneManager.saveSceneLoaded();

    glDeleteVertexArrays(1, &screenQuadVAO);
    glDeleteBuffers(1, &screenQuadVBO);
    glDeleteBuffers(1, &screenQuadEBO);

    Random::terminate();

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
    /*
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    */
}

void Application::setupFramebuffer(glm::ivec2 renderedScreenSize)
{
    // Cleaning up previous buffers
    deleteFramebuffer();

    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glGenTextures(1, &screenTexture);


    // Setting up the texture
    glBindTexture(GL_TEXTURE_2D, screenTexture);

    // Making it an empty image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, renderedScreenSize.x, renderedScreenSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    // Setting texture filter settings
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Generate and bind the depth buffer
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);

    // Set the storage for the depth buffer
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WINDOW_SIZE_X, WINDOW_SIZE_Y);

    // Attach the depth buffer to the framebuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    // Binding the texture to be drawn to
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, screenTexture, 0);

    // Set the list of draw buffers (only draw to the first slot
    GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);


    // Unbinding the frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Application::deleteFramebuffer()
{
    glDeleteTextures(1, &screenTexture);
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteRenderbuffers(1, &depthBuffer);
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
    projection = camera->getProjectionMatrix();
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
