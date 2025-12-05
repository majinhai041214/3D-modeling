#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

// 顶点结构定义
struct Vertex {
    float x, y;      // 位置（NDC）
    float r, g, b;   // 颜色
};

// 所有绘制的点
std::vector<Vertex> g_points;

// 当前画笔颜色（默认黑色）
float g_color[3] = {0.0f, 0.0f, 0.0f};

// 鼠标状态
bool g_mousePressed = false;

// 窗口大小
int g_width = 800;
int g_height = 600;

// 将屏幕坐标转换为 NDC 坐标
void screenToNDC(double sx, double sy, float& nx, float& ny) {
    nx = (float)(sx / g_width) * 2.0f - 1.0f;
    ny = (float)((g_height - sy) / g_height) * 2.0f - 1.0f;
}

// 窗口尺寸变化时更新视口
void framebuffer_size_callback(GLFWwindow*, int width, int height)
{
    g_width = width;
    g_height = height;
    glViewport(0, 0, width, height);
}

// 鼠标按下 / 抬起事件
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS) {
            g_mousePressed = true;

            // 添加一个点
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            float nx, ny;
            screenToNDC(x, y, nx, ny);

            g_points.push_back({nx, ny, g_color[0], g_color[1], g_color[2]});
        }
        else if (action == GLFW_RELEASE) {
            g_mousePressed = false;
        }
    }
}

// 鼠标移动事件（按下左键时绘图）
void cursor_position_callback(GLFWwindow* window, double x, double y)
{
    if (g_mousePressed)
    {
        float nx, ny;
        screenToNDC(x, y, nx, ny);

        g_points.push_back({nx, ny, g_color[0], g_color[1], g_color[2]});
    }
}

// 处理按键输入
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        g_color[0] = g_color[1] = g_color[2] = 0.0f;    // 黑色

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) { // 红色
        g_color[0] = 1.0f; g_color[1] = 0.0f; g_color[2] = 0.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) { // 绿色
        g_color[0] = 0.0f; g_color[1] = 1.0f; g_color[2] = 0.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) { // 蓝色
        g_color[0] = 0.0f; g_color[1] = 0.0f; g_color[2] = 1.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) { // 清空
        g_points.clear();
    }
}

// Shader 工具函数
void checkCompile(GLuint shader, const std::string& type)
{
    GLint success;
    char info[512];

    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 512, NULL, info);
            std::cout << "Shader Compile Error (" << type << "):\n" << info << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 512, NULL, info);
            std::cout << "Program Linking Error\n" << info << std::endl;
        }
    }
}

int main()
{
    // 初始化 GLFW
    if (!glfwInit()) {
        std::cout << "GLFW 初始化失败！" << std::endl;
        return -1;
    }

    // 配置 OpenGL 版本为 3.3 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(g_width, g_height, "OpenGL 白板软件", NULL, NULL);
    if (!window) {
        std::cout << "窗口创建失败！" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 加载 GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "GLAD 初始化失败！" << std::endl;
        return -1;
    }

    // 注册回调
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // --- Shader ---
    const char* vShaderSrc = R"(
        #version 330 core
        layout(location = 0) in vec2 aPos;
        layout(location = 1) in vec3 aColor;
        out vec3 vColor;

        void main(){
            gl_Position = vec4(aPos, 0.0, 1.0);
            vColor = aColor;
        }
    )";

    const char* fShaderSrc = R"(
        #version 330 core
        out vec4 FragColor;
        in vec3 vColor;

        void main(){
            FragColor = vec4(vColor, 1.0);
        }
    )";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vShaderSrc, NULL);
    glCompileShader(vs);
    checkCompile(vs, "VERTEX");

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fShaderSrc, NULL);
    glCompileShader(fs);
    checkCompile(fs, "FRAGMENT");

    GLuint shader = glCreateProgram();
    glAttachShader(shader, vs);
    glAttachShader(shader, fs);
    glLinkProgram(shader);
    checkCompile(shader, "PROGRAM");

    glDeleteShader(vs);
    glDeleteShader(fs);

    // 顶点数组和缓冲
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glPointSize(5.0f); // 画笔大小

    // 主循环
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // 背景设为白色
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (!g_points.empty())
        {
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);

            // 更新点数据
            glBufferData(GL_ARRAY_BUFFER, g_points.size() * sizeof(Vertex),
                         g_points.data(), GL_DYNAMIC_DRAW);

            // 顶点格式：位置 (vec2) + 颜色 (vec3)
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                                  sizeof(Vertex), (void*)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                                  sizeof(Vertex), (void*)(2 * sizeof(float)));
            glEnableVertexAttribArray(1);

            glUseProgram(shader);
            glDrawArrays(GL_POINTS, 0, g_points.size());
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 清理
    glfwTerminate();
    return 0;
}
