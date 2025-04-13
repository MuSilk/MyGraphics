#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void Texture2D::Generate(const char* file){
    int height, width, channels;
    stbi_uc* data = stbi_load(file, &width, &height, &channels, STBI_rgb_alpha);
    if (data == nullptr) {
        std::cout << "file not find\n";
    }
    Generate(width, height, data);
    stbi_image_free(data);
}