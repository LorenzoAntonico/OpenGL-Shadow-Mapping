#include "Plane.h"
#include <glad/glad.h>

Plane::Plane() {
    float planeVertices[] = {
        // positions            // normals         // texcoords
        -1.0f, 0.0f, -1.0f,     0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
         1.0f, 0.0f, -1.0f,     0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
         1.0f, 0.0f,  1.0f,     0.0f, 1.0f, 0.0f,   1.0f, 1.0f,

         1.0f, 0.0f,  1.0f,     0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
        -1.0f, 0.0f,  1.0f,     0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
        -1.0f, 0.0f, -1.0f,     0.0f, 1.0f, 0.0f,   0.0f, 0.0f
    };

    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);

    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texcoords attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Plane::render() {
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
