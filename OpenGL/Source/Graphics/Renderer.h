#pragma once

#include <GL/glew.h>

#include "VertexArray.h"
#include "Shader.h"
#include "Mesh.h"
#include "Model.h"
#include "Cubemap.h"

class Renderer
{
public:
	Renderer();

	void Clear(float r, float g, float b, float a) const;
	void SetDepthTest(bool enabled);

public:
	void Draw(const VertexArray& vao, int count, const Shader& shader) const;
	void Draw(Mesh& mesh, Shader& shader);
	void Draw(Model& model, Shader& shader);
	void Draw(Cubemap& cubemap, Shader& shader);
};