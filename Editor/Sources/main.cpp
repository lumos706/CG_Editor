// Local Headers
#include "glitter.hpp"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "HeightMap.h"
// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <windows.h>
#include <commdlg.h>
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void setupLightingAndMaterial(Shader& Shader);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
bool castRay(GLFWwindow* window, glm::vec3& outHitPoint, Model*& selectedModel);
void setCube(float* vertices);
unsigned int loadTexture(std::vector< std::string> faces);
unsigned int loadTexture_heightmap(const char* path);
glm::vec3 RayTerrainIntersection(GLFWwindow* window, const std::vector<Vertex>& vertices, int width, int height, float gridSpacing);
std::string openFileDialog();
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//models
std::vector<Model*> models;  // 存储所有模型的列表
Model init(camera);
extern Model* selectedModel = &init;

//lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 pointLightPositions[] = {
    glm::vec3(0.1f,  0.2f,  0.5f),
    glm::vec3(2.3f, -1.3f, -4.0f),
    glm::vec3(-4.0f,  2.0f, -12.0f),
    glm::vec3(0.0f,  0.0f, -3.0f)
};

//地形图
float heightScale = 2.5f;
float gridSpacing = 0.25f;
std::vector<Vertex> terrainVertices;
std::vector<unsigned int> terrainIndices;
int Width, Height;

