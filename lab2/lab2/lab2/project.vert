//below previously worked:
#version 330 core

// Input
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec2 vertexUV;
layout(location = 3) in vec3 vertexNormal;    // Vertex normal

// Output data, to be interpolated for each fragment
out vec3 color;
out vec2 uv;
out vec3 worldNormal;
out vec3 worldPosition;


// TODO: To add UV to this vertex shader

// Matrix for vertex transformation
uniform mat4 model;
uniform mat4 MVP;

void main() {
    // Transform vertex
    gl_Position =  MVP * vec4(vertexPosition, 1);

    // Pass vertex color to the fragment shader
    color = vertexColor;
    uv = vertexUV;

    // Transform normals and position to world space
    worldNormal = mat3(transpose(inverse(model))) * vertexNormal;
    worldPosition = vec3(model * vec4(vertexPosition, 1.0));
}