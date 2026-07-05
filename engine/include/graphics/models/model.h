#pragma once
#include <assimp/postprocess.h>
#include <assimp/quaternion.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "helper.h"
namespace flow {
    struct Model {
        FLOW_INLINE Model() = default;
        FLOW_INLINE Model(const std::string& path) {
            Load(path);
        }

        FLOW_INLINE void Load(const std::string& path) {
            Assimp::Importer importer;
            uint32_t flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace |
            aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_ValidateDataStructure |
            aiProcess_ImproveCacheLocality | aiProcess_FixInfacingNormals |
            aiProcess_GenUVCoords | aiProcess_FlipUVs;
            const aiScene* ai_Scene = importer.ReadFile(path, flags);
            if (!ai_Scene || ai_Scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !ai_Scene->mRootNode) {
                FLOW_ERROR("Failed to load model: {}", importer.GetErrorString());
                return;
            }
            parseNode(ai_Scene, ai_Scene->mRootNode);
        }

        FLOW_INLINE void Draw(uint32_t mode) {
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
                    vertex.Normal = assimptoVec3(ai_Mesh->mNormals[i]);
                    vertex.UVs.x = ai_Mesh->mTextureCoords[0][i].x;
                    vertex.UVs.y = ai_Mesh->mTextureCoords[0][i].y;
                    vertex.Tangent = glm::normalize(assimptoVec3(ai_Mesh->mTangents[i]));
                    vertex.Bitangent = glm::normalize(assimptoVec3(ai_Mesh->mBitangents[i]));
                    data.vertices.push_back(vertex);
                }

                for (uint32_t i = 0; i < ai_Mesh->mNumFaces; i++) {
                    for (uint32_t j = 0; j < ai_Mesh->mFaces[i].mNumIndices; j++) {
                        data.indices.push_back(ai_Mesh->mFaces[i].mIndices[j]);
                    }
                }
                auto mesh = std::make_unique<shadedMesh>(data);
                m_Meshes.push_back(std::move(mesh));
            }

            std::vector<mesh3D> m_Meshes;
    };
    using model3D = std::shared_ptr<Model>;
}
