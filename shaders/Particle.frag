// Licensed under the Apache License, Version 2.0
// Particle Fragment Shader for Bas Veeg Arc 3D

#version 330 core

in vec4 ParticleColor;
in vec2 TexCoord;

out vec4 FragColor;

uniform float time;

void main() {
    // Circular particle shape with soft edges
    vec2 center = vec2(0.5);
    float dist = distance(TexCoord, center);
    float alpha = 1.0 - smoothstep(0.3, 0.5, dist);

    // Add animated glow
    float glow = sin(time * 10.0) * 0.3 + 0.7;

    vec3 color = ParticleColor.rgb * glow;
    FragColor = vec4(color, ParticleColor.a * alpha);
}
