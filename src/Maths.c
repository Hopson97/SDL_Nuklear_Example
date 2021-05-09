#include "Maths.h"

Vector3 initVector3(float x, float y, float z)
{
    Vector3 vec;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    return vec;
}

Matrix4 createModelMatrix(const Vector3* position, const Vector3* rotation)
{
    Matrix4 modelMatrix = MATRIX4_IDENTITY;
    modelMatrix = glms_translate(modelMatrix, *position);

    // glm_rotate_x(modelMatrix, 0.0f, modelMatrix);
    // glm_rotate_y(modelMatrix, 0.0f, modelMatrix);
    // glm_rotate_z(modelMatrix, 0.0f, modelMatrix);

    return modelMatrix;
}
