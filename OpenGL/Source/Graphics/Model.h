#pragma once

#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model
{
public:
	Model(const std::string& filePath);
	~Model();

	std::vector<Mesh>& GetMeshes() { return mMeshes; }

	// stats
	inline const int GetVertexCount() { return mVertexCount; }
	inline const int GetIndexCount() { return mIndexCount; }
	inline const int GetTextureCount() { return mTextureCount; }
	inline const int GetNumMeshes() { return mNumMeshes; }


private:
	void ProcessNode(aiNode* node, const aiScene* scene);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene);
	void LoadMaterialTextures(std::vector<Texture2D>& textures,aiMaterial* mat, aiTextureType type, const std::string& typeName);
private:
	std::vector<Mesh> mMeshes;
	std::string mDirectory;
	std::unordered_map<std::string, Texture2D> mTextureCache;
	//std::vector<Texture2D> mTextureCache;

	unsigned int mVertexCount;
	unsigned int mIndexCount;
	unsigned int mTextureCount;
	unsigned int mNumMeshes;

};