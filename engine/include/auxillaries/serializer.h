#pragma once

#include "ECS.h"
#include "common/YAML.h"
#include <magic_enum/magic_enum.hpp>

namespace flow {
    struct DataSerializer {

        // ── String serialize (for runtime snapshot) ──

        FLOW_INLINE std::string Serialize(entityRegistry& scene) {
            YAML::Emitter emitter;

            emitter << YAML::BeginMap;
            {
                emitter << YAML::Key << "ENTITIES" << YAML::Value << YAML::BeginSeq;
                {
                    for (auto entt : scene.storage<entt::entity>()) {
                        Entity entity(&scene, entt);

                        emitter << YAML::BeginMap;
                        {
                            if (entity.template Has<infoComponent>()) {
                                auto& info = entity.template Get<infoComponent>();
                                emitter << YAML::Key << "InfoComponent" << YAML::BeginMap;
                                {
                                    emitter << YAML::Key << "UID"    << YAML::Value << info.UID;
                                    emitter << YAML::Key << "Name"   << YAML::Value << info.Name;
                                    emitter << YAML::Key << "Parent" << YAML::Value << info.Parent;
                                }
                                emitter << YAML::EndMap;
                            }

                            if (entity.template Has<cameraComponent>()) {
                                auto& camera = entity.template Get<cameraComponent>().Camera;
                                emitter << YAML::Key << "CameraComponent" << YAML::BeginMap;
                                {
                                    emitter << YAML::Key << "NearPlane" << YAML::Value << camera.nearPlane;
                                    emitter << YAML::Key << "FarPlane"  << YAML::Value << camera.farPlane;
                                    emitter << YAML::Key << "FOV"       << YAML::Value << camera.FOV;
                                }
                                emitter << YAML::EndMap;
                            }

                            if (entity.template Has<transformComponent>()) {
                                auto& transform = entity.template Get<transformComponent>().Transform;
                                emitter << YAML::Key << "TransformComponent" << YAML::BeginMap;
                                {
                                    emitter << YAML::Key << "Translate" << YAML::Value << transform.Translate;
                                    emitter << YAML::Key << "Rotation"  << YAML::Value << transform.Rotate;
                                    emitter << YAML::Key << "Scale"     << YAML::Value << transform.Scale;
                                }
                                emitter << YAML::EndMap;
                            }

                            if (entity.template Has<rigidBodyComponent>()) {
                                auto& body = entity.template Get<rigidBodyComponent>().RigidBody;
                                emitter << YAML::Key << "RigidBodyComponent" << YAML::BeginMap;
                                {
                                    emitter << YAML::Key << "Density" << YAML::Value << body.Density;
                                    emitter << YAML::Key << "Dynamic" << YAML::Value << body.Dynamic;
                                }
                                emitter << YAML::EndMap;
                            }

                            if (entity.template Has<colliderComponent>()) {
                                auto& collider = entity.template Get<colliderComponent>().Collider;
                                std::string type(magic_enum::enum_name(collider.Type));

                                emitter << YAML::Key << "ColliderComponent" << YAML::BeginMap;
                                {
                                    emitter << YAML::Key << "DynamicFriction" << YAML::Value << collider.DynamicFriction;
                                    emitter << YAML::Key << "StaticFriction"  << YAML::Value << collider.StaticFriction;
                                    emitter << YAML::Key << "Restitution"     << YAML::Value << collider.Restitution;
                                    emitter << YAML::Key << "Type"            << YAML::Value << type;
                                }
                                emitter << YAML::EndMap;
                            }

                            if (entity.template Has<charControllerComponent>()) {
                                auto& control = entity.template Get<charControllerComponent>();
                                emitter << YAML::Key << "CharControllerComponent" << YAML::BeginMap;
                                {
                                    emitter << YAML::Key << "Radius"     << YAML::Value << control.Controller.Radius;
                                    emitter << YAML::Key << "Height"     << YAML::Value << control.Controller.Height;
                                    emitter << YAML::Key << "StepOffset" << YAML::Value << control.Controller.StepOffset;
                                    emitter << YAML::Key << "MoveSpeed"  << YAML::Value << control.Controller.MoveSpeed;
                                    emitter << YAML::Key << "EyeOffset"  << YAML::Value << control.Controller.EyeOffset;
                                    emitter << YAML::Key << "SpawnPosition" << YAML::Value << control.Controller.SpawnPosition;
                                    emitter << YAML::Key << "SpawnYaw" << YAML::Value << control.Controller.SpawnYaw;
                                }
                                emitter << YAML::EndMap;
                            }

                            if (entity.template Has<directLightComponent>()) {
                                auto& light = entity.template Get<directLightComponent>().Light;
                                emitter << YAML::Key << "DirectLightComponent" << YAML::BeginMap;
                                {
                                    emitter << YAML::Key << "Intensity" << YAML::Value << light.Intensity;
                                    emitter << YAML::Key << "Radiance"  << YAML::Value << light.Radiance;
                                }
                                emitter << YAML::EndMap;
                            }

                            if (entity.template Has<pointLightComponent>()) {
                                auto& light = entity.template Get<pointLightComponent>().Light;
                                emitter << YAML::Key << "PointLightComponent" << YAML::BeginMap;
                                {
                                    emitter << YAML::Key << "Intensity" << YAML::Value << light.Intensity;
                                    emitter << YAML::Key << "Radiance"  << YAML::Value << light.Radiance;
                                }
                                emitter << YAML::EndMap;
                            }

                            if (entity.template Has<spotLightComponent>()) {
                                auto& light = entity.template Get<spotLightComponent>().Light;
                                emitter << YAML::Key << "SpotLightComponent" << YAML::BeginMap;
                                {
                                    emitter << YAML::Key << "Intensity" << YAML::Value << light.Intensity;
                                    emitter << YAML::Key << "Radiance"  << YAML::Value << light.Radiance;
                                    emitter << YAML::Key << "Falloff"   << YAML::Value << light.Falloff;
                                    emitter << YAML::Key << "Cutoff"    << YAML::Value << light.Cutoff;
                                }
                                emitter << YAML::EndMap;
                            }

                            if (entity.template Has<scriptComponent>()) {
                                emitter << YAML::Key << "ScriptComponent" << YAML::BeginMap;
                                {
                                    auto& asset = entity.template Get<scriptComponent>().Script;
                                    emitter << YAML::Key << "Script" << YAML::Value << asset;
                                }
                                emitter << YAML::EndMap;
                            }

                            if (entity.template Has<skyboxComponent>()) {
                                emitter << YAML::Key << "SkyboxComponent" << YAML::BeginMap;
                                {
                                    auto& asset = entity.template Get<skyboxComponent>().Sky;
                                    emitter << YAML::Key << "Skybox" << YAML::Value << asset;
                                }
                                emitter << YAML::EndMap;
                            }

                            if (entity.template Has<modelComponent>()) {
                                auto& comp = entity.template Get<modelComponent>();
                                emitter << YAML::Key << "ModelComponent" << YAML::BeginMap;
                                {
                                    emitter << YAML::Key << "Material" << YAML::Value << comp.Material;
                                    emitter << YAML::Key << "Model"    << YAML::Value << comp.Model;
                                }
                                emitter << YAML::EndMap;
                            }
                        }
                        emitter << YAML::EndMap;
                    }
                }
                emitter << YAML::EndSeq;
            }
            emitter << YAML::EndMap;

            return emitter.c_str();
        }

