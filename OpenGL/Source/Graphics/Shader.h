#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>


class Shader
{
public:
	Shader(const std::string& filePath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	// manage uniforms
private:
	int GetUniformLocation(const std::string& name);
public:
	void SetUniform(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniform(const std::string& name, int value);
	void SetUniform(const std::string& name, float value);
	void SetUniform(const std::string& name, const glm::vec3& vector);
	void SetUniform(const std::string& name, const glm::mat4& matrix);

private:
	struct ShaderSource
	{
		std::string vs;
		std::string ps;
	};
	ShaderSource Parse();
	unsigned int Compile(const std::string& shader, unsigned int type);
	void CreateProgram(const std::string& vertexShader, const std::string& pixelShader);

private:
	unsigned int mID;
	std::string mFilePath;
	ShaderSource mSources;
	std::unordered_map<std::string, int> mUniformCache;
};