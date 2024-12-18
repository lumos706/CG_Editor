# CG_Editor

## 目录
- [简介](#简介)
- [功能](#功能)
- [安装](#安装)
- [使用](#使用)

## 简介
CG_Editor ———— 小组简单做的场景编辑器。

## 功能
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

## 使用
双击Build/Editor.sln
