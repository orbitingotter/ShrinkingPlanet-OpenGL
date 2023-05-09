#include "pch.h"
#include "VertexArray.h"


VertexArray::VertexArray()
{
	glGenVertexArrays(1, &mID);
	glBindVertexArray(mID);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &mID);
}

void VertexArray::AddBuffer(const VertexBuffer& vbo, const IndexBuffer& ibo, const BufferLayout& layout)
{
	Bind();
	vbo.Bind();
	ibo.Bind();

	const std::vector<BufferLayout::LayoutElement>& elements = layout.GetElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++)
	{
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(
			i,
			elements[i].count,
			elements[i].type,
			elements[i].normalized,
			layout.GetStride(),
			(const void*)offset
		);
		offset += (unsigned int)elements[i].count * BufferLayout::LayoutElement::GetSizeOfType(elements[i].type);

	}

}

void VertexArray::Bind() const
{
	glBindVertexArray(mID);
}

void VertexArray::Unbind() const
{
	glBindVertexArray(0);
}
