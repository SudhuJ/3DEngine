#pragma once
#include <fstream>
#include <filesystem>
#include <memory>
#include <unordered_set>
#include <yaml-cpp/yaml.h>
#include "assets.h"
#include "serializer.h"

namespace flow {
    struct Project {
        std::string Name;
        std::string Dir;

        std::string FilePath()   const { return Dir + "/project.yaml"; }
        std::string ScenePath()  const { return Dir + "/scene.yaml"; }
        std::string AssetsPath() const { return Dir + "/assets.yaml"; }

        // project-relative if inside the project, cwd-relative if under resources/, else absolute
        std::string MakeRelative(const std::string& path) const {
            auto abs = std::filesystem::absolute(path).lexically_normal();
            auto isUnder = [&](const std::filesystem::path& root) {
                auto rel = abs.lexically_relative(root);
                return !rel.empty() && *rel.begin() != "..";
            };
            auto projRoot = std::filesystem::absolute(Dir).lexically_normal();
            if (!Dir.empty() && isUnder(projRoot)) {
                return abs.lexically_relative(projRoot).generic_string();
            }
            auto cwd = std::filesystem::current_path();
            if (isUnder(cwd / "resources")) {
                return abs.lexically_relative(cwd).generic_string();
            }
            return abs.generic_string();
        }
    };

    struct ProjectManager {
        AssetRegistry*  Assets = nullptr;
        entityRegistry* Scene  = nullptr;
        DataSerializer* Serializer = nullptr;
        std::unique_ptr<Project> Current;

        ProjectManager(AssetRegistry* a, entityRegistry* s, DataSerializer* ser)
            : Assets(a), Scene(s), Serializer(ser) {}

        Project* OpenProject(const std::string& dir) {
            auto proj = std::make_unique<Project>();
            proj->Dir = std::filesystem::path(dir).lexically_normal().string();
            proj->Name = std::filesystem::path(dir).filename().string();

            auto meta = std::filesystem::path(dir) / "project.yaml";
            if (std::filesystem::exists(meta)) {
                try {
                    auto node = YAML::LoadFile(meta.string());
                    proj->Name = node["name"].as<std::string>(proj->Name);
                }
                catch (YAML::Exception&) {
                    FLOW_WARN("Could not read {}", meta.string());
                }
            }

            Scene->clear();
            Assets->Reset();
            Assets->BaseDir = proj->Dir;
            if (std::filesystem::exists(proj->AssetsPath())) {
                Serializer->Deserialize(*Assets, proj->AssetsPath());
                Assets->ResolveMaterialTextures();
            }
            if (std::filesystem::exists(proj->ScenePath())) {
                Serializer->Deserialize(*Scene, proj->ScenePath());
            }

            Current = std::move(proj);
            return Current.get();
        }

        bool SaveProject() {
            if (!Current) {
                FLOW_WARN("No project open: nothing saved.");
                return false;
            }
            std::filesystem::create_directories(Current->Dir);
            Serializer->Serialize(*Assets, Current->AssetsPath());
            Serializer->Serialize(*Scene,  Current->ScenePath());
            WriteMeta(Current->Dir, Current->Name);
            FLOW_INFO("Saved project to {}", Current->Dir);
            return true;
        }

        // Self-contained copy: asset files are copied into <dir>/assets and
        // sources rewritten relative to <dir>. retarget makes <dir> the open project (Save As).
        bool ExportProject(const std::string& dir, bool retarget = false) {
            namespace fs = std::filesystem;
            std::error_code ec;
            const fs::path root = fs::path(dir).lexically_normal();
            const fs::path assetDir = root / "assets";
            fs::create_directories(assetDir, ec);
            if (ec) {
                FLOW_ERROR("Export failed: cannot create {}: {}", assetDir.string(), ec.message());
                return false;
            }

            std::unordered_map<AssetID, std::string> remap;
            std::unordered_map<std::string, std::string> copied; // resolved src -> exported rel
            std::unordered_set<std::string> written;
            Assets->View([&](Asset* asset) {
                if (asset->Type == AssetType::MATERIAL || asset->Type == AssetType::SCENE ||
                    asset->Source.empty()) {
                    return;
                }
                const fs::path src = fs::absolute(Assets->Resolve(asset->Source)).lexically_normal();
                if (auto it = copied.find(src.string()); it != copied.end()) {
                    remap[asset->UID] = it->second;
                    return;
                }
                if (!fs::exists(src)) {
                    FLOW_WARN("Export: missing source {} for asset {}", src.string(), asset->Name);
                    return;
                }

                // unique file name per export; stale files from earlier exports are overwritten
                fs::path target = assetDir / src.filename();
                for (int32_t i = 1; written.count(target.string()); i++) {
                    target = assetDir / (src.stem().string() + "_" + std::to_string(i) +
                        src.extension().string());
                }
                written.insert(target.string());
                if (!(fs::exists(target) && fs::equivalent(target, src, ec))) {
                    fs::copy_file(src, target, fs::copy_options::overwrite_existing, ec);
                    if (ec) {
                        FLOW_ERROR("Export: failed to copy {}: {}", src.string(), ec.message());
                        return;
                    }
                }
                const auto rel = target.lexically_relative(root).generic_string();
                copied[src.string()] = rel;
                remap[asset->UID] = rel;
            });

            const std::string name = root.filename().string();
            Serializer->Serialize(*Assets, (root / "assets.yaml").string(), [&](const Asset& asset) {
                auto it = remap.find(asset.UID);
                return it != remap.end() ? it->second : asset.Source;
            });
            Serializer->Serialize(*Scene, (root / "scene.yaml").string());
            WriteMeta(root.string(), name);

            if (retarget) {
                Current = std::make_unique<Project>();
                Current->Dir = root.string();
                Current->Name = name;
                Assets->BaseDir = Current->Dir;
                Assets->View([&](Asset* asset) {
                    if (auto it = remap.find(asset->UID); it != remap.end()) {
                        asset->Source = it->second;
                    }
                });
            }
            FLOW_INFO("Exported project to {}", root.string());
            return true;
        }

        void CloseProject() { Current.reset(); }

        private:
            void WriteMeta(const std::string& dir, const std::string& name) {
                YAML::Emitter out;
                out << YAML::BeginMap;
                out << YAML::Key << "name"   << YAML::Value << name;
                out << YAML::Key << "engine" << YAML::Value << "0.1.0";
                out << YAML::Key << "scene"  << YAML::Value << "scene.yaml";
                out << YAML::Key << "assets" << YAML::Value << "assets.yaml";
                out << YAML::EndMap;
                std::ofstream(std::filesystem::path(dir) / "project.yaml") << out.c_str();
            }
    };
}
