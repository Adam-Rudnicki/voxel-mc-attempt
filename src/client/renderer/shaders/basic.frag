#version 460 core

in vec3 vWorldPosition;
in vec2 vUV;
in vec3 vNormal;

uniform sampler2D uTexture;
uniform vec3 uLightDirection;
uniform vec3 uCameraPosition;
uniform vec3 uFogColor;
uniform float uFogStart;
uniform float uFogEnd;

out vec4 FragColor;

void main()
{
    vec4 tex = texture(uTexture, vUV);
    if (tex.a < 0.05) discard;

    // lambert diffuse shading
    float diffuse = max(dot(normalize(vNormal), normalize(uLightDirection)), 0.2);
    vec3 base = tex.rgb * diffuse;

    // fog effect linear
    float distance = length(vWorldPosition - uCameraPosition);
    float fog = clamp((uFogEnd - distance) / (uFogEnd - uFogStart), 0.0, 1.0);
    vec3 color = mix(uFogColor, base, fog);

    FragColor = vec4(color, 1.0);
}
