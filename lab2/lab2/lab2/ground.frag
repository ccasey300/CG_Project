#version 330 core

in vec2 texCoords;

uniform sampler2D textureSampler;

out vec4 FragColor;

void main() {
    FragColor = texture(textureSampler, texCoords);
}