int main(int argc, char* argv[]) {
    //std::cout << "init:"<<selectedModel << endl;
    //std::cout << "Current Working Directory: " << std::filesystem::current_path() << std::endl;
    // Load GLFW and Create a Window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    auto mWindow = glfwCreateWindow(mWidth, mHeight, "OpenGL", nullptr, nullptr);

    // Check for Valid Context
    if (mWindow == nullptr) {
        fprintf(stderr, "Failed to Create OpenGL Context");
        return EXIT_FAILURE;
    }

    // Create Context and Load OpenGL Functions
    glfwMakeContextCurrent(mWindow);
    glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);
    glfwSetCursorPosCallback(mWindow, mouse_callback);
    glfwSetScrollCallback(mWindow, scroll_callback);

    // 鼠标
    glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);


    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader ourShader("../Editor/Shaders/1.model_loading.vs", "../Editor/Shaders/1.model_loading.fs");
    Shader terrainShader("../Editor/Shaders/heightmap.vs", "../Editor/Shaders/heightmap.fs");
    Shader lightCubeShader("../Editor/Shaders/light_cube.vs", "../Editor/Shaders/light_cube.fs");
    Shader skyBoxShader("../Editor/Shaders/skybox.vs", "../Editor/Shaders/skybox.fs");
    // load models，如果需要导入新模型，在此增加
    // -----------
    Model tentmodel(camera,"../Editor/Models/lowpolytent.obj");
    //cout << "tentmodel:" << &tentmodel << endl;
    models.push_back(&tentmodel);

    Model treemodel(camera, "../Editor/Models/oakTree.obj");
    models.push_back(&treemodel);

    Model treemodel_2(camera, "../Editor/Models/pineTree.obj");
    models.push_back(&treemodel_2);
    /*string path1 = "C:/Users/10559/Desktop/Study/Three/CG/CV_Editor/Editor/Models/lowpolytent.obj";
    string path2 = "C:/Users/10559/Desktop/Study/Three/CG/CV_Editor/Editor/Models/Tree.obj";
    createmodel(path1);
    createmodel(path2);
    createmodel(path2);*/

    unsigned int VBO, cubeVAO;
    
    float vertices[288]{
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    setCube(vertices);
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(cubeVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    GLfloat cude_index[] = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
    };

    unsigned int skyVAO, skyVBO;
    glGenVertexArrays(1, &skyVAO);
    glGenBuffers(1, &skyVBO);
    glBindVertexArray(skyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cude_index), cude_index, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    // we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    //skybox
    // 
    unsigned int skyboxTexture;
    std::vector< std::string> faces;
    faces.push_back("../Editor/Objects/left.jpg");
    faces.push_back("../Editor/Objects/right.jpg");

    faces.push_back("../Editor/Objects/top.jpg");
    faces.push_back("../Editor/Objects/bottom.jpg");
    faces.push_back("../Editor/Objects/front.jpg");
    faces.push_back("../Editor/Objects/back.jpg");
    skyboxTexture = loadTexture(faces);

    skyBoxShader.use();
    skyBoxShader.setInt("sky", 0);
    
    HeightMap::GenerateTerrainFromHeightMap(Width, Height, "../Editor/Objects/Heightmap.png", heightScale, gridSpacing, terrainVertices, terrainIndices);
    //cout << Width << " " << Height << endl;
    //cout << terrainVertices.size() << endl;
    // Create a Mesh object for the terrain
    Mesh terrain(terrainVertices, terrainIndices, std::vector<Texture>());
    unsigned int grassTexture = loadTexture_heightmap("../Editor/Models/dark_grass.jpg");
    // 绑定纹理到纹理单元 0
    
    

    // Rendering Loop

    while (glfwWindowShouldClose(mWindow) == false) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        //cout << "before_selectedmodel:" << selectedModel << endl;
        processInput(mWindow);


        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //// material properties
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(10.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f));	// it's a bit too big for our scene, so scale it down
        terrainShader.use();
        terrainShader.setInt("texture_diffuse1", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, grassTexture);
        // 设置 `texture_diffuse1` uniform 指定为纹理单元 1

        terrainShader.setMat4("model", model);
        terrainShader.setMat4("view", view);
        terrainShader.setMat4("projection", projection);
        terrainShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        terrainShader.setVec3("dirLight.ambient", 0.3f, 0.3f, 0.3f);
        terrainShader.setVec3("dirLight.diffuse", 0.8f, 0.8f, 0.8f);
        terrainShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
        // point light 1
        terrainShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        terrainShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        terrainShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        terrainShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        terrainShader.setFloat("pointLights[0].constant", 1.0f);
        terrainShader.setFloat("pointLights[0].linear", 0.09f);
        terrainShader.setFloat("pointLights[0].quadratic", 0.032f);
        // point light 2
        terrainShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        terrainShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        terrainShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        terrainShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        terrainShader.setFloat("pointLights[1].constant", 1.0f);
        terrainShader.setFloat("pointLights[1].linear", 0.09f);
        terrainShader.setFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        terrainShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        terrainShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        terrainShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        terrainShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        terrainShader.setFloat("pointLights[2].constant", 1.0f);
        terrainShader.setFloat("pointLights[2].linear", 0.09f);
        terrainShader.setFloat("pointLights[2].quadratic", 0.032f);
        // point light 4
        terrainShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        terrainShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        terrainShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        terrainShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        terrainShader.setFloat("pointLights[3].constant", 1.0f);
        terrainShader.setFloat("pointLights[3].linear", 0.09f);
        terrainShader.setFloat("pointLights[3].quadratic", 0.032f);
        // spotLight
        terrainShader.setVec3("spotLight.position", camera.Position);
        terrainShader.setVec3("spotLight.direction", camera.Front);
        terrainShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        terrainShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        terrainShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        terrainShader.setFloat("spotLight.constant", 1.0f);
        terrainShader.setFloat("spotLight.linear", 0.09f);
        terrainShader.setFloat("spotLight.quadratic", 0.032f);
        terrainShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        terrainShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
        terrainShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        // material properties
        terrainShader.setFloat("material.shininess", 64.0f);
        terrain.Draw(terrainShader);
        // don't forget to enable shader before setting uniforms
        setupLightingAndMaterial(ourShader);
        
        for (auto& item : models) {
            glm::mat4 modelmatrix = (*item).GetModelMatrix();
            //cout << models.size();
            ourShader.setMat4("model", modelmatrix);
            (*item).Draw(ourShader);
        }

        //cout << "ourmodel.position:" << tentmodel.position << endl;

        glDepthFunc(GL_LEQUAL);
        skyBoxShader.use();
        // 传视图,投影矩阵
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyBoxShader.setMat4("view", view);
        skyBoxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

        lightCubeShader.use();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);

        glBindVertexArray(cubeVAO);
        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.05f)); // Make it a smaller cube
            lightCubeShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        // Flip Buffers and Draw
        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }   glfwTerminate();
    return EXIT_SUCCESS;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        for (auto& model : models) {
            (*model).Deselect();
        }
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        glm::vec3 hitPoint = RayTerrainIntersection(window, terrainVertices, Width, Height, gridSpacing);

        //cout << hitPoint << endl;
        std::string selectedFile = openFileDialog();
        cout << selectedFile << endl;
        Model* insertmodel = new Model(camera, selectedFile, false, hitPoint);
        float ModelHeight = (*insertmodel).GetHeight();
        (*insertmodel).position += ModelHeight / 2;
        models.push_back(insertmodel); // 将指针存入 models 容器中

        cout << ((*insertmodel).position) << endl;
        //cout << models.size();
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        // 执行射线投射检测
        glm::vec3 hitPoint;
        if (castRay(window, hitPoint, selectedModel)) {
            std::cout << "Select()" << endl;
            // 如果检测到点击模型，更新选中状态
            (*selectedModel).Select();
            cout << "select_modle" << (&selectedModel) << endl;
            for (auto& model : models) {
                if (selectedModel != model) {
                    cout << "not_select_modle" << (&model) << endl;
                    (*model).Deselect();
                }
            }
        }
        else {
            std::cout << "Deselect()" << endl;
            (*selectedModel).Deselect();
            for (auto& model : models) {
                (*model).Deselect(); 
            }
        }
    }
    if ((*selectedModel).isSelected) {
        //cout << "model" << endl;
        // 只有选中模型时，才移动模型
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            cout << "hello";
            //(*selectedModel).position += glm::vec3(0.0f, 0.0f, 10.0f);
            //cout << "process_selectmodel:" << selectedModel << endl;
            (*selectedModel).Move(FORWARD, deltaTime);
        }
            
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            (*selectedModel).Move(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            (*selectedModel).Move(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            (*selectedModel).Move(RIGHT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            (*selectedModel).Move(UP, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            (*selectedModel).Move(DOWN, deltaTime);
    }
    else {
    // 否则继续控制相机
        //cout << "camera" << endl;
        
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime); // 向上
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime); // 向下
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

//检测模型是否被选中
// 射线投射的功能：从相机的位置和方向发射一条射线，并检测与模型的交点
bool castRay(GLFWwindow* window, glm::vec3& outHitPoint, Model*& selectedModel)
{
    // 获取相机视角和投影矩阵
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    // 获取鼠标位置
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    xpos = (xpos / SCR_WIDTH) * 2.0 - 1.0;
    ypos = -(ypos / SCR_HEIGHT) * 2.0 + 1.0;

    glm::vec4 rayClipSpace = glm::vec4(xpos, ypos, -1.0f, 1.0f);
    glm::vec4 rayEyeSpace = glm::inverse(projection) * rayClipSpace;
    rayEyeSpace = glm::vec4(rayEyeSpace.x, rayEyeSpace.y, -1.0f, 0.0f);

    glm::vec3 rayWorldSpace = glm::vec3(glm::inverse(view) * rayEyeSpace);
    glm::vec3 rayOrigin = camera.Position;
    //std::cout << "rayorigin:" << rayOrigin << endl;
    glm::vec3 rayDirection = glm::normalize(rayWorldSpace);
    //std::cout << "raydirection:" << rayDirection << endl;
    // 检测与模型的交点（可以使用AABB或其他方式）
    /*for (auto& model : models) {
        cout <<"model:" << model << endl;
    }*/
    for (auto& model : models) {
        if ((*model).Intersects(rayOrigin, rayDirection)) {
            cout << "some model is selected" << endl;
            selectedModel = model;  // 更新选中的模型
            cout << "选中后的selectmodel:"<<( & selectedModel) << endl;
            outHitPoint = (*model).GetIntersectionPoint(rayOrigin, rayDirection);
            return true;
        }
    }


    return false;
}
// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    static bool isDragging = false;
    int leftButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    int rightButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);  // 检查右键是否按下
    
    if (leftButtonState == GLFW_PRESS) {
        if (!isDragging) {
            isDragging = true;
            lastX = xpos;
            lastY = ypos;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;

        lastX = xpos;
        lastY = ypos;

        float sensitivity = 1.0f; // 调整灵敏度
        camera.ProcessMouseMovement(xoffset * sensitivity, yoffset * sensitivity);
    }
    if (rightButtonState == GLFW_PRESS) {  // 如果右键按下
        if (!isDragging) {
            isDragging = true;
            lastX = xpos;
            lastY = ypos;
        }

        // 计算鼠标移动的偏移量
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;

        lastX = xpos;
        lastY = ypos;

        // 设置旋转灵敏度
        float sensitivity = 1.0f;

        // 根据鼠标的移动量旋转模型
        if (selectedModel) {
            // 根据鼠标的水平和垂直偏移量来旋转模型
            (*selectedModel).Rotate(xoffset * sensitivity, glm::vec3(0.0f, 1.0f, 0.0f));  // 绕Y轴旋转
            //(*selectedModel).Rotate(yoffset * sensitivity, glm::vec3(-1.0f, 0.0f, 0.0f));  // 绕X轴旋转
        }
    }
    if (rightButtonState == GLFW_RELEASE&&leftButtonState == GLFW_RELEASE) {  // 如果右键释放
        isDragging = false;
    }
}




// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if ((*selectedModel).isSelected) {
        // 只有选中的模型才进行缩放
        float scaleFactor = 1.0f + static_cast<float>(yoffset) * 0.1f; // 缩放系数
        (*selectedModel).Scale(scaleFactor);
    }
    else {
        camera.ProcessMouseScroll(static_cast<float>(yoffset));
    }
}
void setCube(float* vertices)
{

}
// 加载纹理函数
unsigned int loadTexture_heightmap(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        //std::cout << nrComponents;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // 设置纹理参数
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else {
        std::cout << "Failed to load texture: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
unsigned int loadTexture(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

glm::vec3 RayTerrainIntersection(
    GLFWwindow* window,
    const std::vector<Vertex>& vertices,
    int width,
    int height,
    float gridSpacing)
{
    // 获取相机视角和投影矩阵
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
        (float)SCR_WIDTH / (float)SCR_HEIGHT,
        0.1f, 100.0f);

    // 获取鼠标位置
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    xpos = (xpos / SCR_WIDTH) * 2.0 - 1.0;
    ypos = -(ypos / SCR_HEIGHT) * 2.0 + 1.0;

    // 将鼠标位置从裁剪坐标系转换到世界坐标系
    glm::vec4 rayClipSpace = glm::vec4(xpos, ypos, -1.0f, 1.0f);
    glm::vec4 rayEyeSpace = glm::inverse(projection) * rayClipSpace;
    rayEyeSpace = glm::vec4(rayEyeSpace.x, rayEyeSpace.y, -1.0f, 0.0f);

    glm::vec3 rayWorldSpace = glm::vec3(glm::inverse(view) * rayEyeSpace);
    glm::vec3 rayOrigin = camera.Position;
    glm::vec3 rayDirection = glm::normalize(rayWorldSpace);
    // 设置初始步长和最大迭代次数
    float tMin = 0.0f, tMax = 1000.0f;
    const int maxIterations = 100;

    for (int i = 0; i < maxIterations; ++i) {
        float tMid = (tMin + tMax) * 0.5f;
        glm::vec3 currentPoint = rayOrigin + tMid * rayDirection;

        // 获取网格坐标
        int x = static_cast<int>((currentPoint.x + (width * gridSpacing / 2.0f)) / gridSpacing);
        int z = static_cast<int>((currentPoint.z + (height * gridSpacing / 2.0f)) / gridSpacing);

        // 判断是否越界
        if (x < 0 || x >= width - 1 || z < 0 || z >= height - 1) {
            tMax = tMid; // 超出范围，向较小t方向收缩
            continue;
        }

        // 获取周围的高度点
        float h00 = vertices[z * width + x].Position.y;
        float h10 = vertices[z * width + (x + 1)].Position.y;
        float h01 = vertices[(z + 1) * width + x].Position.y;
        float h11 = vertices[(z + 1) * width + (x + 1)].Position.y;

        // 计算双线性插值高度
        float localX = (currentPoint.x + (width * gridSpacing / 2.0f)) / gridSpacing - x;
        float localZ = (currentPoint.z + (height * gridSpacing / 2.0f)) / gridSpacing - z;
        float interpolatedHeight = h00 * (1 - localX) * (1 - localZ)
            + h10 * localX * (1 - localZ)
            + h01 * (1 - localX) * localZ
            + h11 * localX * localZ;

        // 检查当前点是否与地形相交
        if (currentPoint.y < interpolatedHeight) {
            tMax = tMid; // 当前点低于地形，缩小范围
        }
        else {
            tMin = tMid; // 当前点高于地形，增加范围
        }

        // 收敛条件：t范围足够小
        if (glm::abs(tMax - tMin) < 0.01f) {
            cout << "find!" << currentPoint << endl;
            return currentPoint;
        }
    }

    // 如果迭代结束后未找到交点，返回空值
    cout << "don't find!" << endl;
    return glm::vec3(0.0f, 0.0f, 0.0f);
}

std::string openFileDialog() {
    OPENFILENAME ofn;       // common dialog box structure
    char szFile[260];       // buffer for file name

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "OBJ Files\0*.OBJ\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = "Select a model file";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the Save As dialog box
    if (GetOpenFileName(&ofn) == TRUE) {
        std::string filePath = std::string(ofn.lpstrFile);
        std::replace(filePath.begin(), filePath.end(), '\\', '/');
        return filePath;
    }

    return "";
}

void setupLightingAndMaterial(Shader& Shader) {
    Shader.use();

    // directional light
    Shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    Shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    Shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    Shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
    // point light 1
    Shader.setVec3("pointLights[0].position", pointLightPositions[0]);
    Shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    Shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
    Shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    Shader.setFloat("pointLights[0].constant", 1.0f);
    Shader.setFloat("pointLights[0].linear", 0.09f);
    Shader.setFloat("pointLights[0].quadratic", 0.032f);
    // point light 2
    Shader.setVec3("pointLights[1].position", pointLightPositions[1]);
    Shader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
    Shader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
    Shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
    Shader.setFloat("pointLights[1].constant", 1.0f);
    Shader.setFloat("pointLights[1].linear", 0.09f);
    Shader.setFloat("pointLights[1].quadratic", 0.032f);
    // point light 3
    Shader.setVec3("pointLights[2].position", pointLightPositions[2]);
    Shader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
    Shader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
    Shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
    Shader.setFloat("pointLights[2].constant", 1.0f);
    Shader.setFloat("pointLights[2].linear", 0.09f);
    Shader.setFloat("pointLights[2].quadratic", 0.032f);
    // point light 4
    Shader.setVec3("pointLights[3].position", pointLightPositions[3]);
    Shader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
    Shader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
    Shader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
    Shader.setFloat("pointLights[3].constant", 1.0f);
    Shader.setFloat("pointLights[3].linear", 0.09f);
    Shader.setFloat("pointLights[3].quadratic", 0.032f);
    // spotLight
    Shader.setVec3("spotLight.position", camera.Position);
    Shader.setVec3("spotLight.direction", camera.Front);
    Shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    Shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    Shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    Shader.setFloat("spotLight.constant", 1.0f);
    Shader.setFloat("spotLight.linear", 0.09f);
    Shader.setFloat("spotLight.quadratic", 0.032f);
    Shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    Shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));


    Shader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
    // material properties
    Shader.setFloat("material.shininess", 64.0f);
    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    Shader.setMat4("projection", projection);
    Shader.setMat4("view", view);
}
