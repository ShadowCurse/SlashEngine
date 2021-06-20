#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform Camera
{
    mat4 view;
    mat4 proj;
} camera;

layout(binding = 1) uniform Transform
{
    mat4 translation;
    mat4 rotation;
    mat4 scale;
} transform;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    mat4 model = transform.translation * transform.rotation * transform.scale;
    gl_Position = camera.proj * camera.view * model * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}