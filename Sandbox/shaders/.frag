#version 450

layout (location = 0) out vec4 fragColor;
layout (location = 0) in vec3 aColor;

void main()
{
    fragColor = vec4(aColor, 1.0);
}