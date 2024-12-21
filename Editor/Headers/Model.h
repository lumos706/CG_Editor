#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include "Camera.h"
using namespace std;

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);
std::ostream& operator<<(std::ostream& os, const glm::vec3& vec) {
    os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return os;
}
class Model
{
public:
    // model data 
    //���м��ع������� vector 
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    //ģ������  
    vector<Mesh> meshes;
    string directory;

    bool gammaCorrection;

    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    bool isSelected = false; // �Ƿ�ѡ��

    // ��������Χ�еļ�������
    glm::vec3 boundingBoxMin = glm::vec3(FLT_MAX);
    glm::vec3 boundingBoxMax = glm::vec3(-FLT_MAX);
    //std::shared_ptr<Model> modelPtr;
    Model& operator=(const Model& model) {
    }
    // ���캯��������һ��·����������load model�������ļ�
    Model(const Camera& cam, string const& path, bool gamma = false, glm::vec3 pos = glm::vec3(0.0f,0.0f,0.0f)) : camera(cam), gammaCorrection(gamma), position(pos)
    {
        loadModel(path);
        CalculateBoundingBox(); // �����Χ��
        updateVectors();
    }

    Model(const Camera& cam) : camera(cam),gammaCorrection(false)
    {
        CalculateBoundingBox();
        updateVectors();
    }

