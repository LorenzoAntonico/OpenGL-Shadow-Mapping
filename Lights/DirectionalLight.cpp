#include "DirectionalLight.h"



DirectionalLight::DirectionalLight(glm::vec3 direction, float ambient, float diffuse, float specular) : 
intensity(0.0f),
direction(direction),
ambient(ambient),
diffuse(diffuse),
specular(specular)
{
}

void DirectionalLight::apply(Shader& shader) const {
   
    shader.setVec3("dirLight.direction", direction);
    shader.setVec3("dirLight.ambient", glm::vec3(ambient) );
    shader.setVec3("dirLight.diffuse", glm::vec3(diffuse)* intensity);
    shader.setVec3("dirLight.specular", glm::vec3(specular)* intensity);
}

void DirectionalLight::setIntensity(float value) {   
        intensity = value;
}

float DirectionalLight::getIntensity() {
    return intensity;
}


