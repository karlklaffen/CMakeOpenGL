#include "Shader.h"

Shader::Shader(std::filesystem::path vertexPath, std::filesystem::path fragmentPath) /* : Resource(ResourceType::SHADER, "Vertex Path = " + vertexPath.string() + ", Fragment Path = " + fragmentPath.string())*/ {
	std::ifstream vertexFile(vertexPath);

	if (!vertexFile.is_open()) {
		std::cout << "Shader Error: Failed to open file " << vertexPath << " for shader" << std::endl;
	}

	std::ifstream fragmentFile(fragmentPath);

	if (!fragmentFile.is_open()) {
		std::cout << "Shader Error: Failed to open file " << fragmentPath << " for shader" << std::endl;
	}

	std::stringstream vertexStream, fragmentStream;
	vertexStream << vertexFile.rdbuf();
	fragmentStream << fragmentFile.rdbuf();

	vertexFile.close();
	fragmentFile.close();

	std::string vertexCode = vertexStream.str();
	std::string fragmentCode = fragmentStream.str();

	int success;
	char info[512];

	const char* vertexCodeChars = vertexCode.c_str();
	const char* fragmentCodeChars = fragmentCode.c_str();

	unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexCodeChars, NULL);
	glCompileShader(vertex);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, info);
		std::cout << "Shader Error: Vertex Shader Compilation Failed:\n" << info << std::endl;
	}

	unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentCodeChars, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, info);
		std::cout << "Shader Error: Fragment Shader Compilation Failed:\n" << info << std::endl;
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, info);
		std::cout << "Shader Error: Shader Program Linking Failed:\n" << info << std::endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);

}

//Shader::Shader(Shader& original) : ID(original.ID) {
//	original.Deprecate();
//}
//
//Shader::Shader(Shader&& original) noexcept : ID(original.ID) {
//	original.Deprecate();
//}

void Shader::Activate() const {
	glUseProgram(ID);
}

unsigned int Shader::GetID() const {
	return ID;
}

void Shader::SetUnsignedInt(const std::string& name, unsigned int val) const {
	glUniform1ui(glGetUniformLocation(ID, name.c_str()), val);
}

void Shader::SetInt(const std::string &name, int val) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), val);
}

void Shader::SetIntArray(const std::string& name, const std::vector<int>& v) const {
	glUniform1iv(glGetUniformLocation(ID, name.c_str()), v.size(), v.data());
}

void Shader::SetFloat(const std::string& name, float val) const {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), val);
}

void Shader::SetFloatArray(const std::string& name, const std::vector<float>& v) const {
	glUniform1fv(glGetUniformLocation(ID, name.c_str()), v.size(), v.data());
}

void Shader::SetVec3(const std::string& name, const glm::vec3& v) const {
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(v));
}

void Shader::SetVec4(const std::string& name, const glm::vec4& v) const {
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(v));
}

void Shader::SetMat4(const std::string& name, const glm::mat4& transform) const {
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(transform));
}

Shader::~Shader() {
	if (IsDeprecated())
		return;

	glDeleteProgram(ID);
}