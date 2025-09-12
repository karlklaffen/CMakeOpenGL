#include "Mesh.h"

TinyMesh::TinyMesh(const std::vector<TinyVertex>& vertices, const std::vector<unsigned int>& indices, const unsigned int normalTexIndex, unsigned int albedoTexIndex, unsigned int ARMTexIndex) : Vertices(vertices), Indices(indices), NormalTexIndex(normalTexIndex), AlbedoTexIndex(albedoTexIndex), ARMTexIndex(ARMTexIndex) {
//Resource(ResourceType::MESH, "# Vertices = " + std::to_string(vertices.size()) + ", # Indices = " + std::to_string(indices.size()) + ", Normal Tex Index = " + std::to_string(normalTexIndex) + ", Albedo Tex Index = " + std::to_string(albedoTexIndex) + ", ARM Tex Index = " + std::to_string(ARMTexIndex)) {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TinyVertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// vertex positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TinyVertex), (void*)offsetof(TinyVertex, Pos));

	// vertex normal vectors
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TinyVertex), (void*)offsetof(TinyVertex, Normal));

	// vertex tangent vectors
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(TinyVertex), (void*)offsetof(TinyVertex, Tangent));

	// vertex texture coordinates
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(TinyVertex), (void*)offsetof(TinyVertex, TexCoord));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);

}

//TinyMesh::TinyMesh(TinyMesh& original) : Vertices(original.Vertices), Indices(original.Indices), NormalTexIndex(original.NormalTexIndex), AlbedoTexIndex(original.AlbedoTexIndex), ARMTexIndex(original.ARMTexIndex), VAO(original.VAO), VBO(original.VBO), EBO(original.EBO) {
//	original.Deprecate();
//}
//
//TinyMesh::TinyMesh(TinyMesh&& original) noexcept : Vertices(original.Vertices), Indices(original.Indices), NormalTexIndex(original.NormalTexIndex), AlbedoTexIndex(original.AlbedoTexIndex), ARMTexIndex(original.ARMTexIndex), VAO(original.VAO), VBO(original.VBO), EBO(original.EBO) {
//	original.Deprecate();
//}

const std::vector<TinyVertex>& TinyMesh::GetVertices() const {
	return Vertices;
}

const std::vector<unsigned int>& TinyMesh::GetIndices() const {
	return Indices;
}

unsigned int TinyMesh::GetNormalTexIndex() const {
	return NormalTexIndex;
}

unsigned int TinyMesh::GetAlbedoTexIndex() const {
	return AlbedoTexIndex;
}

unsigned int TinyMesh::GetARMTexIndex() const {
	return ARMTexIndex;
}

const unsigned int TinyMesh::GetVAO() const {
	//std::cout << "VBO = " << VBO << std::endl;
	//std::cout << "EBO = " << EBO << std::endl;
	return VAO;
}

TinyMesh::~TinyMesh() {

	if (IsDeprecated())
		return;

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}


Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<MeshTexture>& diffuses, const std::vector<MeshTexture>& speculars) : Vertices(vertices), Indices(indices), Diffuses(diffuses), Speculars(speculars) {
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), Vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(unsigned int), Indices.data(), GL_STATIC_DRAW);
	
	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Pos));

	// vertex normal vectors
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	// vertex texture coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoord));
}

const std::vector<Vertex>& Mesh::GetVertices() const {
	return Vertices;
}

const std::vector<unsigned int>& Mesh::GetIndices() const {
	return Indices;
}

const std::vector<MeshTexture>& Mesh::GetDiffuses() const {
	return Diffuses;
}

const std::vector<MeshTexture>& Mesh::GetSpeculars() const {
	return Speculars;
}

const unsigned int Mesh::GetVAO() const {
	return VAO;
}