#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <stb_image/stb_image.h>

#include <shader_s.h>

#include <camera.h>

#include "imgui.h"

#include "backends/imgui_impl_glfw.h"

#include "backends/imgui_impl_opengl3.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;

const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

float lastX = SCR_WIDTH / 2.0f;

float lastY = SCR_HEIGHT / 2.0f;

bool firstMouse = true;

bool mouseCaptured = true;

bool tabWasPressed = false;

float deltaTime = 5.5f;

float lastFrame = 0.0f;

float shininess = 32.0f;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

unsigned int loadTexture(const char* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

    if (data)
    {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            format,
            width,
            height,
            0,
            format,
            GL_UNSIGNED_BYTE,
            data
        );
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load texture: " << path << "\n";
        stbi_image_free(data);
    }

    return textureID;
}

int main()

{

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

#endif

    GLFWwindow* window = glfwCreateWindow(

        SCR_WIDTH,
        SCR_HEIGHT,
        "why am I doing that to myself",
        nullptr,
        nullptr
    );

    if (window == nullptr)

    {
        std::cout << "Failed to create GLFW window\n";

        glfwTerminate();

        return -1;

    }

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))

    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);

    ImGui_ImplOpenGL3_Init("#version 330");

    glEnable(GL_DEPTH_TEST);

    Shader lightingShader(

        RESOURCES_PATH "1.colors.vs",

        RESOURCES_PATH "1.colors.fs"

    );

    Shader lightCubeShader(
        RESOURCES_PATH "1.light_cube.vs",
        RESOURCES_PATH "1.light_cube.fs"

    );

    unsigned int diffuseMap = loadTexture(RESOURCES_PATH "container2.png");

    unsigned int specularMap = loadTexture(RESOURCES_PATH "container2_specular.png");

    float vertices[] =
    {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f

    };

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

    glVertexAttribPointer(

        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(float),

        (void*)0
    );

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(float),
        (void*)(3 * sizeof(float))

    );

    glEnableVertexAttribArray(1);

    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(float),
        (void*)(6 * sizeof(float))

    );

    glEnableVertexAttribArray(2);

    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(float),
        (void*)0
    );
    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window))

    {

        float currentFrame =

            static_cast<float>(glfwGetTime());

        deltaTime =
            currentFrame - lastFrame;

        lastFrame = currentFrame;

        processInput(window);

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

        ImGui_ImplOpenGL3_NewFrame();

        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();

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

        ImGui::Separator();

        ImGui::Text("Material");

        ImGui::SliderFloat(
            "Shininess",
            &shininess,
            1.0f,
            256.0f
        );

        ImGui::Separator();

        ImGui::Text("Press TAB to toggle mouse capture.");

        ImGui::End();

        lightingShader.use();

        lightingShader.setVec3(
            "viewPos",
            camera.Position
        );

        lightingShader.setVec3(
            "light.position",
            lightPos
        );

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        lightingShader.setInt(
            "material.diffuse",
            0
        );

        lightingShader.setInt(
            "material.specular",
            1
        );

        lightingShader.setFloat(
            "material.shininess",
            shininess
        );

        float time =
            static_cast<float>(glfwGetTime());

        glm::vec3 lightColor(
            sin(time * 2.0f) * 0.5f + 0.5f,

            sin(time * 0.7f) * 0.5f + 0.5f,

            sin(time * 1.3f) * 0.5f + 0.5f

        );

        glm::vec3 diffuseColor =
            lightColor;

        glm::vec3 ambientColor =
            diffuseColor * glm::vec3(0.3f);

        lightingShader.setVec3(
            "light.ambient",
            ambientColor
        );

        lightingShader.setVec3(
            "light.diffuse",
            diffuseColor
        );

        lightingShader.setVec3(
            "light.specular",
            1.0f,
            1.0f,
            1.0f
        );

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

        glm::mat4 model =
            glm::mat4(1.0f);

        lightingShader.setMat4(
            "model",
            model
        );

        glBindVertexArray(cubeVAO);

        glDrawArrays(
            GL_TRIANGLES,
            0,
            36
        );

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

        model = glm::translate(
            model,
            lightPos
        );

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

        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(
            ImGui::GetDrawData()
        );

        glfwSwapBuffers(window);

        glfwPollEvents();

    }

    glDeleteTextures(1, &diffuseMap);
    glDeleteTextures(1, &specularMap);
    glDeleteVertexArrays(1, &cubeVAO);

    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;

}

void processInput(GLFWwindow* window)
{

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)

    {
        glfwSetWindowShouldClose(window, true);

    }

    bool tabPressed =
        glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS;

    if (tabPressed && !tabWasPressed)

    {
        mouseCaptured = !mouseCaptured;

        if (mouseCaptured)
        {
            glfwSetInputMode(
                window,
                GLFW_CURSOR,
                GLFW_CURSOR_DISABLED
            );
            firstMouse = true;

        }
        else
        {
            glfwSetInputMode(
                window,
                GLFW_CURSOR,
                GLFW_CURSOR_NORMAL
            );
        }
    }

    tabWasPressed = tabPressed;

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

void mouse_callback(
    GLFWwindow* window,
    double xposIn,
    double yposIn
)
{

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
