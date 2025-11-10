// Licensed under the Apache License, Version 2.0
// PBR Vertex Shader for Bas Veeg Arc 3D

#version 330 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;
layout(location = 3) in vec2 uv;

uniform mat4 worldViewProj;
uniform mat4 world;
uniform vec3 lightPos;
uniform vec3 cameraPos;

out vec3 FragPos;
out vec3 Normal;
out vec4 Color;
out vec2 TexCoord;
out vec3 LightDir;
out vec3 ViewDir;

void main() {
    vec4 worldPos = world * vec4(vertex, 1.0);
    FragPos = worldPos.xyz;
    Normal = mat3(transpose(inverse(world))) * normal;
    Color = color;
    TexCoord = uv;

    LightDir = normalize(lightPos - FragPos);
    ViewDir = normalize(cameraPos - FragPos);

    gl_Position = worldViewProj * vec4(vertex, 1.0);
}
