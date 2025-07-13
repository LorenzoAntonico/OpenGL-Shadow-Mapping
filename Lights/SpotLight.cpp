#include "SpotLight.h"



SpotLight::SpotLight(glm::vec3 position, glm::vec3 direction, float ambient, float diffuse, float specular, float cutOff, float outerCutOff) :
    intensity(0.0f),
    position(position),
    direction(direction),
    ambient(ambient),
    diffuse(diffuse),
    specular(specular),
    cutOff(cutOff),
    outerCutOff(outerCutOff),
    constant(1.0f),
    linear(0.09f),
    quadratic(0.032f)
    
{
}

void SpotLight::apply(Shader& shader) const {
    
    shader.setVec3("spotLight.position", position);
    shader.setVec3("spotLight.direction", direction);
    shader.setVec3("spotLight.ambient", glm::vec3(ambient)*intensity);
    shader.setVec3("spotLight.diffuse", glm::vec3(diffuse)*intensity);
    shader.setVec3("spotLight.specular", glm::vec3(specular)*intensity);
    shader.setFloat("spotLight.constant", constant);
    shader.setFloat("spotLight.linear", linear);
    shader.setFloat("spotLight.quadratic", quadratic);
    shader.setFloat("spotLight.cutOff", cutOff);
    shader.setFloat("spotLight.outerCutOff", outerCutOff);
}

void SpotLight::setIntensity(float value) {
    intensity = value;
}

float SpotLight::getIntensity() {
    return intensity;
}

glm::vec3 SpotLight::getPosition() const {
    return position;
}

glm::vec3 SpotLight::getDirection() const {
    return direction;
}

float SpotLight::getCutOff() const {
    return cutOff;
}

// Setter
void SpotLight::setPosition(const glm::vec3& pos) {
    position = pos;
}

void SpotLight::setDirection(const glm::vec3& dir) {
    direction = dir;
}

void SpotLight::setCutOff(float cutoffAngle) {
    cutOff = cutoffAngle;
}

