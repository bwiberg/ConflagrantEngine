#include "ForwardRenderer.hh"

#include <conflagrant/Time.hh>
#include <conflagrant/components/Model.hh>
#include <conflagrant/components/Transform.hh>

#include <imgui.h>
#include <fstream>
#include <conflagrant/components/PointLight.hh>

namespace cfl {
namespace syst {
ForwardRenderer::ForwardRenderer() {
    Path forwardVertexPath("forward.vert"), forwardFragmentPath("forward.frag");
    {
        PathResolver resolver;
        resolver.append(Path(BUILTIN_SHADER_DIR));

        forwardVertexPath = resolver.resolve(forwardVertexPath);
        forwardFragmentPath = resolver.resolve(forwardFragmentPath);
    }

    if (!forwardVertexPath.is_file() || !forwardFragmentPath.is_file()) {
        LOG_ERROR(cfl::syst::ForwardRenderer)
                << "Fatal error, couldn't locate built-in shader sources for 'forward' shader" << std::endl;
        return;
    }

    std::stringstream buffer;

    buffer << std::ifstream(forwardVertexPath.str()).rdbuf();
    string const forwardShaderVertex = buffer.str();
    buffer.str("");

    buffer << std::ifstream(forwardFragmentPath.str()).rdbuf();
    string const forwardShaderFragment = buffer.str();
    buffer.str("");

    forwardShader = std::make_shared<gl::Shader>(forwardShaderVertex, forwardShaderFragment);
}

void ForwardRenderer::update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) {
    uvec2 size = window->GetSize();
    OGL(glViewport(0, 0, size.x, size.y));
    OGL(glClear(GL_COLOR_BUFFER_BIT));
    OGL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));

    mat4 tmp;
    mat4 const &P = tmp; // todo get active camera's projection
    mat4 const &V = tmp; // todo get active camera's view

    using entityx::ComponentHandle;
    ComponentHandle<comp::Transform> transform;
    ComponentHandle<comp::Model> model;
    ComponentHandle<comp::PointLight> pointLight;

    forwardShader->Bind();

    // upload camera parameters
    forwardShader->Uniform("P", P);
    forwardShader->Uniform("V", V);

    // upload light data
    int ilight = 0;
    std::stringstream ss;
    for (auto const &entity : entities.entities_with_components(transform, pointLight)) {
        ss << "pointLights" << "[" << ilight << "]" << ".";
        string const prefix = ss.str();

        forwardShader->Uniform(prefix + "position", transform->position);
        forwardShader->Uniform(prefix + "intensity", pointLight->intensity);
        forwardShader->Uniform(prefix + "color", pointLight->color);

        ilight++;
        ss.clear();
    }

    forwardShader->Uniform("numPointLights", ilight);

    for (auto const &entity : entities.entities_with_components(transform, model)) {
        forwardShader->Uniform("M", transform->matrix);

        for (auto const &part : model->value->parts) {
            string const prefix = "material.";

            // upload material
            {
                auto const &material = *part.second;

                {
                    string const diffusePrefix = prefix + "diffuse.";
                    forwardShader->Uniform(diffusePrefix + "color", material.diffuseColor);

                    int hasDiffuseMap = (material.diffuseTexture != nullptr) ? 1 : 0;
                    forwardShader->Uniform(diffusePrefix + "hasMap", hasDiffuseMap);
                    if (hasDiffuseMap == 1) {
                        forwardShader->Texture(diffusePrefix + "map", GL_TEXTURE0, material.diffuseTexture->texture);
                    }
                }

                {
                    string const specularPrefix = prefix + "specular.";
                    forwardShader->Uniform(specularPrefix + "color", material.specularColor);

                    int hasSpecularMap = (material.specularTexture != nullptr) ? 1 : 0;
                    forwardShader->Uniform(specularPrefix + "hasMap", hasSpecularMap);
                    if (hasSpecularMap == 1) {
                        forwardShader->Texture(specularPrefix + "map", GL_TEXTURE1, material.specularTexture->texture);
                    }
                }

                // todo implement normal mapping
                int hasNormalMap = 0;
                forwardShader->Uniform(prefix + "hasNormalMap", hasNormalMap);
                if (hasNormalMap == 1) {
                    // forwardShader->Texture(prefix + "normalMap", GL_TEXTURE2, material.ambientTexture->texture);
                    assert(false && "Unreachable code!");
                }

                forwardShader->Uniform(prefix + "shininess", material.shininess);
            }

            // render mesh
            {
                auto const &mesh = *part.first;
                mesh.glMesh->DrawElements();
            }

        }
    }

    forwardShader->Unbind();
}
} // namespace syst
} // namespace cfl