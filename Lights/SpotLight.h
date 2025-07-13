
#include "../Shader.h"
#include "../glm/glm.hpp"

class SpotLight {
public:
    SpotLight(glm::vec3 position, glm::vec3 direction, float ambient, float diffuse, float specular, float cutOff, float outerCutOff);

    void apply(Shader& shader) const;

    void setIntensity(float value);
    float getIntensity();
    glm::vec3 getPosition()const;
    glm::vec3 getDirection() const;
    float getCutOff() const;
    void setPosition(const glm::vec3& pos);
    void setDirection(const glm::vec3& dir);
    void setCutOff(float cutoffAngle);


private:
    float intensity;
    glm::vec3 position;
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    float ambient;
    float diffuse;
    float specular;
    
};


