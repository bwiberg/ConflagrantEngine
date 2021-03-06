/**
 * Created by bwiberg on 9/11/17.
 */

#include "conflagrant.hh"
#include <conflagrant/assets/loaders/TextureLoader.hh>
#include <conflagrant/assets/loaders/ModelLoader.hh>
#include <conflagrant/ComponentFactory.hh>

#include <conflagrant/components/Name.hh>
#include <conflagrant/components/Guid.hh>
#include <conflagrant/components/Mesh.hh>
#include <conflagrant/components/Model.hh>
#include <conflagrant/components/Transform.hh>
#include <conflagrant/components/ActiveCamera.hh>
#include <conflagrant/components/PerspectiveCamera.hh>
#include <conflagrant/components/OrthographicCamera.hh>
#include <conflagrant/components/PointLight.hh>
#include <conflagrant/components/DirectionalLight.hh>
#include <conflagrant/components/DirectionalLightShadow.hh>
#include <conflagrant/components/DirectionalLightAnimation.hh>
#include <conflagrant/components/Skydome.hh>
#include <conflagrant/components/BoundingSphere.hh>
#include <conflagrant/components/VelocityAnimation.hh>
#include <conflagrant/components/PeriodicalAnimation.hh>
#include <conflagrant/components/VctProperties.hh>
#include <conflagrant/components/SnowEmitter.hh>

#include <conflagrant/systems/Animator.hh>
#include <conflagrant/systems/CameraController.hh>
#include <conflagrant/systems/DeferredRenderer.hh>
#include <conflagrant/systems/EcsDebugger.hh>
#include <conflagrant/systems/ForwardRenderer.hh>
#include <conflagrant/systems/SnowfallAnimator.hh>

#include <conflagrant/ShaderSourceManager.hh>

namespace cfl {
bool InitDefaults() {
    using cfl::assets::AssetManager;

    AssetManager::RegisterLoaderForExtensions(
            cfl::assets::LoadModel,
            {"obj", "fbx"});
    AssetManager::RegisterLoaderForExtensions(
            cfl::assets::LoadTexture,
            {"jpg", "jpeg", "png", "tga", "bmp", "psd", "gif", "hdr", "pic"});

    Path builtinAssetsPath(BUILTIN_ASSETS_DIR);
    AssetManager::AddAssetsPath(builtinAssetsPath);

    REGISTER_COMPONENT(cfl::comp::Guid);
    REGISTER_COMPONENT(cfl::comp::Mesh);
    REGISTER_COMPONENT(cfl::comp::Model);
    REGISTER_COMPONENT(cfl::comp::Name);
    REGISTER_COMPONENT(cfl::comp::Transform);
    REGISTER_COMPONENT(cfl::comp::ActiveCamera);
    REGISTER_COMPONENT(cfl::comp::PerspectiveCamera);
    REGISTER_COMPONENT(cfl::comp::OrthographicCamera);
    REGISTER_COMPONENT(cfl::comp::PointLight);
    REGISTER_COMPONENT(cfl::comp::DirectionalLight);
    REGISTER_COMPONENT(cfl::comp::DirectionalLightShadow);
    REGISTER_COMPONENT(cfl::comp::DirectionalLightAnimation);
    REGISTER_COMPONENT(cfl::comp::Skydome);
    REGISTER_COMPONENT(cfl::comp::BoundingSphere);
    REGISTER_COMPONENT(cfl::comp::VelocityAnimation);
    REGISTER_COMPONENT(cfl::comp::PeriodicalAnimation);
    REGISTER_COMPONENT(cfl::comp::SnowEmitter);

#ifdef ENABLE_VOXEL_CONE_TRACING
    REGISTER_COMPONENT(cfl::comp::VctProperties);
#endif // ENABLE_VOXEL_CONE_TRACING

    REGISTER_SYSTEM(cfl::syst::Animator);
    REGISTER_SYSTEM(cfl::syst::CameraController);
    REGISTER_SYSTEM(cfl::syst::DeferredRenderer);
    REGISTER_SYSTEM(cfl::syst::EcsDebugger);
    REGISTER_SYSTEM(cfl::syst::ForwardRenderer);
    REGISTER_SYSTEM(cfl::syst::SnowfallAnimator);

    REGISTER_SHADER_FOLDER(BUILTIN_SHADER_DIR);

    return true;
}
}