        // ── File serialize (delegates to string version) ──

        FLOW_INLINE void Serialize(entityRegistry& scene, const std::string& path) {
            std::ofstream filepath(path);
            if (filepath.is_open()) {
                filepath << Serialize(scene);
            }
        }

        // ── String deserialize (for runtime restore) ──

        FLOW_INLINE void DeserializeFromString(entityRegistry& scene, const std::string& data) {
            scene.clear();
            try {
                const auto root = YAML::Load(data);
                const auto nodes = root["ENTITIES"];

                for (const auto& node : nodes) {
                    entityID entity = scene.create();
                    try {
                        if (auto d = node["InfoComponent"]) {
                            auto& info = scene.emplace<infoComponent>(entity);
                            info.Parent = d["Parent"].as<AssetID>(info.Parent);
                            info.Name = d["Name"].as<std::string>(info.Name);
                            info.UID = d["UID"].as<AssetID>(info.UID);
                        }

                        if (auto d = node["CameraComponent"]) {
                            auto& camera = scene.emplace<cameraComponent>(entity).Camera;
                            camera.nearPlane = d["NearPlane"].as<float>(camera.nearPlane);
                            camera.farPlane  = d["FarPlane"].as<float>(camera.farPlane);
                            camera.FOV       = d["FOV"].as<float>(camera.FOV);
                        }

                        if (auto d = node["TransformComponent"]) {
                            auto& transform = scene.emplace<transformComponent>(entity).Transform;
                            transform.Translate = d["Translate"].as<glm::vec3>(transform.Translate);
                            transform.Rotate    = d["Rotation"].as<glm::vec3>(transform.Rotate);
                            transform.Scale     = d["Scale"].as<glm::vec3>(transform.Scale);
                        }

                        if (auto d = node["RigidBodyComponent"]) {
                            auto& body = scene.emplace<rigidBodyComponent>(entity).RigidBody;
                            body.Density = d["Density"].as<float>(body.Density);
                            body.Dynamic = d["Dynamic"].as<bool>(body.Dynamic);
                        }

                        if (auto d = node["ColliderComponent"]) {
                            auto& collider = scene.emplace<colliderComponent>(entity).Collider;
                            collider.DynamicFriction = d["DynamicFriction"].as<float>(collider.DynamicFriction);
                            collider.StaticFriction  = d["StaticFriction"].as<float>(collider.StaticFriction);
                            collider.Restitution     = d["Restitution"].as<float>(collider.Restitution);
                            const auto name = d["Type"].as<std::string>("BOX");
                            auto opt = magic_enum::enum_cast<ColliderType>(name);
                            if (opt.has_value()) { collider.Type = opt.value(); }
                        }

                        if (auto d = node["CharControllerComponent"]) {
                            auto& control = scene.emplace<charControllerComponent>(entity);
                            control.Controller.Radius     = d["Radius"].as<float>(control.Controller.Radius);
                            control.Controller.Height     = d["Height"].as<float>(control.Controller.Height);
                            control.Controller.StepOffset = d["StepOffset"].as<float>(control.Controller.StepOffset);
                            control.Controller.MoveSpeed  = d["MoveSpeed"].as<float>(control.Controller.MoveSpeed);
                            control.Controller.EyeOffset  = d["EyeOffset"].as<float>(control.Controller.EyeOffset);
                            control.Controller.SpawnPosition = d["SpawnPosition"].as<glm::vec3>(glm::vec3(0.0f, 0.0f, 10.0f));
                            control.Controller.SpawnYaw      = d["SpawnYaw"].as<float>(0.0f);
                        }

                        if (auto d = node["ModelComponent"]) {
                            auto& comp = scene.emplace<modelComponent>(entity);
                            comp.Material = d["Material"].as<AssetID>(comp.Material);
                            comp.Model    = d["Model"].as<AssetID>(comp.Model);
                        }

                        if (auto d = node["ScriptComponent"]) {
                            auto& comp = scene.emplace<scriptComponent>(entity);
                            comp.Script = d["Script"].as<AssetID>(comp.Script);
                        }

                        if (auto d = node["SkyboxComponent"]) {
                            auto& comp = scene.emplace<skyboxComponent>(entity);
                            comp.Sky = d["Skybox"].as<AssetID>(comp.Sky);
                        }

                        if (auto d = node["DirectLightComponent"]) {
                            auto& light = scene.emplace<directLightComponent>(entity).Light;
                            light.Radiance  = d["Radiance"].as<glm::vec3>(light.Radiance);
                            light.Intensity = d["Intensity"].as<float>(light.Intensity);
                        }

                        if (auto d = node["PointLightComponent"]) {
                            auto& light = scene.emplace<pointLightComponent>(entity).Light;
                            light.Radiance  = d["Radiance"].as<glm::vec3>(light.Radiance);
                            light.Intensity = d["Intensity"].as<float>(light.Intensity);
                        }

                        if (auto d = node["SpotLightComponent"]) {
                            auto& light = scene.emplace<spotLightComponent>(entity).Light;
                            light.Radiance  = d["Radiance"].as<glm::vec3>(light.Radiance);
                            light.Intensity = d["Intensity"].as<float>(light.Intensity);
                            light.Falloff   = d["Falloff"].as<float>(light.Falloff);
                            light.Cutoff    = d["Cutoff"].as<float>(light.Cutoff);
                        }
                    }
                    catch (YAML::Exception& e) {
                        FLOW_ERROR("Partially loaded entity: {}", e.what());
                    }
                }
            }
            catch (YAML::Exception&) {
                FLOW_ERROR("Failed to Deserialize Scene snapshot.");
            }
        }

