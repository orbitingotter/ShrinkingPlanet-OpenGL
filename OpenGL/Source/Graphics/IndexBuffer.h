#pragma once

#include <GL/glew.h>

class IndexBuffer
{
public:
	IndexBuffer(const unsigned int* data, unsigned int count);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;

	unsigned int GetCount() const;

private:
	unsigned int mID;
	unsigned int mCount;
};

