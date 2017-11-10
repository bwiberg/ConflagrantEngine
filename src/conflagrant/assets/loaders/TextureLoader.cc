#include "TextureLoader.hh"
#include <conflagrant/assets/Texture.hh>

#include <stb/stb_image.h>

namespace cfl {
namespace assets {
std::shared_ptr<Asset> LoadTexture(Path const &path, AssetManager &manager) {
#define RETURN_ERROR(x) std::cerr << "cfl::assets::LoadTexture(" << path << "): " << (x) << std::endl; \
    return nullptr;

    GLenum format = 3;

    // load texture data
    int width, height, channels;
    uchar *image = stbi_load(path.str().c_str(), &width, &height, &channels, 0);
    if (!image) {
        RETURN_ERROR("stb_image failed to load image.");
    }

    // channel format
    switch (channels) {
        case 1 :
            format = GL_ALPHA;
            break;
        case 2 :
            format = GL_LUMINANCE;
            break;
        case 3 :
            format = GL_RGB;
            break;
        case 4 :
            format = GL_RGBA;
            break;
        default:
            break;
    }

    if (width == 0) {
        RETURN_ERROR("width is 0 for 1-channel image.");
    }

    if (height == 0) {
        gl::Texture1D texture(width, format, format, GL_UNSIGNED_BYTE, image);
        stbi_image_free(image);
        return std::make_shared<Texture1D>(std::move(texture));
    }

    gl::Texture2D texture(width, height, format, format, GL_UNSIGNED_BYTE, image);
    stbi_image_free(image);
    return std::make_shared<Texture2D>(std::move(texture));
}
} // namespace assets
} // namespace cfl
