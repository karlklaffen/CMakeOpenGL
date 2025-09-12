#include "VertexGroup.h"

VertexGroup::VertexGroup(bool init) : Generated(false) {
	if (init)
		Gen();
}

VertexGroup::~VertexGroup() {
	Delete();
}

bool VertexGroup::Gen() {
	if (Generated)
		return true;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return false;
}

bool VertexGroup::Delete() {
	if (!Generated)
		return true;

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	return false;
}

unsigned int VertexGroup::GetVAO() {
	return VAO;
}

unsigned int VertexGroup::GetVBO() {
	return VBO;
}

unsigned int VertexGroup::GetEBO() {
	return EBO;
}

void VertexGroup::BindVAO() const {
	glBindVertexArray(VAO);
}

void VertexGroup::BindVBO() const {
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
}

void VertexGroup::BindEBO() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
}