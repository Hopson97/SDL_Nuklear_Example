/*#version 330

layout (location = 0) in vec3 inVertexCoord;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec3 inNormal;

uniform mat4 modelMatrix;
uniform mat4 projectionViewMatrix;

out vec2 passTexCoord;
out vec3 passFragPosition;
out vec3 passNormal;

void main() {
    vec4 worldPos = modelMatrix * vec4(inVertexCoord, 1.0);
    gl_Position = projectionViewMatrix * worldPos;

    passTexCoord = inTexCoord;
    passFragPosition = vec3(modelMatrix * vec4(inVertexCoord, 1.0));
    passNormal = mat3(modelMatrix) * inNormal;
}*/

#version 450 core

layout(location = 0) in vec3 inPosition;

void main()
{
    gl_Position = vec4(inPosition, 1.0);
}