        // ── File deserialize (reads file, delegates to string version) ──

        FLOW_INLINE void Deserialize(entityRegistry& scene, const std::string& path) {
            try {
                std::ifstream t(path);
                std::stringstream buffer;
                buffer << t.rdbuf();
                DeserializeFromString(scene, buffer.str());
            }
            catch (...) {
                FLOW_ERROR("Failed to Deserialize Scene from file: {}", path);
            }
        }

        // ── Asset serialize ──

        // sourceMap lets callers (e.g. export) rewrite Source without mutating the registry
        using SourceMapper = std::function<std::string(const Asset&)>;

        FLOW_INLINE void Serialize(AssetRegistry& registry, const std::string& path,
                const SourceMapper& sourceMap = nullptr) {
            YAML::Emitter emitter;

            emitter << YAML::BeginMap;
            {
                emitter << YAML::Key << "ASSETS" << YAML::Value << YAML::BeginSeq;
                {
                    registry.View([&] (Asset* asset) {
                        std::string typeName(magic_enum::enum_name(asset->Type));

                        emitter << YAML::BeginMap;
                        {
                            emitter << YAML::Key << "UID"    << YAML::Value << asset->UID;
                            emitter << YAML::Key << "Type"   << YAML::Value << typeName;
                            emitter << YAML::Key << "Name"   << YAML::Value << asset->Name;
                            emitter << YAML::Key << "Source" << YAML::Value
                                << (sourceMap ? sourceMap(*asset) : asset->Source);

                            if (asset->Type == AssetType::MATERIAL) {
                                auto mtl = static_cast<MaterialAsset*>(asset);
                                emitter << YAML::Key << "Properties" << YAML::BeginMap;
                                {
                                    emitter << YAML::Key << "RoughnessMap"  << YAML::Value << mtl->RoughnessMap;
                                    emitter << YAML::Key << "OcclusionMap"  << YAML::Value << mtl->OcclusionMap;
                                    emitter << YAML::Key << "EmissiveMap"   << YAML::Value << mtl->EmissiveMap;
                                    emitter << YAML::Key << "MetallicMap"   << YAML::Value << mtl->MetallicMap;
                                    emitter << YAML::Key << "AlbedoMap"     << YAML::Value << mtl->AlbedoMap;
                                    emitter << YAML::Key << "NormalMap"     << YAML::Value << mtl->NormalMap;

                                    emitter << YAML::Key << "Roughness" << YAML::Value << mtl->Data.Roughness;
                                    emitter << YAML::Key << "Occlusion" << YAML::Value << mtl->Data.Occlusion;
                                    emitter << YAML::Key << "Metallic"  << YAML::Value << mtl->Data.Metallic;
                                    emitter << YAML::Key << "Emissive"  << YAML::Value << mtl->Data.Emissive;
                                    emitter << YAML::Key << "Albedo"    << YAML::Value << mtl->Data.Albedo;
                                }
                                emitter << YAML::EndMap;
                            }
                            else if (asset->Type == AssetType::TEXTURE) {
                                auto texture = static_cast<TextureAsset*>(asset);
                                emitter << YAML::Key << "Properties" << YAML::BeginMap;
                                {
                                    emitter << YAML::Key << "IsHDR" << YAML::Value << texture->isHDR;
                                    emitter << YAML::Key << "FlipV" << YAML::Value << texture->flipV;
                                }
                                emitter << YAML::EndMap;
                            }
                            else if (asset->Type == AssetType::SKYBOX) {
                                auto skybox = static_cast<SkyboxAsset*>(asset);
                                emitter << YAML::Key << "Properties" << YAML::BeginMap;
                                {
                                    emitter << YAML::Key << "IsHDR" << YAML::Value << skybox->isHDR;
                                    emitter << YAML::Key << "FlipV" << YAML::Value << skybox->flipV;
                                    emitter << YAML::Key << "Size"  << YAML::Value << skybox->Size;
                                }
                                emitter << YAML::EndMap;
                            }
                            else if (asset->Type == AssetType::MODEL) {
                                auto model = static_cast<ModelAsset*>(asset);
                                emitter << YAML::Key << "Properties" << YAML::BeginMap;
                                {
                                    emitter << YAML::Key << "HasJoints" << YAML::Value << model->HasJoints;
                                }
                                emitter << YAML::EndMap;
                            }
                        }
                        emitter << YAML::EndMap;
                    });
                }
                emitter << YAML::EndSeq;
            }
            emitter << YAML::EndMap;

            std::ofstream filepath(path);
            if (filepath.is_open()) {
                filepath << emitter.c_str();
            }
        }

