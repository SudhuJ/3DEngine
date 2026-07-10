#pragma once

#include "scripts/utilities.h"
#include "physics/utilities.h"
#include "graphics/models/model.h"
#include "graphics/textures/texture.h"

namespace flow {
    using AssetID = uint64_t;
    const AssetID EMPTY_ASSET = 0u;

    enum class AssetType :uint8_t {
        UNKNOWN = 0,
        MATERIAL,
        TEXTURE,
        SKYBOX,
        SCRIPT,
        SCENE,
        MODEL,
    };

    struct Asset {
        AssetID UID = EMPTY_ASSET;
        std::string Source;
        std::string Name;
        AssetType Type;
    };

    struct MaterialAsset : Asset {
        AssetID RoughnessMap = EMPTY_ASSET;
        AssetID OcclusionMap = EMPTY_ASSET;
        AssetID MetallicMap = EMPTY_ASSET;
        AssetID EmissiveMap = EMPTY_ASSET;
        AssetID AlbedoMap = EMPTY_ASSET;
        AssetID NormalMap = EMPTY_ASSET;

        Material Data;
    };

    struct TextureAsset : Asset {
        bool isHDR = false;
        bool flipV = true;
        texture2D Data;
    };

    struct SkyboxAsset : Asset {
        int32_t Size = 2048;
        bool isHDR = true;
        bool flipV = true;
        texture2D EnvMap;
        Skybox Data;
    };

    struct ModelAsset : Asset {
        bool HasJoints = false;
        model3D Data;
    };

    struct ScriptAsset : Asset {
        /* ... */
    };

    struct SceneAsset : Asset {
        /* ... */
    };

    using SharedAsset = std::shared_ptr<Asset>;
    using AssetMap = std::unordered_map<AssetID, SharedAsset>;

    struct AssetRegistry {

        FLOW_INLINE AssetRegistry() {
            AddEmpty<MaterialAsset>();
            AddEmpty<TextureAsset>();
            AddEmpty<SkyboxAsset>();
            AddEmpty<ScriptAsset>();
            AddEmpty<ModelAsset>();
            AddEmpty<SceneAsset>();
        }

        template <typename T>
        FLOW_INLINE T& Get(AssetID uid) {
            const uint32_t type = TypeID<T>();
            if (m_Registry[type].count(uid)) {
                return (T&)(*m_Registry[type][uid]);
            }
            return (T&)(*m_Registry[type][EMPTY_ASSET]);
        }

        template <typename F>
        FLOW_INLINE void View(F&& func) {
            for (auto& [_, assetMap] : m_Registry) {
                for (auto& [uid, asset] : assetMap) {
                    if (uid != EMPTY_ASSET) {
                        func(asset.get());
                    }
                }
            }
        }

        template <typename T>
        FLOW_INLINE auto& GetMap() {
            return m_Registry[TypeID<T>()];
        }

        FLOW_INLINE void Clear() {
            m_Registry.clear();
        }

        FLOW_INLINE auto AddSkybox(AssetID uid, const std::string& src, int32_t size,
            bool isHDR = true, bool flipV = true) {
            auto asset = std::make_shared<SkyboxAsset>();
            asset->EnvMap.Load(src, isHDR, flipV);
            asset->Type = AssetType::SKYBOX;
            asset->isHDR = isHDR;
            asset->flipV = flipV;
            asset->Size = size;
            Add(uid, src, asset);
            return asset;
        }

        FLOW_INLINE auto AddTexture(AssetID uid, const std::string& src,
                bool isHDR = false, bool flipV = true) {
            auto asset = std::make_shared<TextureAsset>();
            asset->Data.Load(src, isHDR, flipV);
            asset->Type = AssetType::TEXTURE;
            asset->isHDR = isHDR;
            asset->flipV = flipV;
            Add(uid, src, asset);
            return asset;
        }

        FLOW_INLINE auto AddModel(AssetID uid, const std::string& src, bool hasJoints = false) {
            auto asset = std::make_shared<ModelAsset>();
            asset->HasJoints = hasJoints;
            asset->Type = AssetType::MODEL;
            if (hasJoints) {
                asset->Data = std::make_shared<skeletalModel>(src);
            }
            else {
                asset->Data = std::make_shared<staticModel>(src);
            }
            Add(uid, src, asset);
            return asset;
        }

        FLOW_INLINE auto AddMaterial(AssetID uid, const std::string& name) {
            auto asset = std::make_shared<MaterialAsset>();
            asset->Type = AssetType::MATERIAL;
            Add(uid, name, asset);
            return asset;
        }

        FLOW_INLINE auto AddScript(AssetID uid, const std::string& src) {
            auto asset = std::make_shared<ScriptAsset>();
            asset->Type = AssetType::SCRIPT;
            Add(uid, src, asset);
            return asset;
        }

        FLOW_INLINE auto AddScene(AssetID uid, const std::string& src) {
            auto asset = std::make_shared<SceneAsset>();
            asset->Type = AssetType::SCENE;
            Add(uid, src, asset);
            return asset;
        }

        private:
            template <typename T>
            FLOW_INLINE void Add(AssetID uid, const std::string& src, std::shared_ptr<T>& asset) {
                asset->UID = uid;
                asset->Source = src;
                std::filesystem::path path(src);
                asset->Name = path.stem().string();
                m_Registry[TypeID<T>()][asset->UID] = asset;
            }

            template <typename T>
            FLOW_INLINE void AddEmpty() {
                m_Registry[TypeID<T>()][EMPTY_ASSET] = std::make_shared<T>();
            }

        private:
            std::unordered_map<uint32_t, AssetMap> m_Registry;
    };
}
