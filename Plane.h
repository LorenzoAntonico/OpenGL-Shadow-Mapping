
#include <glad/glad.h>

class Plane {
public:
    Plane();
    void render();

private:
    unsigned int planeVAO = 0;
    unsigned int planeVBO = 0;
};
