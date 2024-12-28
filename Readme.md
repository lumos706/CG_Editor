# CG_Editor
第十小组

组长：马清扬

组员：邓笑晗、刘苇嘉、陶宇卓
## 目录
- [简介](#简介)
- [特点](#特点)
- [安装](#安装)
- [功能](#功能)
- [还未实现的功能](#还未实现的功能)

## 简介
CG_Editor ———— 小组开发的基于OpenGL的场景编辑器。

## 特点
- 集成多种 C++ 库。
- CMake 优化构建系统。
- GLSL 着色器编程。

## 安装
### 先决条件
- 只支持Windows系统
- CMake (>= v3.5.0, 推荐使用 >= v3.8.0)
- 兼容的 C++ 编译器
- std C++ 17或以上

### 步骤
1. 克隆仓库：
    ```bash
    git clone https://github.com/lumos706/CG_Editor.git
    cd CG_Editor
    ```

2. 创建构建目录并进入：
    ```bash
    mkdir build
    cd build
    ```

3. 使用 CMake 生成构建文件：
    ```bash
    cmake ..
    ```

4. 构建项目：
    ```bash
    make
    ```

## 功能
双击Build/Editor.sln
- 使用 w/s 键前后移动摄像头
- 使用 a/d 键左右移动摄像头
- 使用 q/e 键上下移动摄像头
- 通过鼠标左键拖动视角
- 通过鼠标 + g 键选择模型
- 选中后通过 w/s/a/d/g/e 键前后左右上下移动模型
- 按住鼠标右键旋转模型
- 通过鼠标滚轮放大和缩小模型
- 使用 f 键取消对所有模型的选中状态
- 通过 r 键选择放置模型与地形进行交互


## 还未实现的功能
- 用户友好的UI界面（可能要基于QT开发）
- 优化代码运行效率，提高帧率

## Stargazers over time
[![Stargazers over time](https://starchart.cc/lumos706/CG_Editor.svg?variant=adaptive)](https://starchart.cc/lumos706/CG_Editor)
