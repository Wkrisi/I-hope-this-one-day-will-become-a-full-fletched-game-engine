#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader_s.h>
#include <camera.h>

// =========================
// ImGui
// =========================
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <iostream>


// =========================
// Function declarations
// =========================

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);


// =========================
// Settings
// =========================

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


// =========================
// Camera
// =========================

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

bool firstMouse = true;


// =========================
// Mouse mode
// =========================

// true  = mouse controls camera
// false = mouse is free and can interact with ImGui
bool mouseCaptured = true;

// Used to detect a single TAB press instead of
// repeatedly toggling while TAB is held down.
bool tabWasPressed = false;


// =========================
// Timing
// =========================

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float shininess = 32.0f;

// =========================
// Lighting
// =========================

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);


int main()
{
    // =========================================================
    // GLFW: initialize and configure
    // =========================================================

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif


    // =========================================================
    // Create GLFW window
    // =========================================================

    GLFWwindow* window = glfwCreateWindow(
        SCR_WIDTH,
        SCR_HEIGHT,
        "why am I doing that to myself",
        NULL,
        NULL
    );

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(
        window,
        framebuffer_size_callback
    );

    glfwSetCursorPosCallback(
        window,
        mouse_callback
    );

    glfwSetScrollCallback(
        window,
        scroll_callback
    );


    // =========================================================
    // Start with the mouse captured
    // =========================================================

    glfwSetInputMode(
        window,
        GLFW_CURSOR,
        GLFW_CURSOR_DISABLED
    );


    // =========================================================
    // GLAD: load OpenGL functions
    // =========================================================

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // =========================================================
    // ImGui initialization
    // =========================================================

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    //
    // ImGuiIO& io = ImGui::GetIO();
    // (void)io;
    //
    // Use ImGui's dark theme.
    ImGui::StyleColorsDark();

    // Connect ImGui to GLFW.
    ImGui_ImplGlfw_InitForOpenGL(window, true);

    // Tell ImGui we're using OpenGL 3.3.
    ImGui_ImplOpenGL3_Init("#version 330");


    // =========================================================
    // OpenGL state
    // =========================================================

    glEnable(GL_DEPTH_TEST);


    // =========================================================
    // Shaders
    // =========================================================

    Shader lightingShader(
        RESOURCES_PATH "1.colors.vs",
        RESOURCES_PATH "1.colors.fs"
    );

    Shader lightCubeShader(
        RESOURCES_PATH "1.light_cube.vs",
        RESOURCES_PATH "1.light_cube.fs"
    );


    // =========================================================
    // Vertex data
    // positions + normals
    // =========================================================

    float vertices[] = {

        // positions          // normals

        // back face
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        // front face
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        // left face
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

        // right face
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        // bottom face
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        // top face
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };


    // =========================================================
    // Cube VAO + VBO
    // =========================================================

    unsigned int VBO;
    unsigned int cubeVAO;

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    glBindVertexArray(cubeVAO);


    // Position attribute
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);


    // Normal attribute
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)(3 * sizeof(float))
    );

    glEnableVertexAttribArray(1);


    // =========================================================
    // Light cube VAO
    // =========================================================

    unsigned int lightCubeVAO;

    glGenVertexArrays(1, &lightCubeVAO);

    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);


    // =========================================================
    // Render loop
    // =========================================================

    while (!glfwWindowShouldClose(window))
    {
        // =====================================================
        // Timing
        // =====================================================

        float currentFrame =
            static_cast<float>(glfwGetTime());

        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        // =====================================================
        // Input
        // =====================================================

        processInput(window);


        // =====================================================
        // Clear screen
        // =====================================================

        glClearColor(
            0.1f,
            0.1f,
            0.1f,
            1.0f
        );

        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );


        // =====================================================
        // Start ImGui frame
        // =====================================================

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        // =====================================================
        // Our ImGui Light window
        // =====================================================

        ImGui::Begin("Light");

        ImGui::Text("Move the light around!");

        ImGui::Separator();

        ImGui::SliderFloat(
            "X",
            &lightPos.x,
            -5.0f,
            5.0f
        );

        ImGui::SliderFloat(
            "Y",
            &lightPos.y,
            -5.0f,
            5.0f
        );

        ImGui::SliderFloat(
            "Z",
            &lightPos.z,
            -5.0f,
            5.0f
        );

        ImGui::Separator();

        ImGui::Text(
            "Position: %.2f, %.2f, %.2f",
            lightPos.x,
            lightPos.y,
            lightPos.z
        );

        if (ImGui::Button("Reset Light"))
        {
            lightPos = glm::vec3(
                1.2f,
                1.0f,
                2.0f
            );
        }

        ImGui::Text("Press TAB to toggle mouse capture.");
        
        ImGui::Separator();

        ImGui::Text("Lighting");

        ImGui::SliderFloat("shininess", &shininess, 1.0, 256.0f);

        ImGui::End();


        // =====================================================
        // Lighting shader
        // =====================================================

        lightingShader.use();

        lightingShader.setVec3(
            "objectColor",
            1.0f,
            0.8f,
            0.91f
        );

        lightingShader.setVec3(
            "lightColor",
            1.0f,
            0.2f,
            0.0f
        );

        // THIS is the important part.
        //
        // lightPos may have been changed by ImGui
        // this frame.
        lightingShader.setVec3(
            "lightPos",
            lightPos
        );

        lightingShader.setVec3("viewPos", camera.Position);

        lightingShader.setFloat("shininess", shininess);
        // =====================================================
        // Projection + view
        // =====================================================

        glm::mat4 projection =
            glm::perspective(
                glm::radians(camera.Zoom),
                (float)SCR_WIDTH / (float)SCR_HEIGHT,
                0.1f,
                100.0f
            );

        glm::mat4 view =
            camera.GetViewMatrix();

        lightingShader.setMat4(
            "projection",
            projection
        );

        lightingShader.setMat4(
            "view",
            view
        );


        // =====================================================
        // Model transformation
        // =====================================================

        glm::mat4 model =
            glm::mat4(1.0f);

        lightingShader.setMat4(
            "model",
            model
        );


        // =====================================================
        // Draw lit cube
        // =====================================================

        glBindVertexArray(cubeVAO);

        glDrawArrays(
            GL_TRIANGLES,
            0,
            36
        );


        // =====================================================
        // Draw light cube
        // =====================================================

        lightCubeShader.use();

        lightCubeShader.setMat4(
            "projection",
            projection
        );

        lightCubeShader.setMat4(
            "view",
            view
        );


        model = glm::mat4(1.0f);

        // Put the small light cube at lightPos.
        model = glm::translate(
            model,
            lightPos
        );

        // Make it smaller.
        model = glm::scale(
            model,
            glm::vec3(0.2f)
        );

        lightCubeShader.setMat4(
            "model",
            model
        );


        glBindVertexArray(lightCubeVAO);

        glDrawArrays(
            GL_TRIANGLES,
            0,
            36
        );


        // =====================================================
        // Render ImGui
        // =====================================================

        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(
            ImGui::GetDrawData()
        );


        // =====================================================
        // Finish frame
        // =====================================================

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // =========================================================
    // Cleanup OpenGL resources
    // =========================================================

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);

    glDeleteBuffers(1, &VBO);


    // =========================================================
    // Cleanup ImGui
    // =========================================================

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext();


    // =========================================================
    // Cleanup GLFW
    // =========================================================

    glfwTerminate();

    return 0;
}


