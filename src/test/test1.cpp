#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// 窗口大小改变时的回调
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

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
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL + VS Code Demo", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 3. 初始化 GLAD（必须在有了 OpenGL 上下文之后）
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    // 设置初始视口
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 4. 渲染循环
    while (!glfwWindowShouldClose(window)) {
        // 处理输入：按 ESC 关闭窗口
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        // 设置清屏颜色（R,G,B,A）
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 交换前后缓冲
        glfwSwapBuffers(window);
        // 处理事件
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
