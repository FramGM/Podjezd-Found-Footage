#include "CVHSEffect.h"

const char* g_pszVHSVertexShaderCode = R"(
#version 330
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;
out vec2 fragTexCoord;
out vec4 fragColor;
uniform mat4 mvp;
void main()
{
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
)";

const char* g_pszVHSFragmentShaderCode = R"(
#version 330
in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform sampler2D texture0;
uniform float time;
uniform vec2 renderSize;
uniform float vhsIntensity;
uniform float flashlightOn;
uniform float enableVHSTapeEffects;
uniform float ditherIntensity;

float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    vec2 uv = fragTexCoord;
    vec3 col = vec3(0.0);

    if (enableVHSTapeEffects > 0.5) {
        // Lens distortion
        vec2 d = uv - vec2(0.5);
        float r2 = d.x * d.x + d.y * d.y;
        uv = uv + d * (r2 * 0.02 * vhsIntensity);

        // Chromatic Aberration
        float caOffset = (0.002 + 0.001 * sin(time * 7.0)) * vhsIntensity;
        float r = texture(texture0, vec2(uv.x + caOffset, uv.y)).r;
        float g = texture(texture0, uv).g;
        float b = texture(texture0, vec2(uv.x - caOffset, uv.y)).b;
        col = vec3(r, g, b);
    } else {
        col = texture(texture0, uv).rgb;
    }

    // Dynamic Found Footage Flashlight & Dark Atmosphere
    vec2 aspectUV = (uv - vec2(0.5)) * vec2(renderSize.x / renderSize.y, 1.0);
    float distFromCenter = length(aspectUV);

    // Spotlight cone beam (Smooth transition from 95% clear center down to dark edges)
    float spotCone = clamp(1.0 - smoothstep(0.10, 0.45, distFromCenter), 0.0, 1.0);
    float lightMultiplier = mix(0.15, 0.95, spotCone);

    if (flashlightOn < 0.5) {
        lightMultiplier = 0.12; // Dim horror ambient when flashlight is OFF
    }

    col *= lightMultiplier;

    if (enableVHSTapeEffects > 0.5) {
        // Scanlines
        float scanline = sin(uv.y * renderSize.y * 1.5) * 0.035 * vhsIntensity;
        col -= scanline;

        // Film Grain / Noise
        float noise = (rand(uv + vec2(time * 0.12, time * 0.06)) - 0.5) * 0.04 * vhsIntensity;
        col += noise;

        // Tape artifacts
        float lineNoise = sin(uv.y * 120.0 + time * 40.0);
        if (rand(vec2(floor(time * 18.0), 0.0)) > 0.94) {
            col += 0.02 * lineNoise * vhsIntensity;
        }
    }

    // Dithering (Ordered 4x4 Bayer Matrix)
    float bayer[16] = float[](
        0.0/16.0, 8.0/16.0, 2.0/16.0, 10.0/16.0,
        12.0/16.0, 4.0/16.0, 14.0/16.0, 6.0/16.0,
        3.0/16.0, 11.0/16.0, 1.0/16.0, 9.0/16.0,
        15.0/16.0, 7.0/16.0, 13.0/16.0, 5.0/16.0
    );
    int dX = int(mod(gl_FragCoord.x, 4.0));
    int dY = int(mod(gl_FragCoord.y, 4.0));
    float dither = bayer[dX + dY * 4] - 0.5;
    
    // Apply dithering and subtle color quantization
    float colorLevels = 48.0; // Number of color steps per channel
    col += (dither / colorLevels) * ditherIntensity;
    
    if (ditherIntensity > 0.0) {
        col = floor(col * colorLevels + 0.5) / colorLevels;
    }

    finalColor = vec4(col, 1.0);
}
)";

CVHSEffect::CVHSEffect()
	: m_bEnableVHS(true), m_bEnableVHSTapeEffects(true), m_flVHSIntensity(1.0f), m_flDitherIntensity(0.5f), m_iTimeLoc(-1), m_iRenderSizeLoc(-1), m_iVHSIntensityLoc(-1), m_iFlashlightOnLoc(-1), m_iEnableVHSTapeEffectsLoc(-1), m_iDitherIntensityLoc(-1)
{
}

