//
// Created by bwiberg on 11/6/17.
//

#include "ModelLoader.hh"
#include "TextureLoader.hh"
#include "AssimpGlmConvert.hh"
#include <conflagrant/assets/AssetManager.hh>
#include <conflagrant/Model.hh>
#include <conflagrant/Material.hh>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace cfl {
namespace assets {
unsigned int const AssimpFlags = aiProcessPreset_TargetRealtime_MaxQuality;

void ProcessNode(aiNode const *node, aiScene const *scene, Model &model, AssetManager &manager);

std::shared_ptr<Mesh> LoadMesh(aiMesh const *mesh, aiScene const *scene,
                               Path const &path,
                               AssetManager &manager);

std::shared_ptr<Material> LoadMaterial(aiMaterial const *material, aiScene const *scene,
                                       Path const &path,
                                       AssetManager &manager);

void ProcessNode(aiNode const *node, aiScene const *scene,
                 std::vector<std::shared_ptr<Material>> const &materials,
                 Model &model,
                 Path const &path,
                 AssetManager &manager) {
    for (uint i = 0; i < node->mNumMeshes; i++) {
        aiMesh const *aimesh = scene->mMeshes[node->mMeshes[i]];

        auto mesh = LoadMesh(aimesh, scene, path, manager);
        auto &material = materials[aimesh->mMaterialIndex];

        model.meshes.emplace_back(mesh, material);
    }

    // After we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (GLuint i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene, model, manager);
    }
}

std::shared_ptr<Mesh> LoadMesh(aiMesh const *mesh, aiScene const *scene,
                               Path const &path,
                               AssetManager &manager) {
    std::vector<std::shared_ptr<Texture2D>> textures;

    bool const hasTangents = mesh->HasTangentsAndBitangents();
    bool const hasTexCoords = mesh->HasTextureCoords(0);

    std::vector<Vertex> vertices(mesh->mNumVertices);
    for (uint i = 0; i < mesh->mNumVertices; i++) {
        Vertex &vertex = vertices[i];

        Apply(vertex.position, mesh->mVertices[i]);
        Apply(vertex.normal, mesh->mNormals[i]);

        if (hasTangents) {
            Apply(vertex.tangent, mesh->mTangents[i]);
            Apply(vertex.bitangent, mesh->mBitangents[i]);
        } else {
            vertex.tangent = glm::vec3(0.0f, 1.0f, 0.0f);
            vertex.bitangent = glm::cross(vertex.normal, vertex.tangent);
        }

        if (hasTexCoords) {
            vertex.texCoord = glm::make_vec2(&mesh->mTextureCoords[0][i].x);
        }
    }

    std::vector<uvec3> triangles(mesh->mNumFaces);
    for (uint i = 0; i < mesh->mNumFaces; i++) {
        aiFace const &face = mesh->mFaces[i];

        if (face.mNumIndices != 3) {
            std::cerr << "cfl::assets::LoadMesh: " << "ERROR" << std::endl;
            return nullptr;
        }

        uvec3 &triangle = triangles[i];
        for (uint j = 0; j < face.mNumIndices; j++)
            triangle[j] = face.mIndices[j];
    }

    return std::make_shared<Mesh>(std::move(vertices), std::move(triangles));
}

bool TryLoadMaterialProperty(std::shared_ptr<assets::Texture2D> &textureTarget, vec3 &colorTarget,
                             AssetManager &manager,
                             aiMaterial const *aimtl, aiTextureType const type,
                             char const *propKey, uint const ttype, uint const idx,
                             std::string const typeName) {
#define RETURN_ERROR(x, y) std::cerr << "cfl::assets::TryLoadMaterialProperty(" << typeName << "): " << (x) << std::endl; \
    return false;

    aiString str;
    aiReturn ret;
    aiColor3D color;

    uint const numDiffuseTextures = aimtl->GetTextureCount(type);
    if (numDiffuseTextures > 1) {
        RETURN_ERROR("aiMaterial should have 0 or 1 diffuse textures, has ", numDiffuseTextures);
    }

    if (numDiffuseTextures == 1) {
        ret = aimtl->GetTexture(type, 0, &str);
        if (ret != aiReturn_SUCCESS) {
            RETURN_ERROR("aiMaterial->GetTexture failed with return value ", ret);
        };

        textureTarget = std::dynamic_pointer_cast<Texture2D>(LoadTexture(filesystem::path(str.C_Str()), manager));
        return true;
    }

    ret = aimtl->Get(propKey, ttype, idx, color);
    if (ret != aiReturn_SUCCESS) {
        RETURN_ERROR("aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE) failed with return value ", ret);
    };

    Apply(colorTarget, color);
    return true;
}

std::shared_ptr<Material> LoadMaterial(aiMaterial const *material, aiScene const *scene,
                                       Path const &path,
                                       AssetManager &manager) {
#undef RETURN_ERROR
#define RETURN_ERROR(x, y) std::cerr << "cfl::assets::LoadMaterial: " << (x) << std::endl; \
    return nullptr;
    auto mtl = std::make_shared<Material>();

    bool succeeded = false;
    succeeded |= TryLoadMaterialProperty(mtl->diffuseTexture, mtl->diffuseColor, manager,
                                         material, aiTextureType_DIFFUSE, AI_MATKEY_COLOR_DIFFUSE, "diffuse");
    succeeded |= TryLoadMaterialProperty(mtl->specularTexture, mtl->specularColor, manager,
                                         material, aiTextureType_SPECULAR, AI_MATKEY_COLOR_SPECULAR, "specular");
    succeeded |= TryLoadMaterialProperty(mtl->ambientTexture, mtl->ambientColor, manager,
                                         material, aiTextureType_AMBIENT, AI_MATKEY_COLOR_AMBIENT, "ambient");
    if (!succeeded) {
        return nullptr;
    }

    aiReturn ret = material->Get(AI_MATKEY_SHININESS, mtl->shininess);
    if (ret != aiReturn_SUCCESS) {
        RETURN_ERROR("aiMaterial->Get(ambient) failed with return value ", ret);
    }

    return mtl;
}

std::shared_ptr<Model> LoadModel(Path const &path, AssetManager &manager) {
    Assimp::Importer importer;

    aiScene const *scene = importer.ReadFile(path.str().c_str(), AssimpFlags);
    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "cfl::assets::LoadModel" << importer.GetErrorString() << std::endl;
        return nullptr;
    }

    std::vector<std::shared_ptr<Material>> materials(scene->mNumMaterials);
    for (uint i = 0; i < scene->mNumMaterials; ++i) {
        aiMaterial const *aimaterial = scene->mMaterials[i];
        materials[i] = LoadMaterial(aimaterial, scene, path, manager);
    }

    auto model = std::make_shared<Model>();
    ProcessNode(scene->mRootNode, scene,
                materials, *model,
                path, manager);

    return model;
}
} // namespace cfl
} // namespace assets
