#include "pch.h"
#include "Renderer.h"

#include <GLFW/glfw3.h>

static void APIENTRY MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	std::cout << std::endl;

	// OpenGL Source
	std::cout << "Source: ";
	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Other"; break;
	}
	std::cout << std::endl;

	// Debug Message
	std::cout << "Message: " << message << std::endl;

	// Error Type
	std::cout << "Type: ";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Performance"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Other"; break;
	}

	// Error ID
	std::cout << std::endl;
	std::cout << "Id: " << id << std::endl;

	// Error Severity
	std::cout << "Severity: ";
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:
		std::cout << "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		std::cout << "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		std::cout << "HIGH";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		std::cout << "NOTIFICATION";
		break;
	}
	std::cout << std::endl;

	// Backtrack error location
#ifdef _DEBUG
	__debugbreak();
#endif
}


Renderer::Renderer()
{
	// Enables debug output
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(MessageCallback, 0);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);

	// Enables blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enables depth test
	glEnable(GL_DEPTH_TEST);

	// Enables back culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void Renderer::Clear(float r, float g, float b, float a) const
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(r, g, b, a);
}

void Renderer::SetDepthTest(bool enabled)
{
	if (enabled)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}

void Renderer::Draw(const VertexArray& vao, int count, const Shader& shader) const
{
	vao.Bind();
	shader.Bind();
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
}

void Renderer::Draw(Mesh& mesh, Shader& shader)
{
	shader.Bind();
	mesh.BindTextures(shader);
	mesh.Bind();
	glDrawElements(GL_TRIANGLES, mesh.GetIndexCount(), GL_UNSIGNED_INT, nullptr);
}

void Renderer::Draw(Model& model, Shader& shader)
{
	auto& meshes = model.GetMeshes();
	for (unsigned int i = 0; i < meshes.size(); i++)
		Draw(meshes[i], shader);
}

void Renderer::Draw(Cubemap& cubemap, Shader& shader)
{
	cubemap.Bind();
	shader.Bind();
	glDrawElements(GL_TRIANGLES, cubemap.GetIndexCount(), GL_UNSIGNED_INT, nullptr);
}