// =============================================================
// Process keyboard input
// =============================================================

void processInput(GLFWwindow* window)
{
    // ---------------------------------------------------------
    // Escape
    // ---------------------------------------------------------

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }


    // ---------------------------------------------------------
    // TAB = toggle mouse capture
    // ---------------------------------------------------------

    bool tabPressed =
        glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS;

    if (tabPressed && !tabWasPressed)
    {
        mouseCaptured = !mouseCaptured;

        if (mouseCaptured)
        {
            // Camera mode

            glfwSetInputMode(
                window,
                GLFW_CURSOR,
                GLFW_CURSOR_DISABLED
            );

            // Prevent a huge camera jump when
            // the mouse becomes captured again.
            firstMouse = true;
        }
        else
        {
            // ImGui/editor mode

            glfwSetInputMode(
                window,
                GLFW_CURSOR,
                GLFW_CURSOR_NORMAL
            );
        }
    }

    tabWasPressed = tabPressed;


    // ---------------------------------------------------------
    // Camera movement
    // ---------------------------------------------------------

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(
            FORWARD,
            deltaTime
        );
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(
            BACKWARD,
            deltaTime
        );
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(
            LEFT,
            deltaTime
        );
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(
            RIGHT,
            deltaTime
        );
    }
}


// =============================================================
// Window resize callback
// =============================================================

void framebuffer_size_callback(
    GLFWwindow* window,
    int width,
    int height
)
{
    glViewport(
        0,
        0,
        width,
        height
    );
}


// =============================================================
// Mouse callback
// =============================================================

void mouse_callback(
    GLFWwindow* window,
    double xposIn,
    double yposIn
)
{
    // If we're using the mouse for ImGui,
    // don't rotate the camera.
    if (!mouseCaptured)
    {
        return;
    }


    float xpos =
        static_cast<float>(xposIn);

    float ypos =
        static_cast<float>(yposIn);


    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;

        firstMouse = false;
    }


    float xoffset =
        xpos - lastX;

    float yoffset =
        lastY - ypos;


    lastX = xpos;
    lastY = ypos;


    camera.ProcessMouseMovement(
        xoffset,
        yoffset
    );
}


// =============================================================
// Mouse scroll callback
// =============================================================

void scroll_callback(
    GLFWwindow* window,
    double xoffset,
    double yoffset
)
{
    camera.ProcessMouseScroll(
        static_cast<float>(yoffset)
    );
}

