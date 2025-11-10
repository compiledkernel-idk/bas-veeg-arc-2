// Licensed under the Apache License, Version 2.0
// Particle Vertex Shader for Bas Veeg Arc 3D

#version 330 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 uv;
layout(location = 3) in float size;

uniform mat4 worldViewProj;
uniform vec3 cameraPos;
uniform vec3 cameraUp;
uniform vec3 cameraRight;

out vec4 ParticleColor;
out vec2 TexCoord;

void main() {
    // Billboard particles to face camera
    vec3 particlePos = vertex;
    particlePos += cameraRight * uv.x * size;
    particlePos += cameraUp * uv.y * size;

    gl_Position = worldViewProj * vec4(particlePos, 1.0);
    ParticleColor = color;
    TexCoord = uv;
}
