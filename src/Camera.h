#pragma once

#include "Maths.h"

struct Camera {
    Matrix4 projectionMatrix;

    Vector3 position;
    Vector3 rotation;

    Vector3 up;
    Vector3 front;
};

struct Camera createCamera();
void cameraKeyboardInput(struct Camera* camera, const uint8_t* keyboard);
void cameraMouseInput(struct Camera* camera, uint32_t xrel, uint32_t yrel);
void cameraUpdate(struct Camera* camera, Matrix4 projectionViewMatrix);
