#pragma once
#include "../buffers/mesh.h"
#include "helper.h"

namespace flow {
    struct Animation {
        float Duration = 0.0f;
        float Speed = 1.0f;
        std::string Name;
    };

    struct keyFrame {
        glm::vec3 Position = glm::vec3(0.0f);
        glm::quat Rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 Scale = glm::vec3(1.0f);
        float timeStamp = 0.0f;
    };

    struct Joint {
        std::vector<Joint> Children;
        std::vector<keyFrame> Keys;
        std::string Name;
        glm::mat4 Offset = glm::mat4(1.0f);
        int32_t Index = -1;
    };
    using JointMatrices = std::vector<glm::mat4>;
}
