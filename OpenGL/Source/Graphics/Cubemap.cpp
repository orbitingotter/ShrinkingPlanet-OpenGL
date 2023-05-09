#include "pch.h"
#include "Cubemap.h"

#include <GL/glew.h>
#include "Vendor/stb_image/stb_image.h"

Cubemap::Cubemap(const std::vector<std::string>& facesFilepath)
{
	// setup cube positions
	mPositions.reserve(3 * 8);
	mIndices.reserve(6 * 6);

	mPositions =
	{
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, +1.0f,
		-1.0f, +1.0f, -1.0f,
		-1.0f, +1.0f, +1.0f,
		+1.0f, -1.0f, -1.0f,
		+1.0f, -1.0f, +1.0f,
		+1.0f, +1.0f, -1.0f,
		+1.0f, +1.0f, +1.0f,
	};

	mIndices =
	{
		1,0,2,	2,3,1,
		4,5,7,	7,6,4,
		0,4,6,	6,2,0,
		5,1,3,  3,7,5,
		2,6,7,	7,3,2,
		1,5,4,	1,4,0
	};

	mVAO = std::make_unique<VertexArray>();
	mVBO = std::make_unique<VertexBuffer>(mPositions.data(), mPositions.size() * sizeof(float));
	mIBO = std::make_unique<IndexBuffer>(mIndices.data(), mIndices.size());
	BufferLayout layout;
	layout.Push<float>(3);
	mVAO->AddBuffer(*mVBO, *mIBO, layout);

	// create textures
	glGenTextures(1, &mID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mID);

	int width, height, channels;
	for (unsigned int i = 0; i < facesFilepath.size(); i++)
	{
		stbi_set_flip_vertically_on_load(false);
		unsigned char* data = stbi_load(facesFilepath[i].c_str(), &width, &height, &channels, 0);

		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "ERROR : Cubemap texture " << facesFilepath[i] << " failed to load\n";
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

}

Cubemap::~Cubemap()
{
	glDeleteTextures(1, &mID);
}

void Cubemap::Bind()
{
	mVAO->Bind();
	glBindTexture(GL_TEXTURE_CUBE_MAP, mID);
	glActiveTexture(GL_TEXTURE0);
}
