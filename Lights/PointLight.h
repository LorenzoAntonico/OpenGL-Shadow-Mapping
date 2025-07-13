
#include "../Shader.h"
#include "../glm/glm.hpp"

class PointLight {
public:
    PointLight(glm::vec3 position, float ambient, float diffuse, float specular);

    void apply(Shader& shader) const;

    void setIntensity(float value);
    float getIntensity();
    void setPosition(glm::vec3 position);
    glm::vec3 getDirection() const;


private:
    float intensity;
    glm::vec3 position;
    float ambient;
    float diffuse;
    float specular;
    float constant;
    float linear;
    float quadratic;
};

