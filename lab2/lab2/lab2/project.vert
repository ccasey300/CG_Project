#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 VertexColor;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 camPos; // Camera position in world space

void main()
{
    // Recenter the skybox vertices around the camera position
    vec3 adjustedPos = aPos + camPos;

    // Calculate the final position in clip space
    gl_Position = projection * view * vec4(adjustedPos, 1.0);

    // Pass outputs to fragment shader
    FragPos = adjustedPos;   // World-space position
    VertexColor = aColor;    // Vertex color for debugging
    TexCoord = aTexCoord;    // Texture coordinates
}


//below previously worked:
#version 330 core

// Input
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec2 vertexUV;

// Output data, to be interpolated for each fragment
out vec3 color;
out vec2 uv;

// TODO: To add UV to this vertex shader

// Matrix for vertex transformation
uniform mat4 MVP;

void main() {
    // Transform vertex
    gl_Position =  MVP * vec4(vertexPosition, 1);

    // Pass vertex color to the fragment shader
    color = vertexColor;

    // TODO: Pass UV to the fragment shader
    uv = vertexUV;
}