#include "Maths.h"

void createModelMatrix(Vector3 position, Vector3 rotation, Matrix4 dest)
{
    glm_translate(dest, position);

    glm_rotate_x(dest, glm_rad(rotation[0]), dest);
    glm_rotate_y(dest, glm_rad(rotation[1]), dest);
    glm_rotate_z(dest, glm_rad(rotation[2]), dest);
}
