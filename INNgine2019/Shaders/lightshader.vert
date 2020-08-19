#version 330 core

layout(location = 0) in vec3 posAttr;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

uniform mat4 pMatrix;
uniform mat4 vMatrix;
uniform mat4 mMatrix;

void main() {
    gl_Position = pMatrix * vMatrix * mMatrix * vec4(posAttr, 1.0);
    FragPos = vec3(mMatrix * vec4(posAttr, 1.0));
//    Normal = aNormal;
    // normal matrix to get the normal in world space
    //TODO calculate normal matrix on CPU and send in as uniform
    Normal = mat3(transpose(inverse(mMatrix))) * aNormal;
    TexCoords = aTexCoords;
}
