#include "PointLight.h"



PointLight::PointLight(glm::vec3 position, float ambient, float diffuse, float specular) :
    intensity(0.0f),
    position(position),
    ambient(ambient),
    diffuse(diffuse),
    specular(specular),
    constant(1.0f),
    linear(0.09f),
    quadratic(0.032f)
{
}

void PointLight::apply(Shader& shader) const {
    
    shader.setVec3("pointLight.position", position);
    shader.setVec3("pointLight.ambient", glm::vec3(ambient)*intensity);
    shader.setVec3("pointLight.diffuse", glm::vec3(diffuse)*intensity);
    shader.setVec3("pointLight.specular", glm::vec3(specular)*intensity);
    shader.setFloat("pointLight.constant", constant);
    shader.setFloat("pointLight.linear", linear);
    shader.setFloat("pointLight.quadratic", quadratic);
}

void PointLight::setIntensity(float value) {
    intensity = value;
}

float PointLight::getIntensity() {
    return intensity;
}
void PointLight::setPosition(glm::vec3 Newposition) {
    position = Newposition;
}


