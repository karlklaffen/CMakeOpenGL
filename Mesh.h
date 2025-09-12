#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <filesystem>

#include "Texture.h"
#include "NotCopyable.h"

struct TinyVertex {
	glm::vec3 Pos;
	glm::vec3 Normal;
	glm::vec4 Tangent;
	glm::vec2 TexCoord;
};

class TinyMesh : public NotCopyable {

public:
	TinyMesh(const std::vector<TinyVertex>& vertices, const std::vector<unsigned int>& indices, unsigned int normalTex, unsigned int albedoTex, unsigned int armTex);

	TinyMesh(const TinyMesh& original) = default;
	TinyMesh(TinyMesh&& original) = default;

	const std::vector<TinyVertex>& GetVertices() const;
	const std::vector<unsigned int>& GetIndices() const;
	unsigned int GetNormalTexIndex() const;
	unsigned int GetAlbedoTexIndex() const;
	unsigned int GetARMTexIndex() const; // ambient occlusion, roughness, metallic
	const unsigned int GetVAO() const;

	~TinyMesh();

private:
	std::vector<TinyVertex> Vertices;
	std::vector<unsigned int> Indices;
	const unsigned int NormalTexIndex;
	const unsigned int AlbedoTexIndex;
	const unsigned int ARMTexIndex;
	unsigned int VAO, VBO, EBO;
};

struct Vertex {
	glm::vec3 Pos;
	glm::vec3 Normal;
	glm::vec2 TexCoord;
};

struct MeshTexture {
	unsigned int ID;
	std::filesystem::path Path;
};

class Mesh {

public:
	Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<MeshTexture>& diffuses, const std::vector<MeshTexture>& speculars);

	const std::vector<Vertex>& GetVertices() const;
	const std::vector<unsigned int>& GetIndices() const;
	const std::vector<MeshTexture>& GetDiffuses() const;
	const std::vector<MeshTexture>& GetSpeculars() const;
	const unsigned int GetVAO() const;

private:
	std::vector<Vertex> Vertices;
	std::vector<unsigned int> Indices;
	std::vector<MeshTexture> Diffuses;
	std::vector<MeshTexture> Speculars;
	unsigned int VAO, VBO, EBO;
};