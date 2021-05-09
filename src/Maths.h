#pragma once

#include <cglm/struct.h>

typedef mat4s Matrix4;
typedef vec3s Vector3;

#define VECTOR3_ZERO        GLMS_VEC3_ZERO_INIT
#define MATRIX4_IDENTITY    GLMS_MAT4_IDENTITY_INIT

Vector3 initVector3(float x, float y, float z);

Matrix4 createModelMatrix(const Vector3* position, const Vector3* rotation);
