#include "pch.h"
#include "Texture2D.h"


#include <GL/glew.h>
#include "stb_image/stb_image.h"

Texture2D::Texture2D(const std::string& filePath, const std::string& type)
	: mID(0), mFilePath(filePath), mType(type), mImageBuffer(nullptr), mWidth(0), mHeight(0), mChannels(0)
{
	stbi_set_flip_vertically_on_load(1);
	mImageBuffer = stbi_load(filePath.c_str(), &mWidth, &mHeight, &mChannels, 4);

	glGenTextures(1, &mID);
	glBindTexture(GL_TEXTURE_2D, mID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 16.0f);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, mImageBuffer);
	glGenerateMipmap(GL_TEXTURE_2D);

	// unbind to later bind specific slot
	Unbind();

	if (mImageBuffer)
	{
		stbi_image_free(mImageBuffer);
	}
	else
	{
		std::cout << "ERROR : Texture ( " << filePath << " ) not loaded - " << stbi_failure_reason() << std::endl;
	}
}

Texture2D::~Texture2D()
{
	//glDeleteTextures(1, &mID);
}

void Texture2D::Bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, mID);
}

void Texture2D::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
