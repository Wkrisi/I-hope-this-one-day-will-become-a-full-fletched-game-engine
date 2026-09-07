#version 330 core
struct Material
{
  vec3 ambient; 
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

uniform Material material;

out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;
uniform float shininess;

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = material.ambient * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, norm), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;
    
    // result
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
} 
