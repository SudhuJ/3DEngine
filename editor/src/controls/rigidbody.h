#pragma once

#include "IControl.h"

// physics actors are built from these values when Play starts, so editing is
// locked while the simulation runs (changes would not reach PhysX anyway)
FLOW_INLINE bool PhysicsEditable(GuiContext* context) {
    return context->getAppContext()->Mode == engineMode::EDITOR;
}

struct RigidBodyControl : IControl<rigidBodyComponent> {
    FLOW_INLINE RigidBodyControl(GuiContext* context) : IControl(context) {
        setTitle(ICON_FA_RING "\tRigid Body");
    }

    FLOW_INLINE void onBody(GuiContext* context, Entity& entity) {
        // "Remove" in the gear menu runs earlier in the same frame
        if (!entity.template Has<rigidBodyComponent>()) return;
        auto& body = entity.template Get<rigidBodyComponent>().RigidBody;

        ImGui::BeginDisabled(!PhysicsEditable(context));
        int32_t mode = body.Dynamic ? 1 : 0;
        BeginInput("Type");
        if (ImGui::Combo("##", &mode, "Static\0Dynamic\0")) {
            body.Dynamic = (mode == 1);
        }
        EndInput();

        ImGui::BeginDisabled(!body.Dynamic);
        if (InputFloat("Density", &body.Density)) {
            body.Density = glm::max(body.Density, 0.001f);
        }
        ImGui::EndDisabled();

        if (!entity.template Has<colliderComponent>()) {
            ImGui::TextDisabled("No collider: the body won't be simulated.");
        }
        ImGui::EndDisabled();
    }

    FLOW_INLINE void onMenu(GuiContext* context, Entity& entity) {
        if (!PhysicsEditable(context)) {
            ImGui::TextDisabled("Stop Play to edit.");
            return;
        }
        if (ImGui::Selectable(ICON_FA_RECYCLE "\tReset")) {
            entity.template Get<rigidBodyComponent>() = {};
        }
        if (ImGui::Selectable(ICON_FA_TRASH "\tRemove")) {
            entity.template Detach<rigidBodyComponent>();
        }
    }
};

struct ColliderControl : IControl<colliderComponent> {
    FLOW_INLINE ColliderControl(GuiContext* context) : IControl(context) {
        setTitle(ICON_FA_CUBES "\tCollider");
    }

    FLOW_INLINE void onBody(GuiContext* context, Entity& entity) {
        if (!entity.template Has<colliderComponent>()) return;
        auto& collider = entity.template Get<colliderComponent>().Collider;

        ImGui::BeginDisabled(!PhysicsEditable(context));

        // only the shapes physicsContext::AddRigidBody can build
        static const ColliderType types[] = { BOX, SPHERE, MESH };
        static const char* labels[] = { "Box", "Sphere", "Mesh" };
        int32_t current = 0;
        for (int32_t i = 0; i < 3; i++) {
            if (types[i] == collider.Type) current = i;
        }
        BeginInput("Shape");
        if (ImGui::Combo("##", &current, labels, 3)) {
            collider.Type = types[current];
        }
        EndInput();

        InputFloat("Static Friction", &collider.StaticFriction);
        InputFloat("Dynamic Friction", &collider.DynamicFriction);
        InputFloat("Restitution", &collider.Restitution);
        collider.StaticFriction  = glm::max(collider.StaticFriction, 0.0f);
        collider.DynamicFriction = glm::max(collider.DynamicFriction, 0.0f);
        collider.Restitution     = glm::clamp(collider.Restitution, 0.0f, 1.0f);

        // shape size comes from the transform, so explain where it's set
        if (collider.Type == BOX) {
            ImGui::TextDisabled("Size: Transform scale.");
        }
        else if (collider.Type == SPHERE) {
            ImGui::TextDisabled("Radius: Transform scale.x / 2.");
        }
        else if (collider.Type == MESH) {
            ImGui::TextDisabled("Uses the Model's first mesh.");
            bool dynamic = entity.template Has<rigidBodyComponent>() &&
                entity.template Get<rigidBodyComponent>().RigidBody.Dynamic;
            if (dynamic) {
                ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "Mesh colliders need a Static body.");
            }
            if (!entity.template Has<modelComponent>()) {
                ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "Needs a Model component.");
            }
        }
        ImGui::EndDisabled();
    }

    FLOW_INLINE void onMenu(GuiContext* context, Entity& entity) {
        if (!PhysicsEditable(context)) {
            ImGui::TextDisabled("Stop Play to edit.");
            return;
        }
        if (ImGui::Selectable(ICON_FA_RECYCLE "\tReset")) {
            entity.template Get<colliderComponent>() = {};
        }
        if (ImGui::Selectable(ICON_FA_TRASH "\tRemove")) {
            entity.template Detach<colliderComponent>();
        }
    }
};
