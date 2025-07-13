#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "shader.h"
#include "camera.h"
#include "texture2D.h"
#include "Cube.h"


#include <iostream>
#include "Lights/DirectionalLight.h"
#include "Lights/PointLight.h"
#include "Lights/SpotLight.h"
#include "Plane.h"
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, Shader& shader);
unsigned int loadTexture(const char* path);
void renderScene(const Shader& shader);
void renderCubeLight(const Shader& shader);
void processLight(Shader& shader);
void setMatForScene(const Shader& shader);
void setMatForDirLight(const Shader& shader);
void renderQuad();



// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
//parametri shadow map  
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

// camera
Camera camera(glm::vec3(0.0f, 2.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float lightChangeCooldown = 0.15f;   // tempo minimo tra modifiche (in secondi)
float lightChangeTimer = 0.0f;       // quanto tempo passato dall'ultima modifica

glm::vec3 dirLightDir = glm::normalize(glm::vec3(-1.0f, -1.5f, -1.0f));
DirectionalLight dirLight(dirLightDir, 0.10f, 0.4f, 0.5f);

glm::vec3 pointLightPos = glm::vec3(0.0f, 1.5f, -2.5f);
PointLight pointLight(pointLightPos, 0.05f, 0.8f, 1.0f);

glm::vec3 spotLightPos = glm::vec3(0.0f, 4.0f, 0.0f);

SpotLight spotLight(
    spotLightPos,      // posizione sopra il cubo
    glm::vec3(0.0f, -1.0f, 0.0f),     // direzione verso il basso 
    0.05f,                            // ambient 
    0.8f,                             // diffuse
    1.0f,                             // specular
    glm::cos(glm::radians(12.5f)),   // cutoff interno 
    glm::cos(glm::radians(17.5f))    // cutoff esterno 
);

unsigned int cubeDiffuseMap, cubeSpecularMap;
unsigned int planeDiffuseMap, planeSpecularMap;
glm::vec3 rotatedLightPos;
float currentFrame;
glm::mat4 projection;
glm::mat4 view;
glm::mat4 lightSpaceMatrixDir;

Cube* cubo = nullptr;
Plane* piano = nullptr;
bool enablePCF = false;
bool enablePCFPoint = false;
bool enableBias = false;
bool enableBlending = false;




int main()
{

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif


    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);


    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    cubo = new Cube();
    piano = new Plane();


    // build and compile shaders

    Shader cubeShader("vertex_shader.vs", "fragment_shader.fs");
    Shader lightCubeShader("LightCube.vs", "LightCube.fs");
    Shader DepthShader("depth_shader_dir.vs", "depth_shader_dir.fs");
    Shader debugDepthQuad("3.1.1.debug_quad.vs", "3.1.1.debug_quad_depth.fs");
    Shader DepthShaderPoint("depth_shader_point.vs", "depth_shader_point.fs", "depth_shader_point.gs");


    cubeDiffuseMap = loadTexture("Wood034_1K-PNG_Color.png");
    cubeSpecularMap = loadTexture("Wood034_1K-PNG_Color.png");

    planeDiffuseMap = loadTexture("Concrete002_4K_Color.jpg");
    planeSpecularMap = loadTexture("plane_specular_map.png");

    //framebuffer depth map
    unsigned int depthMapDirFBO;
    glGenFramebuffers(1, &depthMapDirFBO);

    //texture depth map
    unsigned int depthMapDir;
    glGenTextures(1, &depthMapDir);
    glBindTexture(GL_TEXTURE_2D, depthMapDir);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //attacco la texture al framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapDirFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapDir, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);




    //framebuffer depth map
    unsigned int depthMapSpotFBO;
    glGenFramebuffers(1, &depthMapSpotFBO);

    //texture depth map
    unsigned int depthMapSpot;
    glGenTextures(1, &depthMapSpot);
    glBindTexture(GL_TEXTURE_2D, depthMapSpot);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 }; // nessuna ombra fuori dai bordi
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    //attacco la texture al framebuffer
    // 1. Bind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapSpotFBO);

    // 2. Attacca la texture di profondita
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapSpot, 0);

    // 3. Disabilita color buffer
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);


 
    glBindFramebuffer(GL_FRAMEBUFFER, 0);





    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth cubemap texture
    unsigned int depthCubemap;
    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);





    cubeShader.use();
    cubeShader.setInt("material.diffuse", 0);
    cubeShader.setInt("material.specular", 1);
    cubeShader.setFloat("material.shininess", 32.0f);

    cubeShader.setInt("shadowMapDir", 2);
    cubeShader.setInt("depthMapPoint", 3);
    cubeShader.setInt("shadowMapSpot", 4);

    debugDepthQuad.use();
    debugDepthQuad.setInt("depthMap", 0);






    // render loop

    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic

        currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        lightChangeTimer += deltaTime;


        // input

        processInput(window, cubeShader);


        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //creazione shadow map della luce direzionale 
        setMatForDirLight(DepthShader);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapDirFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        renderScene(DepthShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //creazione shadow map per luce punto
        float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
        float near = 1.0f;
        float far = 30.0f;
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);

        std::vector<glm::mat4> shadowTransforms;

        shadowTransforms.push_back(shadowProj * glm::lookAt(rotatedLightPos, rotatedLightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(rotatedLightPos, rotatedLightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(rotatedLightPos, rotatedLightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0))); 
        shadowTransforms.push_back(shadowProj * glm::lookAt(rotatedLightPos, rotatedLightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0))); 
        shadowTransforms.push_back(shadowProj * glm::lookAt(rotatedLightPos, rotatedLightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(rotatedLightPos, rotatedLightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        DepthShaderPoint.use();
        for (unsigned int i = 0; i < 6; ++i)
            DepthShaderPoint.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
        DepthShaderPoint.setFloat("far_plane", far);
        DepthShaderPoint.setVec3("lightPos", rotatedLightPos);
        renderScene(DepthShaderPoint);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);






        glm::vec3 spotLightPos = spotLight.getPosition();
        glm::vec3 spotLightDir = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f));
        
       
        float near_plane = 1.0f;
        float far_plane = 5.0f;

        float fov = glm::degrees(acos(spotLight.getCutOff())) * 2.0f;

        glm::mat4 lightProjection = glm::perspective(glm::radians(fov), 1.0f, near_plane, far_plane);

        glm::vec3 spotDir = glm::normalize(spotLight.getDirection());
        glm::vec3 up = glm::vec3(1.0, 0.0, 0.0);
       

        glm::mat4 lightView = glm::lookAt(spotLight.getPosition(), spotLightPos + spotDir, up);
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;


        DepthShader.use();
        DepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapSpotFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        renderScene(DepthShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBindTexture(GL_TEXTURE_2D, depthMapSpot);
        









        // 2. render della scena 
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        cubeShader.use();
        cubeShader.setMat4("lightSpaceMatrixDir", lightSpaceMatrixDir);
        cubeShader.setMat4("lightSpaceMatrixSpot", lightSpaceMatrix);
        setMatForScene(cubeShader);


        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, depthMapDir);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, depthMapSpot);

        cubeShader.setInt("shadows", true); 
        cubeShader.setFloat("far_plane", far);

        processLight(cubeShader);
        renderScene(cubeShader);
        renderCubeLight(lightCubeShader);



        /*debugDepthQuad.use();
        debugDepthQuad.setFloat("near_plane", near_plane);
        debugDepthQuad.setFloat("far_plane", far_plane);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMapSpot);
        renderQuad();*/







        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window, Shader& shader)
{
    static bool key1Pressed = false;
    static bool key2Pressed = false;
    static bool key3Pressed = false;
    static bool key4Pressed = false;

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
    float intensitySpeed = 0.5f; // intensit� al secondo

    //directionalLight

    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        float newIntensity = pointLight.getIntensity() - intensitySpeed * deltaTime;
        pointLight.setIntensity(std::max(newIntensity, 0.0f)); 
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        float newIntensity = pointLight.getIntensity() + intensitySpeed * deltaTime;
        pointLight.setIntensity(std::min(newIntensity, 1.0f)); 
    }
    //pointlight

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        float newIntensity = dirLight.getIntensity() + intensitySpeed * deltaTime;
        dirLight.setIntensity(std::min(newIntensity, 1.0f)); 
    }

    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        float newIntensity = dirLight.getIntensity() - intensitySpeed * deltaTime;
        dirLight.setIntensity(std::max(newIntensity, 0.0f)); 
    }
    //spotlight
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        float newIntensity = spotLight.getIntensity() + intensitySpeed * deltaTime;
        spotLight.setIntensity(std::min(newIntensity, 1.0f)); 
    }

    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        float newIntensity = spotLight.getIntensity() - intensitySpeed * deltaTime;
        spotLight.setIntensity(std::max(newIntensity, 0.0f)); 
    }



    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !key1Pressed)
    {
        enablePCF = !enablePCF;
        shader.use();
        shader.setBool("enablePCF", enablePCF);
        key1Pressed = true;

    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE)
    {
        key1Pressed = false;
    }



    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !key2Pressed)
    {
        enableBias = !enableBias;
        shader.use();
        shader.setBool("enableBias", enableBias);
        key2Pressed = true;

    }

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE)
    {
        key2Pressed = false;
    }


    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && !key3Pressed)
    {
        enablePCFPoint = !enablePCFPoint;
        shader.use();
        shader.setBool("enablePCFPoint", enablePCFPoint);
        key3Pressed = true;

    }

    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE)
    {
        key3Pressed = false;
    }

    

        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && !key4Pressed)
        {
            enableBlending = !enableBlending;
            shader.use();
            shader.setBool("enableBlending", enableBlending);
            key4Pressed = true;

        }

    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_RELEASE)
    {
        key4Pressed = false;
    }


}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}



