
#include "../Shader.h"
#include "../glm/glm.hpp"

class DirectionalLight {
public:
    DirectionalLight(glm::vec3 direction, float ambient, float diffuse, float specular);

    void apply(Shader& shader) const;

    // getter per enabled

    void setIntensity(float value);
    float getIntensity();
    

    
    

private:
    float intensity;
    glm::vec3 direction;
    float ambient;
    float diffuse;
    float specular;
};
