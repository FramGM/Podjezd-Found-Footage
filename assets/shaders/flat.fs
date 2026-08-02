#version 330
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;
in vec3 fragPos;

uniform sampler2D texture0; // Base texture

#define MAX_LIGHTS 16
uniform int lightsCount;
uniform vec3 lightsPos[MAX_LIGHTS];
uniform vec3 lightsColor[MAX_LIGHTS];

out vec4 finalColor;

void main()
{
    // Use standard UVs with some tiling
    vec2 uv = fragTexCoord * 4.0;
    vec4 texColor = texture(texture0, uv);
    
    finalColor = texColor;
}
