#include "Camera.h"

#include "Utility.h"
#include <stdio.h>

struct Camera createCamera()
{
    float aspect = (float)WIDTH / (float)HEIGHT;

    struct Camera camera = {0};
    glm_perspective(glm_rad(90.0f), aspect, 0.1f, 100.0f, camera.projectionMatrix);

    camera.rotation[1] = -90.0f;
    camera.up[1] = 1.0f;

    return camera;
}

void cameraKeyboardInput(struct Camera* camera)
{
    const uint8_t* keyboard = SDL_GetKeyboardState(NULL);
    // Keyboard input
    // https://wiki.libsdl.org/SDL_Scancode
    if (keyboard[SDL_SCANCODE_W]) {
        moveVectorForwards(camera->position, camera->rotation, 1);
    }
    if (keyboard[SDL_SCANCODE_A]) {
        moveVectorLeft(camera->position, camera->rotation, 1);
    }
    if (keyboard[SDL_SCANCODE_S]) {
        moveVectorBackwards(camera->position, camera->rotation, 1);
    }
    if (keyboard[SDL_SCANCODE_D]) {
        moveVectorRight(camera->position, camera->rotation, 1);
    }
}

void cameraMouseInput(struct Camera* camera, uint32_t xrel, uint32_t yrel)
{
    int mouseXDiff = xrel;
    int mouseYDiff = yrel;
    camera->rotation[0] -= mouseYDiff / 4.0f;
    camera->rotation[1] += mouseXDiff / 4.0f;
    camera->rotation[0] = glm_clamp(camera->rotation[0], -89.9f, 89.9f);
}

void cameraUpdate(struct Camera* camera, Matrix4 projectionViewMatrix)
{
    // View Matrix
    Vector3 center;

    camera->front[0] = cos(glm_rad(camera->rotation[1])) * cos(glm_rad(camera->rotation[0]));
    camera->front[1] = sin(glm_rad(camera->rotation[0]));
    camera->front[2] = sin(glm_rad(camera->rotation[1])) * cos(glm_rad(camera->rotation[0]));
    glm_normalize(camera->front);
    glm_vec3_add(camera->position, camera->front, center);

    Matrix4 viewMatrix = MATRIX4_IDENTITY;
    glm_lookat(camera->position, center, camera->up, viewMatrix);

    // Calculate projection view matrix and then upload
    glm_mat4_mul(camera->projectionMatrix, viewMatrix, projectionViewMatrix);
}
