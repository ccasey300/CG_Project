#version 330 core

in vec3 worldNormal;        // Normal in world space
in vec3 worldPosition;      // Position in world space
in vec2 TexCoords;          // Texture coordinates passed from vertex shader

out vec4 finalColor;        // Final output color

uniform vec3 lightPosition; // Light position
uniform vec3 lightIntensity; // Light intensity
uniform sampler2D textureSampler; // Texture sampler

void main()
{
    // Normalize vectors
    vec3 Norm = normalize(worldNormal);
    vec3 Direction = normalize(lightPosition - worldPosition);

    // Texture sampling
    vec3 textureColor = texture(textureSampler, TexCoords).rgb;

    // Lighting: Diffuse
    vec3 diffuse = (max(dot(Norm, Direction), 0.0) * lightIntensity * textureColor * 0.0001);

    // Tone mapping
    vec3 tonemap = diffuse / (diffuse + vec3(1.0));

    // Gamma correction
    vec3 gamma_correct = pow(tonemap, vec3(1.0 / 2.2));

    // Final color output
    finalColor = vec4(gamma_correct, 1.0);
}
