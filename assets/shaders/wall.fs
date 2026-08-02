#version 330
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;
in vec3 fragPos;

uniform sampler2D texture0; // Green wall texture (seamless)
uniform sampler2D texture1; // White wall texture (seamless)

#define MAX_LIGHTS 16
uniform int lightsCount;
uniform vec3 lightsPos[MAX_LIGHTS];
uniform vec3 lightsColor[MAX_LIGHTS];

out vec4 finalColor;

void main()
{
    vec3 n = abs(fragNormal);
    vec2 uv = vec2(0.0);
    
    // Scale for tiling (1 unit = 0.5 tiles, so 2m wall = 1 tile)
    float scale = 0.5;
    
    if (n.x > n.z && n.x > n.y) {
        uv = vec2(fragPos.z * scale, fragPos.y * scale);
    } else if (n.z > n.x && n.z > n.y) {
        uv = vec2(fragPos.x * scale, fragPos.y * scale);
    } else {
        uv = vec2(fragPos.x * scale, fragPos.z * scale);
    }
    
    // Y < 0.45 corresponds to the bottom green half of the walls
    vec4 texColor;
    if (fragPos.y < 0.45) {
        texColor = texture(texture0, uv);
    } else {
        texColor = texture(texture1, uv);
    }
    
    finalColor = texColor * fragColor;
}
