/**
 * Created by bwiberg on 9/11/17.
 */

#include "conflagrant.hh"
#include <conflagrant/assets/loaders/TextureLoader.hh>
#include <conflagrant/assets/loaders/ModelLoader.hh>
#include <conflagrant/SystemFactory.hh>
#include <conflagrant/ComponentFactory.hh>

#include <conflagrant/components/Name.hh>
#include <conflagrant/components/Guid.hh>
#include <conflagrant/components/Mesh.hh>
#include <conflagrant/components/Model.hh>
#include <conflagrant/components/Transform.hh>

#include <conflagrant/systems/CameraController.hh>
#include <conflagrant/systems/TransformUpdater.hh>
#include <conflagrant/systems/ForwardRenderer.hh>

namespace cfl {
bool InitDefaults() {
    cfl::assets::AssetManager::RegisterLoaderForExtensions(
            cfl::assets::LoadModel,
            {"obj", "fbx"});
    cfl::assets::AssetManager::RegisterLoaderForExtensions(
            cfl::assets::LoadTexture,
            {"jpg", "jpeg", "png", "tga", "bmp", "psd", "gif", "hdr", "pic"});

    REGISTER_COMPONENT(cfl::comp::Guid);
    REGISTER_COMPONENT(cfl::comp::Mesh);
    REGISTER_COMPONENT(cfl::comp::Model);
    REGISTER_COMPONENT(cfl::comp::Name);
    REGISTER_COMPONENT(cfl::comp::Transform);

    REGISTER_SYSTEM(cfl::syst::CameraController);
    REGISTER_SYSTEM(cfl::syst::TransformUpdater);
    REGISTER_SYSTEM(cfl::syst::ForwardRenderer);

    return true;
}
}