        // ── Asset deserialize ──

        FLOW_INLINE void Deserialize(AssetRegistry& registry, const std::string& path) {
            YAML::Node root;
            try {
                root = YAML::LoadFile(path);
            }
            catch (YAML::Exception& e) {
                FLOW_ERROR("Failed to Deserialize Assets from {}: {}", path, e.what());
                return;
            }

            for (const auto& node : root["ASSETS"]) {
                try {
                    const auto props    = node["Properties"];
                    const auto uid      = node["UID"].as<AssetID>();
                    const auto source   = node["Source"].as<std::string>("");
                    const auto typeName = node["Type"].as<std::string>("");
                    const auto opt  = magic_enum::enum_cast<AssetType>(typeName);
                    const auto type = (opt.has_value()) ? opt.value() : AssetType::UNKNOWN;

                    Asset* asset = nullptr;
                    if (type == AssetType::MATERIAL) {
                        auto mtlAsset = registry.AddMaterial(uid, source);
                        if (props) {
                            mtlAsset->RoughnessMap = props["RoughnessMap"].as<AssetID>(EMPTY_ASSET);
                            mtlAsset->OcclusionMap = props["OcclusionMap"].as<AssetID>(EMPTY_ASSET);
                            mtlAsset->EmissiveMap  = props["EmissiveMap"].as<AssetID>(EMPTY_ASSET);
                            mtlAsset->MetallicMap  = props["MetallicMap"].as<AssetID>(EMPTY_ASSET);
                            mtlAsset->AlbedoMap    = props["AlbedoMap"].as<AssetID>(EMPTY_ASSET);
                            mtlAsset->NormalMap    = props["NormalMap"].as<AssetID>(EMPTY_ASSET);

                            auto& data = mtlAsset->Data;
                            data.Emissive  = props["Emissive"].as<glm::vec3>(data.Emissive);
                            data.Occlusion = props["Occlusion"].as<float>(data.Occlusion);
                            data.Roughness = props["Roughness"].as<float>(data.Roughness);
                            data.Metallic  = props["Metallic"].as<float>(data.Metallic);
                            data.Albedo    = props["Albedo"].as<glm::vec3>(data.Albedo);
                        }
                        asset = mtlAsset.get();
                    }
                    else if (type == AssetType::TEXTURE) {
                        bool isHDR = props ? props["IsHDR"].as<bool>(false) : false;
                        bool flipV = props ? props["FlipV"].as<bool>(true) : true;
                        asset = registry.AddTexture(uid, source, isHDR, flipV).get();
                    }
                    else if (type == AssetType::SKYBOX) {
                        bool isHDR = props ? props["IsHDR"].as<bool>(true) : true;
                        bool flipV = props ? props["FlipV"].as<bool>(true) : true;
                        auto size  = props ? props["Size"].as<int32_t>(2048) : 2048;
                        asset = registry.AddSkybox(uid, source, size, isHDR, flipV).get();
                    }
                    else if (type == AssetType::MODEL) {
                        bool hasJoints = props ? props["HasJoints"].as<bool>(false) : false;
                        asset = registry.AddModel(uid, source, hasJoints).get();
                    }
                    else if (type == AssetType::SCRIPT) {
                        asset = registry.AddScript(uid, source).get();
                    }
                    else if (type == AssetType::SCENE) {
                        asset = registry.AddScene(uid, source).get();
                    }
                    else {
                        FLOW_ERROR("Skipping asset {}: invalid type '{}'", uid, typeName);
                        continue;
                    }

                    asset->Name = node["Name"].as<std::string>(asset->Name);
                }
                catch (YAML::Exception& e) {
                    FLOW_ERROR("Skipping malformed asset entry in {}: {}", path, e.what());
                }
            }
        }
    };
}
