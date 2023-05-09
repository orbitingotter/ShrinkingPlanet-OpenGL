#pragma once

#include "Graphics/VertexArray.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture2D.h"
#include <glm/glm.hpp>

struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Material
{

};

class Mesh
{
public:
	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture2D>& textures);
	~Mesh();

	void BindTextures(Shader& shader);
	inline void Bind() const { mVAO.Bind(); }
	inline const unsigned int GetIndexCount() const { return mIBO.GetCount(); }

private:
	void SetupMesh();

private:
	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndices;
	std::vector<Texture2D> mTextures;

	VertexArray mVAO;
	VertexBuffer mVBO;
	IndexBuffer mIBO;
};