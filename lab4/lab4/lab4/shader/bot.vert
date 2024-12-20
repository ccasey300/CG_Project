#version 330 core

// Input
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;

// Linear blend skinning inputs
layout(location = 3) in ivec4 jointIndices; // Joint indices
layout(location = 4) in vec4 jointWeights;  // Joint weights

// Output data, to be interpolated for each fragment
out vec3 worldPosition;
out vec3 worldNormal;

// Uniforms
uniform mat4 MVP;                // Model-View-Projection matrix
uniform mat4 jointMatrices[25];  // Array of joint transformation matrices

void main() {
    // Initialize the skinned position and normal
    vec4 skinnedPosition = vec4(0.0);
    vec3 skinnedNormal = vec3(0.0);

    // Apply linear blend skinning using joint indices and weights
    for (int i = 0; i < 4; ++i) {
        if (jointWeights[i] > 0.0) {
            mat4 jointMatrix = jointMatrices[jointIndices[i]];
            skinnedPosition += jointWeights[i] * (jointMatrix * vec4(vertexPosition, 1.0));
            skinnedNormal += jointWeights[i] * mat3(jointMatrix) * vertexNormal;
        }
    }

    // Transform vertex to clip space
    gl_Position = MVP * skinnedPosition;

    // Output world-space geometry
    worldPosition = skinnedPosition.xyz;
    worldNormal = normalize(skinnedNormal);
}
