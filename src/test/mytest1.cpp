// main.cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

// 一个顶点：位置 + 颜色
struct Vertex {
    float x, y;      // 位置（NDC 坐标：-1~1）
    float r, g, b;   // 颜色
};

// 全局状态
std::vector<Vertex> g_vertices;    // 所有画过的点
float g_currentColor[3] = {1.0f, 0.0f, 0.0f}; // 默认画笔颜色：红色
bool  g_mousePressed = false;     // 鼠标左键是否按下
int   g_windowWidth  = 800;
int   g_windowHeight = 600;

// 将屏幕坐标转换为 NDC 坐标
// screenX: [0, width]  -> NDCx: [-1, 1]
// screenY: [0, height] -> NDCy: [-1, 1]（注意 OpenGL 原点在左下，屏幕原点在左上）
void screenToNDC(double screenX, double screenY, float& ndcX, float& ndcY)
{
    ndcX = static_cast<float>( (screenX / g_windowWidth) * 2.0 - 1.0 );
    ndcY = static_cast<float>( ( (g_windowHeight - screenY) / g_windowHeight ) * 2.0 - 1.0 );
}

// 窗口尺寸变化时的回调
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    g_windowWidth = width;
    g_windowHeight = height;
    glViewport(0, 0, width, height);
}

// 鼠标按钮事件回调
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            g_mousePressed = true;

            // 按下时立即记录一个点
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            float ndcX, ndcY;
            screenToNDC(xpos, ypos, ndcX, ndcY);

            Vertex v;
            v.x = ndcX;
            v.y = ndcY;
            v.r = g_currentColor[0];
            v.g = g_currentColor[1];
            v.b = g_currentColor[2];
            g_vertices.push_back(v);
        }
        else if (action == GLFW_RELEASE)
        {
            g_mousePressed = false;
        }
    }
}

// 鼠标移动回调
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (g_mousePressed)
    {
        float ndcX, ndcY;
        screenToNDC(xpos, ypos, ndcX, ndcY);

        Vertex v;
        v.x = ndcX;
        v.y = ndcY;
        v.r = g_currentColor[0];
        v.g = g_currentColor[1];
        v.b = g_currentColor[2];

        g_vertices.push_back(v);
    }
}

// 键盘输入：切换颜色 & 清屏 & 退出
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        g_currentColor[0] = 1.0f; g_currentColor[1] = 0.0f; g_currentColor[2] = 0.0f; // 红
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        g_currentColor[0] = 0.0f; g_currentColor[1] = 1.0f; g_currentColor[2] = 0.0f; // 绿
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        g_currentColor[0] = 0.0f; g_currentColor[1] = 0.0f; g_currentColor[2] = 1.0f; // 蓝
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        g_vertices.clear(); // 清空所有点，相当于清空画布
    }
}

// 简单的工具函数：检查着色器编译/链接错误
void checkCompileErrors(GLuint shader, const std::string& type)
{
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: "
                      << type << "\n" << infoLog << "\n";
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: "
                      << type << "\n" << infoLog << "\n";
        }
    }
}

int main()
{
    // 1. 初始化 GLFW
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW\n";
        return -1;
    }

    // 使用 OpenGL 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 2. 创建窗口
    GLFWwindow* window = glfwCreateWindow(g_windowWidth, g_windowHeight,
                                          "OpenGL Paint Board", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 3. 初始化 GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    // 设置视口和回调
    glViewport(0, 0, g_windowWidth, g_windowHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    // 4. 顶点着色器 & 片元着色器（非常简单）
    const char* vertexShaderSource = R"(#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vColor;

void main()
{
    gl_Position = vec4(aPos, 0.0, 1.0);
    vColor = aColor;
}
)";

    const char* fragmentShaderSource = R"(#version 330 core
in vec3 vColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(vColor, 1.0);
}
)";

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    checkCompileErrors(vertexShader, "VERTEX");

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    checkCompileErrors(fragmentShader, "FRAGMENT");

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkCompileErrors(shaderProgram, "PROGRAM");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 5. 创建 VAO / VBO（我们会每帧动态更新 VBO 的内容）
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // 顶点属性 0：位置 (vec2)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // 顶点属性 1：颜色 (vec3)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // 画笔粗细
    glPointSize(6.0f);

    // 6. 主循环
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // 清屏背景：深灰
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);    // 白色背景
        glClear(GL_COLOR_BUFFER_BIT);


        // 只有有点要画时才上传数据
        if (!g_vertices.empty())
        {
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);

            // 每帧更新 VBO（所有点）
            glBufferData(GL_ARRAY_BUFFER,
                         g_vertices.size() * sizeof(Vertex),
                         g_vertices.data(),
                         GL_DYNAMIC_DRAW);

            glUseProgram(shaderProgram);
            glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(g_vertices.size()));

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 7. 清理资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