void CVHSEffect::Init(int iWidth, int iHeight)
{
	m_RenderTarget = LoadRenderTexture(iWidth, iHeight);
	m_VHSShader = LoadShaderFromMemory(g_pszVHSVertexShaderCode, g_pszVHSFragmentShaderCode);

	m_iTimeLoc = GetShaderLocation(m_VHSShader, "time");
	m_iRenderSizeLoc = GetShaderLocation(m_VHSShader, "renderSize");
	m_iVHSIntensityLoc = GetShaderLocation(m_VHSShader, "vhsIntensity");
	m_iFlashlightOnLoc = GetShaderLocation(m_VHSShader, "flashlightOn");
	m_iEnableVHSTapeEffectsLoc = GetShaderLocation(m_VHSShader, "enableVHSTapeEffects");
	m_iDitherIntensityLoc = GetShaderLocation(m_VHSShader, "ditherIntensity");

	Vector2 vecRes = Vector2{ static_cast<float>(iWidth), static_cast<float>(iHeight) };
	SetShaderValue(m_VHSShader, m_iRenderSizeLoc, &vecRes, SHADER_UNIFORM_VEC2);
}

void CVHSEffect::Resize(int iWidth, int iHeight)
{
	UnloadRenderTexture(m_RenderTarget);
	m_RenderTarget = LoadRenderTexture(iWidth, iHeight);

	Vector2 vecRes = Vector2{ static_cast<float>(iWidth), static_cast<float>(iHeight) };
	SetShaderValue(m_VHSShader, m_iRenderSizeLoc, &vecRes, SHADER_UNIFORM_VEC2);
}

void CVHSEffect::BeginRender()
{
	BeginTextureMode(m_RenderTarget);
	ClearBackground(Color{ 18, 22, 20, 255 });
}

void CVHSEffect::EndRender()
{
	EndTextureMode();
}

void CVHSEffect::RenderToScreen(bool bFlashlightOn)
{
	float flTime = static_cast<float>(GetTime());
	float flFlashOn = bFlashlightOn ? 1.0f : 0.0f;
	float flEnableTapeEffects = m_bEnableVHSTapeEffects ? 1.0f : 0.0f;

	SetShaderValue(m_VHSShader, m_iTimeLoc, &flTime, SHADER_UNIFORM_FLOAT);
	SetShaderValue(m_VHSShader, m_iVHSIntensityLoc, &m_flVHSIntensity, SHADER_UNIFORM_FLOAT);
	SetShaderValue(m_VHSShader, m_iFlashlightOnLoc, &flFlashOn, SHADER_UNIFORM_FLOAT);
	SetShaderValue(m_VHSShader, m_iEnableVHSTapeEffectsLoc, &flEnableTapeEffects, SHADER_UNIFORM_FLOAT);
	SetShaderValue(m_VHSShader, m_iDitherIntensityLoc, &m_flDitherIntensity, SHADER_UNIFORM_FLOAT);

	Rectangle sourceRec = Rectangle{ 0, 0, static_cast<float>(m_RenderTarget.texture.width), static_cast<float>(-m_RenderTarget.texture.height) };
	Vector2 zeroPos = Vector2{ 0, 0 };

	if (m_bEnableVHS)
	{
		BeginShaderMode(m_VHSShader);
		DrawTextureRec(m_RenderTarget.texture, sourceRec, zeroPos, WHITE);
		EndShaderMode();
	}
	else
	{
		DrawTextureRec(m_RenderTarget.texture, sourceRec, zeroPos, WHITE);
	}
}

void CVHSEffect::RenderFoundFootageHUD(bool bIsFPSMode, bool bFlashlightOn)
{
	int iWinW = GetScreenWidth();
	int iWinH = GetScreenHeight();
	float flTime = static_cast<float>(GetTime());

	// REC ● Indicator
	if (static_cast<int>(flTime * 2.0f) % 2 == 0)
	{
		DrawCircle(45, 40, 8, RED);
	}
	DrawText("REC", 60, 31, 22, WHITE);
	DrawText("PLAY  [AUTO]", iWinW - 180, 31, 20, GREEN);

	// Date stamp
	char szDateStr[64];
	int iSeconds = static_cast<int>(flTime);
	snprintf(szDateStr, sizeof(szDateStr), "12 OCT 1989  %02d:%02d:%02d",
		(14 + iSeconds / 3600) % 24, (23 + (iSeconds / 60)) % 60, iSeconds % 60);
	DrawText(szDateStr, 40, iWinH - 50, 22, YELLOW);

	// Flashlight status & Reticle
	if (bFlashlightOn)
	{
		DrawText("LIGHT: ON [F]", iWinW - 180, iWinH - 50, 18, WHITE);
	}
	else
	{
		DrawText("LIGHT: OFF [F]", iWinW - 180, iWinH - 50, 18, GRAY);
	}

	//if (bIsFPSMode)
	//{
	//	DrawCircle(iWinW / 2, iWinH / 2, 3, ColorAlpha(WHITE, 0.7f));
	//	DrawCircleLines(iWinW / 2, iWinH / 2, 12, ColorAlpha(WHITE, 0.3f));
	//}
}

void CVHSEffect::Shutdown()
{
	UnloadShader(m_VHSShader);
	UnloadRenderTexture(m_RenderTarget);
}
