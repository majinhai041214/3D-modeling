#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>        // std::sin

// 窗口大小改变时的回调
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// ==================== 顶点着色器 & 片段着色器 ====================
// 加了一个 uniform: xOffset，用来左右移动
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;    // 顶点位置
layout (location = 1) in vec3 aColor;  // 顶点颜色

out vec3 ourColor;

uniform float xOffset;                 // X 方向偏移量

void main() {
    vec3 pos = aPos;
    pos.x += xOffset;                  // 左右移动
    gl_Position = vec4(pos, 1.0);
    ourColor = aColor;
}
)";

// 加了一个 uniform: uTime，用来控制闪烁节奏
const char* fragmentShaderSource = R"(
#version 330 core
in vec3 ourColor;
out vec4 FragColor;

uniform float uTime;   // 时间（秒）

void main() {
    // 闪烁：在 0.2 ~ 1.0 之间来回变化
    float strength = 0.6 + 0.4 * sin(uTime * 3.0); 
    vec3 color = ourColor * strength;
    FragColor = vec4(color, 1.0);
}
)";
// ================================================================

int main() {
    // 1. 初始化 GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // 指定 OpenGL 版本：3.3 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 2. 创建窗口
    GLFWwindow* window = glfwCreateWindow(800, 600, "Moving & Blinking Triangle", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 3. 初始化 GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // ==================== 顶点数据：位置 + 颜色 ====================
    float vertices[] = {
        // 位置              // 颜色
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 左下——红
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 右下——绿
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // 顶点——蓝
    };

    // ==================== VAO & VBO ====================
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)0
    );
    glEnableVertexAttribArray(0);

    // 颜色属性
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)(3 * sizeof(float))
    );
    glEnableVertexAttribArray(1);

    // ==================== 着色器编译 & 链接 ====================
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 先拿到 uniform 的位置（只用查一次）
    int timeLoc   = glGetUniformLocation(shaderProgram, "uTime");
    int offsetLoc = glGetUniformLocation(shaderProgram, "xOffset");

    // ==================== 渲染循环 ====================
    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        // 当前时间（秒）
        float timeValue = static_cast<float>(glfwGetTime());

        // 1）左右移动：在 [-0.5, 0.5] 之间往返
        float xOffset = 0.5f * std::sin(timeValue);
        glUniform1f(offsetLoc, xOffset);

        // 2）闪烁：把时间传给片段着色器
        glUniform1f(timeLoc, timeValue);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
