#pragma once

#include <GL/glew.h>

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "BufferLayout.h"

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vbo, const IndexBuffer& ibo, const BufferLayout& layout);

	void Bind() const;
	void Unbind() const;
private:
	unsigned int mID;
};