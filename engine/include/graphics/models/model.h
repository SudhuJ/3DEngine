#pragma once
#include <assimp/postprocess.h>
#include <assimp/quaternion.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "helper.h"
#include "animator.h"
#include "../buffers/vertex.h"
#include "graphics/utilities/data.h"

namespace flow {
    struct Model {
        FLOW_INLINE virtual JointMatrices* Animate(float) { return nullptr; }
        FLOW_INLINE virtual bool HasJoint() { return false; }
        FLOW_INLINE virtual void Load(const std::string& path) {}
        FLOW_INLINE virtual void Draw(uint32_t mode) {}
    };

    struct staticModel : public Model {
        FLOW_INLINE staticModel() = default;
        FLOW_INLINE staticModel(const std::string& path) {
            Load(path);
        }

        FLOW_INLINE void Load(const std::string& path) override final {
            Assimp::Importer importer;
            uint32_t flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace |
            aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_ValidateDataStructure |
            aiProcess_ImproveCacheLocality | aiProcess_FixInfacingNormals |
            aiProcess_GenUVCoords | aiProcess_FlipUVs;
            const aiScene* ai_Scene = importer.ReadFile(path, flags);
            if (!ai_Scene || (ai_Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !ai_Scene->mRootNode) {
                FLOW_ERROR("Failed to load model: {}", importer.GetErrorString());
                return;
            }
            parseNode(ai_Scene, ai_Scene->mRootNode);
        }

        FLOW_INLINE void Draw(uint32_t mode) override final {
            for (auto& mesh : m_Meshes) {
                mesh->Draw(mode);
            }
        }

        private:
            FLOW_INLINE void parseNode(const aiScene* ai_Scene, aiNode* ai_Node) {
                for (uint32_t i = 0; i < ai_Node->mNumMeshes; i++) {
                    parseMesh(ai_Scene->mMeshes[ai_Node->mMeshes[i]]);
                }
                for (uint32_t i = 0; i < ai_Node->mNumChildren; i++) {
                    parseNode(ai_Scene, ai_Node->mChildren[i]);
                }
            }

            FLOW_INLINE void parseMesh(aiMesh* ai_Mesh) {
                meshData<shadedVertex> data;
                for (uint32_t i = 0; i < ai_Mesh->mNumVertices; i++) {
                    shadedVertex vertex;
                    vertex.Position = assimptoVec3(ai_Mesh->mVertices[i]);
                    vertex.Normal = ai_Mesh->mNormals ? assimptoVec3(ai_Mesh->mNormals[i]) : glm::vec3(0.0f);
                    if (ai_Mesh->HasTextureCoords(0)) {
                        vertex.UVs.x = ai_Mesh->mTextureCoords[0][i].x;
                        vertex.UVs.y = ai_Mesh->mTextureCoords[0][i].y;
                    }
                    vertex.Tangent = ai_Mesh->mTangents ? glm::normalize(assimptoVec3(ai_Mesh->mTangents[i])) : glm::vec3(0.0f);
                    vertex.Bitangent = ai_Mesh->mBitangents ? glm::normalize(assimptoVec3(ai_Mesh->mBitangents[i])) : glm::vec3(0.0f);
                    data.vertices.push_back(vertex);
                }

                for (uint32_t i = 0; i < ai_Mesh->mNumFaces; i++) {
                    for (uint32_t j = 0; j < ai_Mesh->mFaces[i].mNumIndices; j++) {
                        data.indices.push_back(ai_Mesh->mFaces[i].mIndices[j]);
                    }
                }
                auto mesh = std::make_shared<shadedMesh>(data);
                m_Meshes.push_back(std::move(mesh));
            }

            std::vector<mesh3D> m_Meshes;
    };

    struct skeletalModel : public Model {

        using JointMap = std::unordered_map<std::string, Joint>;
        FLOW_INLINE skeletalModel() = default;
        FLOW_INLINE skeletalModel(const std::string& path) {
            Load(path);
        }

