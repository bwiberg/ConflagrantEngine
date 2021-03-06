#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/assets/Asset.hh>
#include <conflagrant/assets/Texture.hh>

namespace cfl {
namespace assets {
struct Material : public Asset {
    /**
     * The material's diffuse texture. If nullptr, uses a constant #diffuseColor instead.
     */
    std::shared_ptr<assets::Texture2D> diffuseTexture{nullptr};
    vec3 diffuseColor{0};

    /**
     * The material's specular texture. If nullptr, uses a constant #specularColor instead.
     */
    std::shared_ptr<assets::Texture2D> specularTexture{nullptr};
    vec3 specularColor{0};

    /**
     * The material's ambient texture. If nullptr, uses a constant #ambientColor instead.
     */
    std::shared_ptr<assets::Texture2D> ambientTexture{nullptr};
    vec3 ambientColor{0};

    /**
     * The material's shininess.
     */
    float shininess{1.0f};

    /**
     * The material's normal map.
     */
    std::shared_ptr<assets::Texture2D> normalTexture{nullptr};
};
} // namespace assets
} // namespace cfl
