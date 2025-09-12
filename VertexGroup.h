#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>
#include <array>

#include "GLEnums.h"
#include "NotCopyable.h"

class VertexGroup : public NotCopyable {

public:

	VertexGroup(bool init = true);
	~VertexGroup();

	bool Gen();
	bool Delete();

	unsigned int GetVAO();
	unsigned int GetVBO();
	unsigned int GetEBO();

	// binding
	void BindVAO() const;
	void BindVBO() const;
	void BindEBO() const;

	// buffer vbo

	template<typename T>
	void BufferVBO(const std::vector<T>& data, bool dynamic = false) {
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	}

	template<typename T, size_t N>
	void BufferVBO(const std::array<T, N>& data, bool dynamic = false) {
		glBufferData(GL_ARRAY_BUFFER, sizeof(data), data.data(), dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	}

	template<typename T>
	void BufferVBO(T* data, size_t size, bool dynamic = false) {
		glBufferData(GL_ARRAY_BUFFER, size * sizeof(T), data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	}

	// buffer sub vbo

	template<typename T>
	void BufferSubVBO(unsigned int numOffset, const std::vector<T>& data) {
		glBufferSubData(GL_ARRAY_BUFFER, numOffset * sizeof(T), data.size() * sizeof(T), data.data());
	}

	template<typename T, size_t N>
	void BufferSubVBO(unsigned int numOffset, const std::array<T, N>& data) {
		glBufferSubData(GL_ARRAY_BUFFER, numOffset * sizeof(T), data.size() * sizeof(T), data.data());
	}

	template<typename T>
	void BufferSubVBO(unsigned int numOffset, T* data, size_t size) {
		glBufferSubData(GL_ARRAY_BUFFER, numOffset * sizeof(T), size * sizeof(T), data);
	}

	// buffer sub ebo

	template<typename T>
	void BufferEBO(const std::vector<T>& data, bool dynamic = false) {
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	}

	template<typename T, size_t N>
	void BufferEBO(const std::array<T, N>& data, bool dynamic = false) {
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(data), data.data(), dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	}

	template<typename T>
	void BufferEBO(T* data, size_t size, bool dynamic = false) {
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(T), data, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	}

	// attrib

	template<typename T>
	void AttribPointer(unsigned int step, unsigned int size, GLEnums::Type type, unsigned int totalSize, bool normalize = false, bool enable = true) {
		if (enable)
			glEnableVertexAttribArray(step);

		size_t typeSize = sizeof(T);
		glVertexAttribPointer(step, size, static_cast<unsigned int>(type), normalize ? GL_TRUE : GL_FALSE, totalSize * typeSize, (void*)(step * size * typeSize));
	}

	/*template<typename T>
	struct AttribStep {
		GLEnums::Type Type;
		unsigned int Size;
		bool Normalize = false;
		bool Enable = true;
	};

	template<typename T>
	void AttribAll(const std::vector<AttribStep>& steps) {
		unsigned int totalSize = 0;
		auto size = sizeof(T);

		for (const AttribStep& step : steps)
			totalSize += step.Size;

		for (unsigned int i = 0; i < steps.size(); i++) {
			const AttribStep& step = steps.at(i);
			glVertexAttribPointer(i, step.Size, static_cast<unsigned int>(step.Type), step.Normalize ? GL_TRUE : GL_FALSE, totalSize * sizeof(T), (void*)(i * step.Size * sizeof(T)));

			if (enable)
				glEnableVertexAttribArray(i);
		}
	}*/

	// special

	

private:

	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

	bool Generated;
};

