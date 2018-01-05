#pragma once

#include <conflagrant/types.hh>
#include <conflagrant/assets/Asset.hh>
#include <conflagrant/assets/Texture.hh>
#include <conflagrant/gl/Shader.hh>
#include <conflagrant/RenderStats.hh>

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

    void Upload(gl::Shader &shader,
                string const &prefix,
                GLenum &textureCount,
                RenderStats &renderStats) const;

    void Upload(gl::Shader &shader,
                string const &prefix,
                GLenum &textureCount) const;
};

inline void Material::Upload(gl::Shader &shader,
                             string const &prefix,
                             GLenum &textureCount,
                             RenderStats &renderStats) const {
    {
        string const diffusePrefix = prefix + "diffuse.";
        int hasDiffuseMap = (diffuseTexture != nullptr) ? 1 : 0;

        shader.Uniform(diffusePrefix + "color", diffuseColor);
        shader.Uniform(diffusePrefix + "hasMap", hasDiffuseMap);
        renderStats.UniformCalls += 3;

        if (hasDiffuseMap == 1) {
            shader.Texture(diffusePrefix + "map",
                           textureCount++,
                           diffuseTexture->texture);
            renderStats.UniformCalls++;
        }
    }

    {
        string const specularPrefix = prefix + "specular.";
        int hasSpecularMap = (specularTexture != nullptr) ? 1 : 0;

        shader.Uniform(specularPrefix + "color", specularColor);
        shader.Uniform(specularPrefix + "hasMap", hasSpecularMap);
        renderStats.UniformCalls += 2;

        if (hasSpecularMap == 1) {
            shader.Texture(specularPrefix + "map",
                           textureCount++,
                           specularTexture->texture);
            renderStats.UniformCalls++;
        }
    }

    {
        int hasNormalMap = (normalTexture != nullptr) ? 1 : 0;
        shader.Uniform(prefix + "hasNormalMap", hasNormalMap);
        renderStats.UniformCalls++;

        if (hasNormalMap == 1) {
            shader.Texture(prefix + "normalMap",
                           textureCount++,
                           normalTexture->texture);
            renderStats.UniformCalls++;
        }
    }

    {
        shader.Uniform(prefix + "shininess", shininess);
        renderStats.UniformCalls++;
    }
}

inline void Material::Upload(gl::Shader &shader, string const &prefix, GLenum &textureCount) const {
    RenderStats dummy;
    Upload(shader, prefix, textureCount, dummy);
}
} // namespace assets
} // namespace cfl
