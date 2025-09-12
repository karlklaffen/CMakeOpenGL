#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "NotCopyable.h"

class Shader : public NotCopyable {
public:
	Shader(std::filesystem::path vertexPath, std::filesystem::path fragmentPath);
	Shader(const Shader& original) = default;
	Shader(Shader&& original) = default;

	void Activate() const;

	unsigned int GetID() const;

	void SetUnsignedInt(const std::string& name, unsigned int val) const;
	void SetInt(const std::string& name, int val) const;
	void SetIntArray(const std::string& name, const std::vector<int>& v) const;
	void SetFloat(const std::string& name, float val) const;
	void SetFloatArray(const std::string& name, const std::vector<float>& v) const;
	void SetVec3(const std::string& name, const glm::vec3& v) const;
	void SetVec4(const std::string& name, const glm::vec4& v) const;
	void SetMat4(const std::string& name, const glm::mat4& transform) const;

	~Shader();

private:

	unsigned int ID;

};