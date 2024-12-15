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
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
bool castRay(GLFWwindow* window, glm::vec3& outHitPoint, Model*& selectedModel);
void setCube(float* vertices);
unsigned int loadTexture(std::vector< std::string> faces);
unsigned int loadTexture_heightmap(const char* path);
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

std::vector<Model*> models;  // 存储所有模型的列表
Model init(camera);
extern Model* selectedModel = &init;

//lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
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
    Shader ourShader2("../Editor/Shaders/1.model_loading.vs", "../Editor/Shaders/1.model_loading.fs");
    Shader ourShader3("../Editor/Shaders/1.model_loading.vs", "../Editor/Shaders/1.model_loading.fs");
    Shader terrainShader("../Editor/Shaders/heightmap.vs", "../Editor/Shaders/heightmap.fs");
    Shader lightCubeShader("../Editor/Shaders/light_cube.vs", "../Editor/Shaders/light_cube.fs");
    Shader skyBoxShader("../Editor/Shaders/skybox.vs", "../Editor/Shaders/skybox.fs");
    // load models，如果需要导入新模型，在此增加
    // -----------
    Model tentmodel(camera,"../Editor/Models/lowpolytent.obj");
    //cout << "tentmodel:" << &tentmodel << endl;
    models.push_back(&tentmodel);

    Model treemodel(camera, "../Editor/Models/Tree.obj");
    models.push_back(&treemodel);

    Model treemodel_2(camera, "../Editor/Models/Tree.obj");
    models.push_back(&treemodel_2);



    unsigned int VBO, cubeVAO;
    glm::vec3 pointLightPositions[] = {
    glm::vec3(0.1f,  0.2f,  0.5f),
    glm::vec3(2.3f, -1.3f, -4.0f),
    glm::vec3(-4.0f,  2.0f, -12.0f),
    glm::vec3(0.0f,  0.0f, -3.0f)
    }; 
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
    std::vector<Vertex> terrainVertices;
    std::vector<unsigned int> terrainIndices;
    HeightMap::GenerateTerrainFromHeightMap("../Editor/Objects/Heightmap.png", 2.5f, 0.25f, terrainVertices, terrainIndices);
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
        ourShader.use();

        // directional light
        ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        ourShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        ourShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        ourShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        ourShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        ourShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[0].constant", 1.0f);
        ourShader.setFloat("pointLights[0].linear", 0.09f);
        ourShader.setFloat("pointLights[0].quadratic", 0.032f);
        // point light 2
        ourShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        ourShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        ourShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[1].constant", 1.0f);
        ourShader.setFloat("pointLights[1].linear", 0.09f);
        ourShader.setFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        ourShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        ourShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        ourShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[2].constant", 1.0f);
        ourShader.setFloat("pointLights[2].linear", 0.09f);
        ourShader.setFloat("pointLights[2].quadratic", 0.032f);
        // point light 4
        ourShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        ourShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        ourShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        ourShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("pointLights[3].constant", 1.0f);
        ourShader.setFloat("pointLights[3].linear", 0.09f);
        ourShader.setFloat("pointLights[3].quadratic", 0.032f);
        // spotLight
        ourShader.setVec3("spotLight.position", camera.Position);
        ourShader.setVec3("spotLight.direction", camera.Front);
        ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        ourShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("spotLight.constant", 1.0f);
        ourShader.setFloat("spotLight.linear", 0.09f);
        ourShader.setFloat("spotLight.quadratic", 0.032f);
        ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));


        ourShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        // material properties
        ourShader.setFloat("material.shininess", 64.0f);
        // view/projection transformations
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        //second shader
        ourShader2.use();

        // directional light
        ourShader2.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        ourShader2.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        ourShader2.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        ourShader2.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        ourShader2.setVec3("pointLights[0].position", pointLightPositions[0]);
        ourShader2.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        ourShader2.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        ourShader2.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        ourShader2.setFloat("pointLights[0].constant", 1.0f);
        ourShader2.setFloat("pointLights[0].linear", 0.09f);
        ourShader2.setFloat("pointLights[0].quadratic", 0.032f);
        // point light 2
        ourShader2.setVec3("pointLights[1].position", pointLightPositions[1]);
        ourShader2.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        ourShader2.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        ourShader2.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        ourShader2.setFloat("pointLights[1].constant", 1.0f);
        ourShader2.setFloat("pointLights[1].linear", 0.09f);
        ourShader2.setFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        ourShader2.setVec3("pointLights[2].position", pointLightPositions[2]);
        ourShader2.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        ourShader2.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        ourShader2.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        ourShader2.setFloat("pointLights[2].constant", 1.0f);
        ourShader2.setFloat("pointLights[2].linear", 0.09f);
        ourShader2.setFloat("pointLights[2].quadratic", 0.032f);
        // point light 4
        ourShader2.setVec3("pointLights[3].position", pointLightPositions[3]);
        ourShader2.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        ourShader2.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        ourShader2.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        ourShader2.setFloat("pointLights[3].constant", 1.0f);
        ourShader2.setFloat("pointLights[3].linear", 0.09f);
        ourShader2.setFloat("pointLights[3].quadratic", 0.032f);
        // spotLight
        ourShader2.setVec3("spotLight.position", camera.Position);
        ourShader2.setVec3("spotLight.direction", camera.Front);
        ourShader2.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        ourShader2.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        ourShader2.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        ourShader2.setFloat("spotLight.constant", 1.0f);
        ourShader2.setFloat("spotLight.linear", 0.09f);
        ourShader2.setFloat("spotLight.quadratic", 0.032f);
        ourShader2.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        ourShader2.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));


        ourShader2.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        // material properties
        ourShader2.setFloat("material.shininess", 64.0f);
        // view/projection transformations
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        ourShader2.setMat4("projection", projection);
        ourShader2.setMat4("view", view);

        //third shader
        ourShader3.use();

        // directional light
        ourShader3.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        ourShader3.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        ourShader3.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        ourShader3.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        ourShader3.setVec3("pointLights[0].position", pointLightPositions[0]);
        ourShader3.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        ourShader3.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        ourShader3.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        ourShader3.setFloat("pointLights[0].constant", 1.0f);
        ourShader3.setFloat("pointLights[0].linear", 0.09f);
        ourShader3.setFloat("pointLights[0].quadratic", 0.032f);
        // point light 2
        ourShader3.setVec3("pointLights[1].position", pointLightPositions[1]);
        ourShader3.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        ourShader3.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        ourShader3.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        ourShader3.setFloat("pointLights[1].constant", 1.0f);
        ourShader3.setFloat("pointLights[1].linear", 0.09f);
        ourShader3.setFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        ourShader3.setVec3("pointLights[2].position", pointLightPositions[2]);
        ourShader3.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        ourShader3.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        ourShader3.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        ourShader3.setFloat("pointLights[2].constant", 1.0f);
        ourShader3.setFloat("pointLights[2].linear", 0.09f);
        ourShader3.setFloat("pointLights[2].quadratic", 0.032f);
        // point light 4
        ourShader3.setVec3("pointLights[3].position", pointLightPositions[3]);
        ourShader3.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        ourShader3.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        ourShader3.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        ourShader3.setFloat("pointLights[3].constant", 1.0f);
        ourShader3.setFloat("pointLights[3].linear", 0.09f);
        ourShader3.setFloat("pointLights[3].quadratic", 0.032f);
        // spotLight
        ourShader3.setVec3("spotLight.position", camera.Position);
        ourShader3.setVec3("spotLight.direction", camera.Front);
        ourShader3.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        ourShader3.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        ourShader3.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        ourShader3.setFloat("spotLight.constant", 1.0f);
        ourShader3.setFloat("spotLight.linear", 0.09f);
        ourShader3.setFloat("spotLight.quadratic", 0.032f);
        ourShader3.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        ourShader3.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));


        ourShader3.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        // material properties
        ourShader3.setFloat("material.shininess", 64.0f);
        // view/projection transformations
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        ourShader3.setMat4("projection", projection);
        ourShader3.setMat4("view", view);




        //// render the loaded model
        //model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        //model = glm::scale(model, glm::vec3(1.0f));	// it's a bit too big for our scene, so scale it down
        //ourShader.setMat4("model", model);
        // 获取模型矩阵并传递到着色器
        
        //cout << tentmodel.position << endl;
        glm::mat4 tent_modelMatrix = tentmodel.GetModelMatrix();
        ourShader.setMat4("model", tent_modelMatrix);
        tentmodel.Draw(ourShader);

        glm::mat4 tree_modelMatrix2 = treemodel_2.GetModelMatrix();
        ourShader2.setMat4("model", tree_modelMatrix2);
        treemodel_2.Draw(ourShader2);

        glm::mat4 tree_modelMatrix = treemodel.GetModelMatrix();
        ourShader3.setMat4("model", tree_modelMatrix);
        treemodel.Draw(ourShader3);



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
            (*selectedModel).Rotate(yoffset * sensitivity, glm::vec3(-1.0f, 0.0f, 0.0f));  // 绕X轴旋转
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