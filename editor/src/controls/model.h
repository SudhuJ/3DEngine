#pragma once

#include "IControl.h"

struct ModelControl : IControl<modelComponent> {
    FLOW_INLINE ModelControl(GuiContext* context) : IControl(context) {
        setTitle(ICON_FA_CUBE "\tModel");
    }

    FLOW_INLINE void onBody(GuiContext* context, Entity& entity) {
        // "Remove" in the gear menu runs earlier in the same frame
        if (!entity.template Has<modelComponent>()) return;
        auto& comp = entity.template Get<modelComponent>();
        auto& assets = *context->getAppContext()->Assets;

        InputAsset<ModelAsset>("Model", assets, comp.Model);
        InputAsset<MaterialAsset>("Material", assets, comp.Material);

        if (comp.Material == EMPTY_ASSET) return;
        auto& mtl = assets.Get<MaterialAsset>(comp.Material).Data;
        InputColor("Albedo", &mtl.Albedo.x);
        InputColor("Emissive", &mtl.Emissive.x);
        InputFloat("Roughness", &mtl.Roughness);
        InputFloat("Metallic", &mtl.Metallic);
        InputFloat("Occlusion", &mtl.Occlusion);
    }

    FLOW_INLINE void onMenu(GuiContext* context, Entity& entity) {
        if (ImGui::Selectable(ICON_FA_RECYCLE "\tReset")) {
            entity.template Get<modelComponent>() = {};
        }
        if (ImGui::Selectable(ICON_FA_TRASH "\tRemove")) {
            entity.template Detach<modelComponent>();
        }
    }

    private:
        // combo over every registered asset of type T; EMPTY_ASSET shows as "None"
        template <typename T>
        FLOW_INLINE void InputAsset(const char* label, AssetRegistry& assets, AssetID& uid) {
            BeginInput(label);
            const char* preview = (uid == EMPTY_ASSET) ? "None" : assets.Get<T>(uid).Name.c_str();
            if (ImGui::BeginCombo("##", preview)) {
                if (ImGui::Selectable("None", uid == EMPTY_ASSET)) {
                    uid = EMPTY_ASSET;
                }
                for (auto& [id, asset] : assets.GetMap<T>()) {
                    if (id == EMPTY_ASSET) continue;
                    ImGui::PushID((void*)asset.get());
                    if (ImGui::Selectable(asset->Name.c_str(), id == uid)) {
                        uid = id;
                    }
                    ImGui::PopID();
                }
                ImGui::EndCombo();
            }
            EndInput();
        }
};
