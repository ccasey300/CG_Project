#version 330 core

layout(location = 0) in vec3 vertexPosition;

uniform mat4 VP;
uniform vec3 camPos;
uniform float gGridCCellSize;

out vec2 texCoords;

void main() {
    gl_Position = VP * vec4(vertexPosition, 1.0);
    texCoords = vertexPosition.xz / gGridCCellSize + camPos.xz / gGridCCellSize;
}
