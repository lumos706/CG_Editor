#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H
#include "glitter.hpp"
#include <vector>
#include <glm/glm.hpp>
#include "Mesh.h"
#include <iostream>
#include <optional>

unsigned int hash_tyz(int x, int y);
float Noise(float x, float y);
float FBM(float x, float y, int layers);  // 分形布朗运动噪声函数
glm::vec3 calcNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
class HeightMap {
public:
    static void GenerateTerrainFromHeightMap(int& width,int& height,const std::string& heightMapPath,
        float heightScale, float gridSpacing,
        std::vector<Vertex>& vertices,
        std::vector<unsigned int>& indices,
        int fbmLayers = 4) {

        int channels;
        unsigned char* data = stbi_load(heightMapPath.c_str(), &width, &height, &channels, 1); // Load as grayscale

        if (!data) {
            std::cerr << "Failed to load height map: " << heightMapPath << std::endl;
            return;
        }
        // 将宽度和高度存储在成员变量中
        std::vector<glm::vec3> normals(width * height, glm::vec3(0.0f));
        // Generate vertices
        for (int z = 0; z < height; ++z) {
            for (int x = 0; x < width; ++x) {
                float h = data[z * width + x] / 255.0f * heightScale; // Use original height map value

                // 应用分形布朗运动噪声以优化地形
                float fbmNoise = FBM(x * 0.1f, z * 0.1f, fbmLayers);
                fbmNoise = fbmNoise * 0.5f - 1.0f;
                h += fbmNoise * heightScale;


                Vertex vertex;
                vertex.Position = glm::vec3(
                    x * gridSpacing - (width * gridSpacing / 2.0f),
                    h - (heightScale / 2.0f),
                    z * gridSpacing - (height * gridSpacing / 2.0f)
                );
                vertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f); // Temporary normal
                vertex.TexCoords = glm::vec2((float)x / (width - 1), (float)z / (height - 1));
                vertices.push_back(vertex);
            }
        }

        // 生成索引
        for (int z = 0; z < height - 1; ++z) {
            for (int x = 0; x < width - 1; ++x) {
                unsigned int topLeft = z * width + x;
                unsigned int topRight = topLeft + 1;
                unsigned int bottomLeft = topLeft + width;
                unsigned int bottomRight = bottomLeft + 1;

                // 生成两个三角形
                indices.push_back(topLeft);
                indices.push_back(bottomLeft);
                indices.push_back(topRight);

                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                indices.push_back(bottomRight);

                // 计算法线并累加
                glm::vec3 normal = glm::normalize(calcNormal(
                    vertices[topLeft].Position,
                    vertices[bottomLeft].Position,
                    vertices[topRight].Position
                ));
                normals[topLeft] += normal;
                normals[bottomLeft] += normal;
                normals[topRight] += normal;

                normal = glm::normalize(calcNormal(
                    vertices[topRight].Position,
                    vertices[bottomLeft].Position,
                    vertices[bottomRight].Position
                ));
                normals[topRight] += normal;
                normals[bottomLeft] += normal;
                normals[bottomRight] += normal;
            }
        }

        // 归一化法线
        for (int i = 0; i < normals.size(); ++i) {
            normals[i] = glm::normalize(normals[i]);
        }

        // 将计算出的法线分配给顶点
        for (int i = 0; i < vertices.size(); ++i) {
            vertices[i].Normal = normals[i];
        }

        stbi_image_free(data);
    }

};
unsigned int hash_tyz(int x, int y) {
    unsigned int n = x + y * 57;  // 加上常量以改变结果
    n = (n << 13) ^ n;
    return (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff;
}

float Noise(float x, float y) {
    // 获取坐标的整数部分和小数部分
    int ix = static_cast<int>(x);
    int iy = static_cast<int>(y);
    float fx = x - ix;
    float fy = y - iy;

    // 哈希函数生成伪随机值
    unsigned int n00 = hash_tyz(ix, iy);
    unsigned int n01 = hash_tyz(ix, iy + 1);
    unsigned int n10 = hash_tyz(ix + 1, iy);
    unsigned int n11 = hash_tyz(ix + 1, iy + 1);

    // 将哈希值映射到[0, 1]范围
    float r00 = static_cast<float>(n00) / 0x7fffffff;
    float r01 = static_cast<float>(n01) / 0x7fffffff;
    float r10 = static_cast<float>(n10) / 0x7fffffff;
    float r11 = static_cast<float>(n11) / 0x7fffffff;

    // 线性插值
    float rx0 = r00 + fx * (r10 - r00);
    float rx1 = r01 + fx * (r11 - r01);
    float r = rx0 + fy * (rx1 - rx0);

    return r;  // 返回插值后的噪声值
}
// FBM生成函数实现
float FBM(float x, float y, int layers) {
    float value = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;

    // 叠加多个噪声层
    for (int i = 0; i < layers; ++i) {
        value += amplitude * Noise(x * frequency, y * frequency);  // 使用基础噪声生成函数
        frequency *= 2.0f;  // 增加频率
        amplitude *= 0.5f;  // 减少振幅
    }

    return value;
}

// 计算法线
glm::vec3 calcNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
    glm::vec3 edge1 = p2 - p1;
    glm::vec3 edge2 = p3 - p1;
    glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));  // 交叉乘积计算法线
    return normal;
}


#endif
