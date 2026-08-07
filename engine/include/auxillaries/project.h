#pragma once
#include <fstream>
#include <filesystem>
#include <memory>
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

        static std::string ToContentRel(const std::string& path) {
            auto abs = std::filesystem::absolute(path).lexically_normal();
            auto root = std::filesystem::absolute("resources").lexically_normal();
            if (abs == root || abs.string().rfind(root.string(), 0) == 0) {
                return std::filesystem::relative(abs, std::filesystem::current_path()).generic_string();
            }
            return abs.string();
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
            if (!Current) return false;
            std::filesystem::create_directories(Current->Dir);
            Serializer->Serialize(*Assets, Current->AssetsPath());
            Serializer->Serialize(*Scene,  Current->ScenePath());

            YAML::Emitter out;
            out << YAML::BeginMap;
            out << YAML::Key << "name"   << YAML::Value << Current->Name;
            out << YAML::Key << "engine" << YAML::Value << "0.1.0";
            out << YAML::Key << "scene"  << YAML::Value << "scene.yaml";
            out << YAML::Key << "assets" << YAML::Value << "assets.yaml";
            out << YAML::EndMap;
            std::ofstream(Current->FilePath()) << out.c_str();
            return true;
        }

        void CloseProject() { Current.reset(); }
    };
}
