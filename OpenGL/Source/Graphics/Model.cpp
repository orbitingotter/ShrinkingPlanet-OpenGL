#include "pch.h"
#include "Model.h"


Model::Model(const std::string& filePath)
	: mVertexCount(0), mIndexCount(0), mTextureCount(0), mNumMeshes(0)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath,
		aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR : ASSIMP : " << importer.GetErrorString() << std::endl;
		return;
	}
	// retrieve the directory path of the filepath
	mDirectory = filePath.substr(0, filePath.find_last_of('/'));

	mNumMeshes = scene->mNumMeshes;
	mMeshes.reserve(mNumMeshes);
	ProcessNode(scene->mRootNode, scene);
}

Model::~Model()
{
}


// traverse through all the nodes and process their meshes
void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		ProcessMesh(mesh, scene);
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}


// convert assimp mesh to our mesh class
void Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture2D> textures;


	// Vertices
	vertices.reserve(mesh->mNumVertices);
	Vertex vertex;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		if (mesh->mTextureCoords[0])
		{
			vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		else
		{
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}
		vertices.emplace_back(vertex);
	}

	// Indices
	indices.reserve(mesh->mNumFaces * 3); // assume triangulated
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.emplace_back(face.mIndices[j]);
	}


	if (mesh->mMaterialIndex >= 0)
	{
		// Material / Textures
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		textures.reserve(
			material->GetTextureCount(aiTextureType_DIFFUSE) +
			material->GetTextureCount(aiTextureType_SPECULAR) +
			material->GetTextureCount(aiTextureType_NORMALS));

		// diffuse
		LoadMaterialTextures(textures, material, aiTextureType_DIFFUSE, "Diffuse");
		// specular
		LoadMaterialTextures(textures, material, aiTextureType_SPECULAR, "Specular");
		// normal
		//LoadMaterialTextures(textures, material, aiTextureType_NORMALS, "Normal");

		//std::cout << textures.size() << std::endl;

	}

	mVertexCount += vertices.size();
	mIndexCount += indices.size() / 3;
	mTextureCount = mTextureCache.size();

	mMeshes.emplace_back(vertices, indices, textures);
}

void Model::LoadMaterialTextures(std::vector<Texture2D>& textures, aiMaterial* mat, aiTextureType type, const std::string& typeName)
{
	std::string completeFilePath;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString relPath;
		mat->GetTexture(type, i, &relPath);
		completeFilePath = mDirectory + '/' + relPath.C_Str();

		/*
		bool skip = false;
		for (unsigned int j = 0; j < mTextureCache.size(); j++)
		{
			if (mTextureCache[j].GetFilePath() == completeFilePath)
			{
				textures.emplace_back(mTextureCache[j]);
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			mTextureCache.emplace_back(completeFilePath, typeName);
			textures.emplace_back(mTextureCache.back());
		}
		*/


		if (mTextureCache.find(completeFilePath) != mTextureCache.end())
		{
			textures.emplace_back(mTextureCache.at(completeFilePath));
			continue;
		}

		mTextureCache.emplace(std::make_pair(completeFilePath, std::move(Texture2D(completeFilePath, typeName))));
		textures.emplace_back(mTextureCache.at(completeFilePath));
	}
}