    void updateVectors() {
        Front = camera.Front;  // ʵʱ��ȡ Front
        Right = camera.Right; // ʵʱ��ȡ Right
        Up = camera.Up;       // ʵʱ��ȡ Up

        // �����߼�
    }
    // �����������磬���ø��Ե�draw���������ģ�ͻ���
    void Draw(Shader& shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

    // �ƶ�ģ��
    void Move(Camera_Movement direction, float deltaTime)
    {
        cout << this << endl;
        CalculateBoundingBox();
        updateVectors();

        //cout << "front:" << Front << endl;
        float velocity = 2.5f * deltaTime;
        if (direction == FORWARD) {
            this->position += Front * velocity;
            //cout << "forward" << position << endl;
        }
            
        if (direction == BACKWARD)
            this->position -= Front * velocity;
        if (direction == LEFT)
            this->position -= Right * velocity;
        if (direction == RIGHT)
            this->position += Right * velocity;
        if (direction == UP)
            this->position += Up * velocity; // �����ƶ�
        if (direction == DOWN)
            this->position -= Up * velocity; // �����ƶ�
        //cout << "position:" << this->position << endl;
    }

    // ��תģ��
    void Rotate(float angle, const glm::vec3& axis)
    {
        CalculateBoundingBox();
        updateVectors();
        rotation += glm::vec3(angle * axis.x, angle * axis.y, angle * axis.z);
    }

    // ����ģ��
    void Scale(float factor)
    {
        CalculateBoundingBox();
        updateVectors();
        scale *= factor;
    }

    // �����Χ��
    void CalculateBoundingBox() {
        boundingBoxMin = glm::vec3(FLT_MAX);
        boundingBoxMax = glm::vec3(-FLT_MAX);

        glm::mat4 modelMatrix = GetModelMatrix();

        for (const auto& mesh : meshes) {
            for (const auto& vertex : mesh.vertices) {
                // ת��������ռ�
                glm::vec4 worldPos = modelMatrix * glm::vec4(vertex.Position, 1.0f);
                boundingBoxMin = glm::min(boundingBoxMin, glm::vec3(worldPos));
                boundingBoxMax = glm::max(boundingBoxMax, glm::vec3(worldPos));
            }
        }
    }

    float GetHeight() {
        CalculateBoundingBox();
        return boundingBoxMax.y - boundingBoxMin.y;
    }

    // �������Χ���ཻ���
    bool Intersects(const glm::vec3& rayOrigin, const glm::vec3& rayDir) {
        CalculateBoundingBox(); // �����Χ��
        updateVectors();
        cout << "boundingboxMax" << boundingBoxMax.b << endl;
        cout<<"boundingboxmin"<< boundingBoxMin.b << endl;
        glm::vec3 invDir = 1.0f / rayDir;
        glm::vec3 tMin = (boundingBoxMin - rayOrigin) * invDir;
        glm::vec3 tMax = (boundingBoxMax - rayOrigin) * invDir;

        glm::vec3 t1 = glm::min(tMin, tMax);
        glm::vec3 t2 = glm::max(tMin, tMax);

        float tNear = glm::max(glm::max(t1.x, t1.y), t1.z);
        float tFar = glm::min(glm::min(t2.x, t2.y), t2.z);

        return tNear <= tFar && tFar > 0.0f;
    }

    glm::vec3 GetIntersectionPoint(const glm::vec3& rayOrigin, const glm::vec3& rayDir) {
        CalculateBoundingBox(); // �����Χ��
        updateVectors();
        glm::vec3 invDir = 1.0f / rayDir;
        glm::vec3 tMin = (boundingBoxMin - rayOrigin) * invDir;
        glm::vec3 tMax = (boundingBoxMax - rayOrigin) * invDir;

        glm::vec3 t1 = glm::min(tMin, tMax);
        glm::vec3 t2 = glm::max(tMin, tMax);

        float tNear = glm::max(glm::max(t1.x, t1.y), t1.z);
        float tFar = glm::min(glm::min(t2.x, t2.y), t2.z);

        // ����������Χ���ཻ���򷵻ؽ���
        if (tNear <= tFar && tFar > 0.0f) {
            // �������ߵĽ���
            glm::vec3 intersectionPoint = rayOrigin + rayDir * tNear;  // ��ʹ�� tFar ��ΪԶ����
            return intersectionPoint;
        }

        return glm::vec3(FLT_MAX);  // ����һ����Ч���ʾû�н���
    }


    // ѡ��ģ��
    void Select() {
        isSelected = true;
    }

    // ȡ��ѡ��
    void Deselect() {
        isSelected = false;
    }


    // ��ȡģ�͵ı任����
    glm::mat4 GetModelMatrix() const
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // ��ת
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // ��ת
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // ��ת
        model = glm::scale(model, scale);  // ����
        return model;
    }


private:
    // loadModel ʹ��assimp����ģ��
    const Camera& camera;  // ����Camera������
    void loadModel(string const& path)
    {
        //cout << path << endl;
        //��Assimp�������ռ��� �ø�importer ����read file����
        Assimp::Importer importer;
        //�ڶ���������һ�����ڴ����ѡ�� 
        //aiProcess_Triangulate ���ģ�Ͳ���ȫ������������ɣ�������ͼԪ��״��Ϊ������
        //aiProcess_FlipUVs ��תy����������� 
        //aiProcess_GenSmoothNormals ���û�з���������ÿ�����㴴������
        //aiProcess_SplitLargeMeshes
        //aiProcess_OptimizeMeshes
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('/'));
        cout << directory << endl;
        // process ASSIMP's root node recursively
        //ÿ���ڵ���ܰ�������ӽڵ㣬���� �ȴ�������еĽڵ� �ٴ������е��ӽڵ� 
        //���ϵݹ�ṹ
        processNode(scene->mRootNode, scene);
    }

    //assimp�ṹ��ÿ���ڵ������һϵ�е��������� ÿ������ָ�򳡾�������ض�����
    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene)
    {
        // process each mesh located at the current node
        //�������е�����
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
           //�Ƚ���һ���������� 
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            //���������е�mMeshes �õ���Ӧ������  
            //���ص����� ���� process mesh  ����һ��mesh���󣬴���meshes�б���
            meshes.push_back(processMesh(mesh, scene));
        }
        //Ȼ�󿴿�����
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }

    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        // data to fill
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;

        // walk through each of the mesh's vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            //������λ�� ���� ��������
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // normals
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }
            // texture coordinates
            if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
                // tangent
                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;
                // bitangent
                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }
        //�������� ÿ������ ������ ���� ��ÿ�������һ��ͼԪ
        // //һ�����ж������ ��ôӦ�ð�ʲô˳������ĸ�����
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            //�浽indices ���vector
            // retrieve all indices of the face and store them in the indices vector
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        // process materials

        //�������
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        // Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN

        //loadMaterialTextures ��ȡ���� ����vector 
        // ������aiTextureType_ Ϊǰ׺
        // 1. diffuse maps
        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures);
    }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if (!skip)
            {   // if texture hasn't been loaded already, load it
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
        }
        return textures;
    }
};


unsigned int TextureFromFile(const char* path, const string& directory, bool gamma)
{
    string filename = string(path);
    filename = directory + '/' + filename;
    //cout << filename << endl;
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
#endif