        FLOW_INLINE bool HasJoint() override final {
            return m_JointCount;
        }

        FLOW_INLINE void Draw(uint32_t mode) override final {
            for (auto& mesh : m_Meshes) {
                mesh->Draw(mode);
            }
        }

        FLOW_INLINE void Load(const std::string& path) override final {
            Assimp::Importer importer;
            uint32_t flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace |
            aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_ValidateDataStructure |
            aiProcess_ImproveCacheLocality | aiProcess_FixInfacingNormals |
            aiProcess_SortByPType | aiProcess_JoinIdenticalVertices |
            aiProcess_FlipUVs | aiProcess_GenUVCoords |
            aiProcess_LimitBoneWeights;
            const aiScene* ai_Scene = importer.ReadFile(path, flags);
            if (!ai_Scene || (ai_Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !ai_Scene->mRootNode) {
                FLOW_ERROR("Failed to load model: {}", importer.GetErrorString());
                return;
            }
            m_Animator = std::make_shared<Animator>();
            m_Animator->m_GlobalTransform = glm::inverse(assimptoMat4(ai_Scene->mRootNode->mTransformation));
            JointMap jointMap = {};
            parseNode(ai_Scene, ai_Scene->mRootNode, jointMap);
            parseAnimations(ai_Scene, jointMap);
        }

        FLOW_INLINE auto GetAnimator() {
            return m_Animator;
        }

        FLOW_INLINE JointMatrices* Animate(float dt) override final {
            return m_Animator->Animate(dt);
        }

        private:
            FLOW_INLINE void setVertexJoint(skeletalVertex& vertex, int32_t id, float weight) {
                for (uint32_t i = 0; i < 4; i++) {
                    if (vertex.Joints[i] < 0) {
                        vertex.Weights[i] = weight;
                        vertex.Joints[i] = id;
                        return;
                    }
                }
            }

            FLOW_INLINE void parseNode(const aiScene* ai_Scene, aiNode* ai_Node, JointMap& jointMap) {
                for (uint32_t i = 0; i < ai_Node->mNumMeshes; i++) {
                    parseMesh(ai_Scene->mMeshes[ai_Node->mMeshes[i]], jointMap);
                }
                for (uint32_t i = 0; i < ai_Node->mNumChildren; i++) {
                    parseNode(ai_Scene, ai_Node->mChildren[i], jointMap);
                }
            }

            FLOW_INLINE void parseHeirarchy(aiNode* ai_Node, Joint& joint, JointMap& jointMap) {
                std::string jointName(ai_Node->mName.C_Str());
                if (jointMap.count(jointName) != 0) {
                    joint = jointMap[jointName];
                    for (uint32_t i = 0; i < ai_Node->mNumChildren; i++) {
                        Joint child;
                        parseHeirarchy(ai_Node->mChildren[i], child, jointMap);
                        joint.Children.push_back(child);
                    }
                }
                else {
                    for (uint32_t i = 0; i < ai_Node->mNumChildren; i++) {
                        parseHeirarchy(ai_Node->mChildren[i], joint, jointMap);
                    }
                }
            }

            FLOW_INLINE void parseMesh(aiMesh* ai_Mesh, JointMap& jointMap) {
                meshData<skeletalVertex> data;
                for (uint32_t i = 0; i < ai_Mesh->mNumVertices; i++) {
                    skeletalVertex vertex;
                    vertex.Position = assimptoVec3(ai_Mesh->mVertices[i]);
                    vertex.Normal = ai_Mesh->mNormals ? assimptoVec3(ai_Mesh->mNormals[i]) : glm::vec3(0.0f);
                    if (ai_Mesh->HasTextureCoords(0)) {
                        vertex.UVs.x = ai_Mesh->mTextureCoords[0][i].x;
                        vertex.UVs.y = ai_Mesh->mTextureCoords[0][i].y;
                    }
                    vertex.Tangent = ai_Mesh->mTangents ? glm::normalize(assimptoVec3(ai_Mesh->mTangents[i])) : glm::vec3(0.0f);
                    vertex.Bitangent = ai_Mesh->mBitangents ? glm::normalize(assimptoVec3(ai_Mesh->mBitangents[i])) : glm::vec3(0.0f);
                    data.vertices.push_back(vertex);
                }

                for (uint32_t i = 0; i < ai_Mesh->mNumFaces; i++) {
                    for (uint32_t j = 0; j < ai_Mesh->mFaces[i].mNumIndices; j++) {
                        data.indices.push_back(ai_Mesh->mFaces[i].mIndices[j]);
                    }
                }

                for (uint32_t i = 0; i < ai_Mesh->mNumBones; i++) {
                    aiBone* ai_bone = ai_Mesh->mBones[i];
                    std::string jointName(ai_bone->mName.C_Str());
                    if (jointMap.count(jointName) == 0) {
                        jointMap[jointName].Offset = assimptoMat4(ai_bone->mOffsetMatrix);
                        jointMap[jointName].Index = m_JointCount++;
                        jointMap[jointName].Name = jointName;
                    }

                    for (uint32_t j=0; j<ai_bone->mNumWeights; j++) {
                        setVertexJoint(data.vertices[ai_bone->mWeights[j].mVertexId],
                        jointMap[jointName].Index, ai_bone->mWeights[j].mWeight);
                    }
                }
                m_Meshes.push_back(std::make_unique<skeletalMesh>(data));
            }

            FLOW_INLINE void parseAnimations(const aiScene* ai_Scene, JointMap& jointMap) {
                for (uint32_t i = 0; i < ai_Scene->mNumAnimations; i++) {
                    auto ai_anim = ai_Scene->mAnimations[i];
                    Animation animation;
                    animation.Name = ai_anim->mName.C_Str();
                    animation.Duration = ai_anim->mDuration;
                    animation.Speed = ai_anim->mTicksPerSecond;
                    m_Animator->m_Animations.push_back(animation);

                    for (uint32_t j = 0; j < ai_anim->mNumChannels; j++) {
                        aiNodeAnim* ai_channel = ai_anim->mChannels[j];
                        auto jointName(ai_channel->mNodeName.C_Str());
                        if (!jointMap.count(jointName)) {
                            continue;
                        }

                        std::map<float, keyFrame> keys;
                        for (uint32_t k = 0; k < ai_channel->mNumPositionKeys; k++) {
                            float t = ai_channel->mPositionKeys[k].mTime;
                            keys[t].Position = assimptoVec3(ai_channel->mPositionKeys[k].mValue);
                            keys[t].timeStamp = t;
                        }
                        for (uint32_t k = 0; k < ai_channel->mNumRotationKeys; k++) {
                            float t = ai_channel->mRotationKeys[k].mTime;
                            keys[t].Rotation = assimptoQuat(ai_channel->mRotationKeys[k].mValue);
                            keys[t].timeStamp = t;
                        }
                        for (uint32_t k = 0; k < ai_channel->mNumScalingKeys; k++) {
                            float t = ai_channel->mScalingKeys[k].mTime;
                            keys[t].Scale = assimptoVec3(ai_channel->mScalingKeys[k].mValue);
                            keys[t].timeStamp = t;
                        }
                        for (auto& [t, key] : keys) {
                            jointMap[jointName].Keys.push_back(key);
                        }
                    }
                }
                parseHeirarchy(ai_Scene->mRootNode, m_Animator->m_Root, jointMap);
                m_Animator->m_Transforms.resize(m_JointCount);
            }

        private:
            std::vector<std::unique_ptr<skeletalMesh>> m_Meshes;
            std::shared_ptr<Animator> m_Animator;
            uint32_t m_JointCount = 0u;
    };

    using model3D = std::shared_ptr<Model>;
    using animator3D = std::shared_ptr<Animator>;
}
