//
// Created by Kyle Bueche on 11/10/2025.
//

#ifndef MODEL_H
#define MODEL_H

#include <../thirdparty/include/assimp/Importer.hpp>
#include <../thirdparty/include/assimp/scene.h>
#include <../thirdparty/include/assimp/mesh.h>
#include <../thirdparty/include/assimp/material.h>
#include <../thirdparty/include/assimp/postprocess.h>
#include "shader.h"
#include "mesh.h"
#include "texture.h"
#include <string>

class Model
{
public:
    Model(char *path)
    {
        loadModel(path);
    }
    void Draw(Shader &shader);
private:
    std::vector<Mesh> meshes;
    std::string directory;

    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    void loadMaterialTextures(std::vector<Texture> &textures, aiMaterial *mat, aiTextureType type, std::string typeName);
};

#endif //MODEL_H