void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
void renderPlane() {
    piano->render();
}
void renderCube()
{
    cubo->render();
}

void renderScene(const Shader& shaderC)
{

    shaderC.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeDiffuseMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, cubeSpecularMap);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f));
    shaderC.setMat4("model", model);
    renderCube();

    shaderC.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeDiffuseMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, cubeSpecularMap);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(5.0f, 0.5f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f));
    shaderC.setMat4("model", model);
    renderCube();

    //pavimento
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, planeDiffuseMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, planeSpecularMap);
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(9.0f, 2.0f, 9.0f));
    shaderC.setMat4("model", model);
    renderPlane();

    


}

void renderCubeLight(const Shader& shader) {
    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, rotatedLightPos);
    model = glm::scale(model, glm::vec3(0.2f));
    shader.setMat4("model", model);
    renderCube();

    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    model = glm::mat4(1.0f);
    model = glm::translate(model, spotLightPos);
    model = glm::scale(model, glm::vec3(0.2f));
    shader.setMat4("model", model);
    renderCube();
}


void setMatForScene(const Shader& shader) {
    shader.use();
    projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    view = camera.GetViewMatrix();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
}

void processLight(Shader& shader) {
    shader.use();
    shader.setVec3("viewPos", camera.Position);
    dirLight.apply(shader);
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), currentFrame, glm::vec3(0.0f, 1.0f, 0.0f));
    rotatedLightPos = glm::vec3(rotation * glm::vec4(pointLightPos, 1.0f));

    pointLight.setPosition(rotatedLightPos);
    pointLight.apply(shader);
    spotLight.apply(shader);
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void calcLightSpaceMatrixDir() {
    glm::mat4 lightProjection, lightView;

    float near_plane = 1.0f, far_plane = 20.0f;
    lightProjection = glm::ortho(-13.0f, 13.0f, -10.0f, 10.0f, near_plane, far_plane);
    float dist = 10.0f;
    glm::vec3 sceneCenter = glm::vec3(0.0, 0.5, 0.0);
    glm::vec3 lightPos = sceneCenter - dirLightDir * dist;
    lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    lightSpaceMatrixDir = lightProjection * lightView;

}

void setMatForDirLight(const Shader& shader) {
    calcLightSpaceMatrixDir();
    shader.use();
    shader.setMat4("lightSpaceMatrix", lightSpaceMatrixDir);
}


