#version 330 core

// Input
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec2 aTexCoords;       // Texture coordinates

// Output data, to be interpolated for each fragment
out vec3 color;
out vec3 worldPosition;
//out vec3 worldNormal;
out vec4 fragLightSpacePosition;
out vec2 TexCoords;         // Pass texture coordinates to fragment shader

uniform mat4 MVP;
//uniform mat4 LVP;
uniform mat4 model;               // Model matrix

void main() {
    // Transform vertex
    gl_Position =  MVP * vec4(vertexPosition, 1);
    //fragLightSpacePosition = LVP * vec4(vertexPosition, 1.0);
    // Pass vertex color to the fragment shader
    color = vertexColor;
    TexCoords = aTexCoords;

    // World-space geometry
    // Transform position and normal to world space
    //worldPosition = vec3(model * vec4(vertexPosition, 1.0));
    //worldNormal = mat3(transpose(inverse(model))) * vertexNormal;
    worldPosition = vertexPosition;
    worldNormal = vertexNormal;
}
