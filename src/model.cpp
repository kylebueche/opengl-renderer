//
// Created by kyleb on 11/10/2025.
//

#include "model.h"
#include <string>

void Model::Draw(Shader &shader)
{
    for (int i = 0; i < meshes.size(); i++)
    {
        meshes[i].draw();
    }
}

void Model::loadModel(std::string path)
{
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (nullptr == scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || nullptr == scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    for (int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for (int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    Mesh newMesh;
    glm::vec3 vector;
    glm::vec2 vec;
    for (int i = 0; i < mesh->mNumVertices; i++)
    {
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        newMesh.vertices.push_back(vector);
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        newMesh.normals.push_back(vector);
        if (mesh->mTextureCoords[0])
        {
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.x = mesh->mTextureCoords[0][i].y;
        }
        else
        {
            vec = glm::vec2(0.0f, 0.0f);
        }
        newMesh.texCoords.push_back(vec);
    }
    for (int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            newMesh.indices.push_back(face.mIndices[j]);
        }
    }
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        //loadMaterialTextures(newMesh., material, aiTextureType_DIFFUSE, "texture_diffuse");
        //loadMaterialTextures(newMesh.textures, material, aiTextureType_SPECULAR, "texture_specular");
    }
    return newMesh;
}

void Model::loadMaterialTextures(std::vector<Texture> &textures, aiMaterial *mat, aiTextureType type, std::string typeName)
{
    for (int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        Texture texture;
        //texture.id = loadTexture(str.C_Str(), directory);
        //texture.type = typeName;
        //texture.path = str.C_Str();
        textures.push_back(texture);
    }
}