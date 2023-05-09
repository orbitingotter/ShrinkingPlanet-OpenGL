#pragma once

#include <GL/glew.h>


class BufferLayout
{
public:
	class LayoutElement
	{
	public:
		int count;
		unsigned int type;
		bool normalized;

		static unsigned int GetSizeOfType(unsigned int type)
		{
			switch (type)
			{
			case GL_FLOAT:			return sizeof(float);
			case GL_UNSIGNED_INT:	return sizeof(unsigned int);
			case GL_UNSIGNED_BYTE:	return sizeof(unsigned char);
			}

			std::cout << "ERROR : Invalid type size" << std::endl;
			return 0;
		}

	};

public:
	BufferLayout() : mStride(0) {}

	template<typename T>
	void Push(int count)
	{
		std::cout << "ERROR : Invalid layout type pushed" << std::endl;
	}

	template<>
	void Push<float>(int count)
	{
		mElements.push_back({ count, GL_FLOAT, false });
		mStride += count *	LayoutElement::GetSizeOfType(GL_FLOAT);
	}

	template<>
	void Push<unsigned int>(int count)
	{
		mElements.push_back({ count, GL_UNSIGNED_INT, false });
		mStride += count * LayoutElement::GetSizeOfType(GL_UNSIGNED_INT);
	}

	template<>
	void Push<unsigned char>(int count)
	{
		mElements.push_back({ count, GL_UNSIGNED_BYTE, true });
		mStride += count * LayoutElement::GetSizeOfType(GL_UNSIGNED_BYTE);
	}

	unsigned int GetStride() const{ return mStride; }
	const std::vector<LayoutElement>& GetElements() const { return mElements; }

private:
	std::vector<LayoutElement> mElements;
	unsigned int mStride;
};