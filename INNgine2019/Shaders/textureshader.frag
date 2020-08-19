#version 330 core

in vec2 UV;
uniform sampler2D textureSampler;
uniform vec3 objectColor;

out vec3 fragColor;

void main() {
        fragColor = texture(textureSampler, UV).rgb * objectColor;
}
