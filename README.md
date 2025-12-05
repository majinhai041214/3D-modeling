# 3D Modeling & Animation System  
### 基于 C/C++ 与 OpenGL 的图形学课程项目

本项目为《计算机图形学》课程的大作业，由两名成员共同开发，旨在构建一个具备**基本建模、渲染、视图控制、运动动画与物理模拟**的轻量级 3D 图形系统。  
项目采用 **C/C++ + OpenGL** 实现核心渲染流程，通过逐步迭代的方式构建完整的 3D 可视化功能。

---
## 项目初始化
  - g++ --version:
  -  g++ (x86_64-win32-seh-rev0, Built by MinGW-W64 project) 8.1.0

  - 项目运行命令：
    ```bash
    g++ -std=c++17 src/main.cpp src/glad.c \
    -Iinclude -Llib \
    -lglfw3 -lopengl32 -lgdi32 \
    -o exe/app.exe

## 🚀 项目目标（Project Goals）

- 实现开放的基础 3D 图形绘制系统  
- 支持模型样式设计：参数、材质、纹理、光照  
- 提供灵活的 3D 视角控制与用户交互  
- 实现图形基础运动（平移、缩放）  
- 实现高级运动功能：路径动画 + 物理引擎模拟  
- 形成可运行的 Demo 系统与清晰的代码结构  

---

## 🧱 功能开发路线（Feature Roadmap）

### **Step 0：环境配置与test项目运行**

### **Step 1：基础 3D 图形绘制**
- A：根据基础网格点/默认图形绘制立体图形 
- B：实现菜单栏功能

---

### **Step 2：图形样式设计**
- A：图形纹理加载 
- B：图形参数、光照模型

---

### **Step 3：灵活视图控制**
- A：鼠标/键盘控制视角旋转
- B：坐标轴、网格地面等辅助元素可视化

---

### **Step 4：基础运动功能**
- A：针对单个对象的平移、缩放
- B：用户交互：多对象情况选择物体的运动  

---

### **Step 5：高级运动功能**
- A：路径运动
- B：物理属性运动

---

## 🛠️ 技术栈（Tech Stack）

- **编程语言：C/C++**
- **图形 API：OpenGL**
- **依赖库：**
  - GLFW – 窗口与输入控制  
  - GLAD – OpenGL 函数加载器  
  - glm – 数学库  
  - stb_image – 纹理加载  
- **构建工具：**
  - Visual Studio / VSCode g++ 构建  

---

## 📁 项目结构（Project Structure）

```plaintext
ProjectRoot
│── src/                 # 源代码文件夹
│   ├── main.cpp         # 主程序入口
│   ├── renderer/        # 渲染模块（Shader、Mesh、Texture）
│   ├── camera/          # 摄像机模块
│   ├── objects/         # 图形对象及其属性
│   ├── animation/       # 动画系统模块
│   ├── physics/         # 物理引擎模块
│── assets/              # 模型与纹理素材
│── include/             # 头文件
│── external/            # 第三方库
│── README.md            # 项目说明文件
