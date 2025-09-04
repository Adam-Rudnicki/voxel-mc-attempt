#version 460 core
layout(location = 0) in uvec3 aPosition;
layout(location = 1) in uint aPackedNormal;
layout(location = 2) in vec2 aUV;

uniform ivec3 uChunkOrigin;
uniform mat4 uMVP;

out vec3 vWorldPosition;
out vec2 vUV;
out vec3 vNormal;

void main()
{
    vWorldPosition = vec3(aPosition) + vec3(uChunkOrigin);
    gl_Position = uMVP * vec4(vWorldPosition, 1.0);

    vUV = aUV;
    uint bits = aPackedNormal;
    vNormal = vec3(
    (bits & 1u) - (bits & 2u),
    (bits & 4u) - (bits & 8u),
    (bits & 16u) - (bits & 32u));
}
