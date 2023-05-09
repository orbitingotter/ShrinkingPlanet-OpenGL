#include "pch.h"
#include "Shader.h"

Shader::Shader(const std::string& filePath)
	: mFilePath(filePath), mID(0)
{
	mSources = Parse();
	CreateProgram(mSources.vs, mSources.ps);
	Bind();
}

Shader::~Shader()
{
	glDeleteProgram(mID);
}

Shader::ShaderSource Shader::Parse()
{
	enum class ShaderType
	{
		Invalid = -1, Vertex, Pixel
	};

	std::ifstream file;
	file.open(mFilePath);
	if (!file.is_open())
	{
		std::cout << "ERROR : File ( " << mFilePath << " ) could not be opened\n";
		return {};
	}

	std::string line;
	ShaderType mode = ShaderType::Invalid;
	std::string sources[2];
	while (getline(file, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				mode = ShaderType::Vertex;
			else if (line.find("pixel") != std::string::npos)
				mode = ShaderType::Pixel;
			else
				mode = ShaderType::Invalid;
		}
		else
		{
			line += '\n';
			sources[(int)mode] += line;
		}
	}

	file.close();
	return { sources[(int)ShaderType::Vertex], sources[(int)ShaderType::Pixel] };
}

unsigned int Shader::Compile(const std::string& shader, unsigned int type)
{
	unsigned int shaderID = glCreateShader(type);
	const char* source = shader.c_str();
	glShaderSource(shaderID, 1, &source, nullptr);
	glCompileShader(shaderID);

	// Error Handling
	int result;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
		char* message = new char[length];
		glGetShaderInfoLog(shaderID, length, &length, message);

		std::cout << "ERROR : Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "pixel") << "shader.\n";
		std::cout << message << std::endl;
		return 0;
	}

	return shaderID;
}

void Shader::CreateProgram(const std::string& vertexShader, const std::string& pixelShader)
{
	mID = glCreateProgram();
	unsigned int vs = Compile(vertexShader, GL_VERTEX_SHADER);
	unsigned int ps = Compile(pixelShader, GL_FRAGMENT_SHADER);

	glAttachShader(mID, vs);
	glAttachShader(mID, ps);
	glLinkProgram(mID);
	glValidateProgram(mID);

	glDeleteShader(vs);
	glDeleteShader(ps);
}


void Shader::Bind() const
{
	glUseProgram(mID);
}

void Shader::Unbind() const
{
	glUseProgram(0);
}


// manage uniforms

int Shader::GetUniformLocation(const std::string& name)
{
	if (mUniformCache.find(name) != mUniformCache.end())
		return mUniformCache[name];

	int location = glGetUniformLocation(mID, name.c_str());
	if (location == -1)
	{
		std::cout << "WARNING : Uniform \"" << name << "\" does not exist in shader " << mFilePath << std::endl;
 	}

	mUniformCache[name] = location;

	return location;
}

void Shader::SetUniform(const std::string& name, float v0, float v1, float v2, float v3)
{
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniform(const std::string& name, int value)
{
	glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform(const std::string& name, float value)
{
	glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetUniform(const std::string& name, const glm::vec3& vector)
{
	glUniform3f(GetUniformLocation(name), vector.x, vector.y, vector.z);
}

void Shader::SetUniform(const std::string& name, const glm::mat4& matrix)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}
