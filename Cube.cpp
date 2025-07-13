#include "Cube.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"



Cube::Cube(){
    float vertices[] = {
    // positions          // normals           // texcoords
    // front face
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,

    // back face
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,-1.0f,   0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 0.0f,-1.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 0.0f,-1.0f,   1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 0.0f,-1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 0.0f,-1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,-1.0f,   0.0f, 0.0f,

    // left face
    -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,

    // right face
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   1.0f, 0.0f,

     // bottom face
     -0.5f, -0.5f, -0.5f,  0.0f,-1.0f, 0.0f,   0.0f, 1.0f,
      0.5f, -0.5f, -0.5f,  0.0f,-1.0f, 0.0f,   1.0f, 1.0f,
      0.5f, -0.5f,  0.5f,  0.0f,-1.0f, 0.0f,   1.0f, 0.0f,
      0.5f, -0.5f,  0.5f,  0.0f,-1.0f, 0.0f,   1.0f, 0.0f,
     -0.5f, -0.5f,  0.5f,  0.0f,-1.0f, 0.0f,   0.0f, 0.0f,
     -0.5f, -0.5f, -0.5f,  0.0f,-1.0f, 0.0f,   0.0f, 1.0f,

     // top face
     -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
      0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
     -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
     -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f
};

glGenVertexArrays(1, &cubeVAO);
glGenBuffers(1, &cubeVBO);
glBindVertexArray(cubeVAO);

glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

// position attribute
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
// normal attribute
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
glEnableVertexAttribArray(1);
// texture coords attribute
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
glEnableVertexAttribArray(2);

glBindBuffer(GL_ARRAY_BUFFER, 0);
glBindVertexArray(0);

}
void Cube::render() {
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}




