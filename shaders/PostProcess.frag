// Licensed under the Apache License, Version 2.0
// Post-Processing Shader for Bas Veeg Arc 3D
// Includes: Bloom, HDR, Motion Blur effects

#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D sceneTexture;
uniform sampler2D brightTexture;
uniform float time;
uniform float bloomStrength;
uniform float exposure;
uniform float saturation;

// Chromatic aberration parameters
const float aberrationStrength = 0.002;

vec3 chromaticAberration(sampler2D tex, vec2 uv) {
    vec2 direction = uv - vec2(0.5);
    float r = texture(tex, uv + direction * aberrationStrength).r;
    float g = texture(tex, uv).g;
    float b = texture(tex, uv - direction * aberrationStrength).b;
    return vec3(r, g, b);
}

vec3 bloom(sampler2D tex, vec2 uv, float strength) {
    vec3 result = vec3(0.0);
    float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

    vec2 texOffset = 1.0 / textureSize(tex, 0);
    result += texture(tex, uv).rgb * weights[0];

    for(int i = 1; i < 5; ++i) {
        result += texture(tex, uv + vec2(texOffset.x * i, 0.0)).rgb * weights[i];
        result += texture(tex, uv - vec2(texOffset.x * i, 0.0)).rgb * weights[i];
        result += texture(tex, uv + vec2(0.0, texOffset.y * i)).rgb * weights[i];
        result += texture(tex, uv - vec2(0.0, texOffset.y * i)).rgb * weights[i];
    }

    return result * strength;
}

vec3 adjustSaturation(vec3 color, float saturation) {
    float gray = dot(color, vec3(0.299, 0.587, 0.114));
    return mix(vec3(gray), color, saturation);
}

// Vignette effect
float vignette(vec2 uv) {
    vec2 pos = uv - vec2(0.5);
    return 1.0 - dot(pos, pos) * 0.8;
}

void main() {
    vec2 uv = TexCoord;

    // Chromatic aberration
    vec3 color = chromaticAberration(sceneTexture, uv);

    // Add bloom
    vec3 bloomColor = bloom(brightTexture, uv, bloomStrength);
    color += bloomColor;

    // HDR tone mapping (Reinhard)
    color = vec3(1.0) - exp(-color * exposure);

    // Adjust saturation
    color = adjustSaturation(color, saturation);

    // Apply vignette
    color *= vignette(uv);

    // Subtle scan line effect for style
    float scanline = sin(uv.y * 800.0 + time * 10.0) * 0.02 + 0.98;
    color *= scanline;

    // Gamma correction
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
}
