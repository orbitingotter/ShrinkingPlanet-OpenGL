#pragma once

#include "Graphics/VertexArray.h"
#include "Graphics\Shader.h"

class Cubemap
{
public:
	Cubemap(const std::vector<std::string>& facesFilepath);
	~Cubemap();

	void Bind();

	inline const int GetIndexCount() const { return mIBO->GetCount(); }
private:
	unsigned int mID;

	std::vector<float> mPositions;
	std::vector<unsigned int> mIndices;

	std::unique_ptr<VertexArray> mVAO;
	std::unique_ptr<VertexBuffer> mVBO;
	std::unique_ptr<IndexBuffer> mIBO;
};