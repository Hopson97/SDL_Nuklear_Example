#include "Texture.h"

#include <stb/stb_image.h>
#include <string.h>

GLuint loadTexture(const char* file)
{
    char texturePath[128] = "Data/Textures/";
    strcat(texturePath, file);
    printf("%s", texturePath);

    int width;
    int height;
    int channels;
    unsigned char* imageData = stbi_load(texturePath, &width, &height, &channels, STBI_rgb_alpha);
    printf("%d", width);
    GLuint texture;
    glCreateTextures(GL_TEXTURE_2D, 1, &texture);
    glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureStorage2D(texture, 1, GL_RGBA8, width, height);
    glTextureSubImage2D(texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    stbi_image_free(imageData);

    return texture;
}
