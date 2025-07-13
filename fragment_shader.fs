#version 330 core

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}; 

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};
 
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;
in vec4 FragPosLightSpaceDir;
in vec4 FragPosLightSpaceSpot;

out vec4 FragColor;


uniform DirLight dirLight;
uniform Material material;
uniform PointLight pointLight;
uniform SpotLight spotLight;

uniform sampler2D shadowMapDir;
uniform sampler2D shadowMapSpot;
uniform samplerCube depthMapPoint;

uniform vec3 viewPos;
uniform bool enablePCF;
uniform bool enablePCFPoint;
uniform bool enableBias;
uniform float far_plane;
uniform bool shadows;
uniform bool enableBlending;

//funzione che calcola luce direzionale
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);  

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);  

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

float ShadowCalculation(vec4 FragPosLightSpaceDir, vec3 normal, DirLight light);

float ShadowCalculationPoint(vec3 fragPos);

float ShadowCalculationSpot(vec4 FragPosLightSpaceSpot, vec3 normal, SpotLight light);




void main()
{

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 dirLightDir = normalize(-dirLight.direction);


    bool litByDirLight = dot(norm, dirLightDir) > 0.1; 

    float shadowSpot = ShadowCalculationSpot(FragPosLightSpaceSpot, norm, spotLight);

    float distToPointLight = length(pointLight.position - FragPos);
    float minDistSpot = 2.7f;   // distanza in cui l'ombra viene annullata
    float maxDistSpot = 4.2f;   // distanza oltre cui l'ombra è completa
    bool pointLightEnabled = length(pointLight.diffuse) > 0.01;

    float fadeSpot=1.0f;
    if (pointLightEnabled) {
    fadeSpot = clamp((distToPointLight - minDistSpot) / (maxDistSpot - minDistSpot), 0.0, 1.0);
    }

    shadowSpot *= fadeSpot;

    if (litByDirLight&& length(dirLight.diffuse) > 0.01) {
    shadowSpot = 0.0;
}


    float shadowDir = ShadowCalculation(FragPosLightSpaceDir, norm, dirLight);
    float minDistDir = 2.5f;
    float maxDistDir = 5.0f;

    float fadeDir = 1.0f;
    if (pointLightEnabled) {
      fadeDir = clamp((distToPointLight - minDistDir) / (maxDistDir - minDistDir), 0.0, 1.0);
    }
    shadowDir *= fadeDir;
    

    float shadowPoint = 0.0;
    if (length(pointLight.diffuse) > 0.001)
    shadowPoint = ShadowCalculationPoint(FragPos);

    

    float blendedShadow = max(max(shadowDir, shadowPoint), shadowSpot);




    vec3 ambient  = dirLight.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 lightColor = vec3(0.0);
    lightColor += CalcDirLight(dirLight, norm, viewDir);
    lightColor += CalcPointLight(pointLight, norm, FragPos, viewDir);
    lightColor += CalcSpotLight(spotLight, norm, FragPos, viewDir);

    vec3 result = ambient + (1.0 - blendedShadow) * lightColor;



   
    
   FragColor = vec4(result,1.0);
   

}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    
    float shadow = ShadowCalculation(FragPosLightSpaceDir, normal, light);

    
  
      return ambient +  (diffuse + specular);
}


vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    
    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
   
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    float shadow = ShadowCalculationPoint(FragPos);
    
    return ambient +  (diffuse + specular);
    

} 

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    float shadow = ShadowCalculationSpot(FragPosLightSpaceSpot, normal, light);


    return ambient + (diffuse + specular);

    
}

float ShadowCalculation(vec4 FragPosLightSpaceDir,vec3 normal,DirLight light)
{

    if (length(light.diffuse) < 0.001)
        return 0.0; // luce spenta 
    
    vec3 projCoords = FragPosLightSpaceDir.xyz / FragPosLightSpaceDir.w;
   
    projCoords = projCoords * 0.5 + 0.5;

   
    
    float currentDepth = projCoords.z;
   


     float bias=0;
     if(enableBias==true){
          bias = max(0.005 * (1.0 - dot(normal, light.direction)), 0.0005);
     }else if(enableBias==false){
         bias=0;
     }


    float shadow = 0.0;
    if(enablePCF==true){
    vec2 texelSize = 1.0 / textureSize(shadowMapDir, 0);
     for(int x = -1; x <= 1; ++x)
      {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMapDir, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
      }
    
    shadow /= 9.0;
    }else{
    
    float closestDepth = texture(shadowMapDir, projCoords.xy).r; 
    shadow = currentDepth-bias > closestDepth  ? 1.0 : 0.0;
    }
    
    
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}  

float ShadowCalculationPoint(vec3 fragPos)
{
    vec3 fragToLight = fragPos - pointLight.position;
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias = 0.15;

    if (enablePCFPoint)
    {
        int samples = 20;
        float viewDistance = length(viewPos - fragPos);
        float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;

        for (int i = 0; i < samples; ++i)
        {
            float closestDepth = texture(depthMapPoint, fragToLight + gridSamplingDisk[i] * diskRadius).r;
            closestDepth *= far_plane;

            if (currentDepth - bias > closestDepth)
                shadow += 1.0;
        }
        shadow /= float(samples);
    }
    else
    {
        float closestDepth = texture(depthMapPoint, fragToLight).r * far_plane;
        shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    }

    return shadow * 0.3;;
}

float ShadowCalculationSpot(vec4 FragPosLightSpaceSpot, vec3 normal, SpotLight light)
{

     if (length(light.diffuse) < 0.001)
        return 0.0; // luce spenta => niente ombra
    
    vec3 projCoords = FragPosLightSpaceSpot.xyz / FragPosLightSpaceSpot.w;
    projCoords = projCoords * 0.5 + 0.5;

   
    float currentDepth = projCoords.z;



   
    
    float bias = 0.0;
    if (enableBias) {
        vec3 lightDir = normalize(light.position - FragPos);
        bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
    }

    float shadow = 0.0;

    if (enablePCF) {
        vec2 texelSize = 1.0 / textureSize(shadowMapSpot, 0);
        for (int x = -1; x <= 1; ++x) {
            for (int y = -1; y <= 1; ++y) {
                float pcfDepth = texture(shadowMapSpot, projCoords.xy + vec2(x, y) * texelSize).r;
                shadow += (currentDepth - bias > pcfDepth) ? 1.0 : 0.0;
            }
        }
        shadow /= 9.0;
    } else {
        float closestDepth = texture(shadowMapSpot, projCoords.xy).r;
        shadow = (currentDepth - bias > closestDepth) ? 1.0 : 0.0;
    }

    
    if (projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}


