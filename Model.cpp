#include "Model.h"



TinyModel::TinyModel(std::filesystem::path dir) /* : Resource(ResourceType::MODEL, "TinyModel (Directory = " + dir.string())*/ {
	tinygltf::TinyGLTF gltf;
	tinygltf::Model model;
	std::string error;
	std::string warning;

	bool read = false;

	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(dir)) {
		std::filesystem::path p = entry.path();
		if (p.extension() == ".gltf") {
			gltf.LoadASCIIFromFile(&model, &error, &warning, p.string());
			read = true;
			break;
		}
	}

	if (!read) {
		std::cout << "TinyGLTF Error: Couldn't find GLTF file in directory (" << dir << ")" << std::endl;
	}

	if (!error.empty()) {
		std::cout << "TinyGLTF Error: " << error << std::endl;
	}

	if (!warning.empty()) {
		std::cout << "TinyGLTF Warning: " << warning << std::endl;
	}

	std::unordered_map<unsigned int, unsigned int> filterMap = { {TINYGLTF_TEXTURE_FILTER_LINEAR, GL_LINEAR},
		{TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR},
		{TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST },
		{TINYGLTF_TEXTURE_FILTER_NEAREST, GL_NEAREST},
		{TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR, GL_NEAREST_MIPMAP_LINEAR},
		{TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_NEAREST} };

	//Textures.reserve(model.textures.size()); // reserve to avoid moving texture when 

	Textures.reserve(model.textures.size());

	for (const tinygltf::Texture& tex : model.textures) {
		const tinygltf::Sampler& sampler = model.samplers.at(tex.sampler);
		//std::cout << "Model texture: " << (dir / model.images.at(tex.source).uri) << std::endl;

		// don't flip textures
		Textures.emplace_back(dir / model.images.at(tex.source).uri, filterMap.at(sampler.minFilter), filterMap.at(sampler.magFilter), true, false); // textures are causing extreme memory consumption, downscale them (maybe, not actually sure if textures)
	}

	for (const tinygltf::Scene& scene : model.scenes) {

		for (unsigned int nodeIndex : scene.nodes) {

			const tinygltf::Node& node = model.nodes.at(nodeIndex);
			const tinygltf::Mesh& mesh = model.meshes.at(node.mesh);

			Meshes.reserve(mesh.primitives.size());

			for (const tinygltf::Primitive& prim : mesh.primitives) {

				std::vector<glm::vec3> positions = GetMeshSubData<glm::vec3>(model, prim.attributes.at("POSITION"));
				std::vector<glm::vec3> normals = GetMeshSubData<glm::vec3>(model, prim.attributes.at("NORMAL"));
				std::vector<glm::vec4> tangents = GetMeshSubData<glm::vec4>(model, prim.attributes.at("TANGENT"));
				std::vector<glm::vec2> texCoords = GetMeshSubData<glm::vec2>(model, prim.attributes.at("TEXCOORD_0"));

				std::vector<unsigned int> indices = GetMeshSubData<unsigned short, unsigned int>(model, prim.indices);

				if (!(positions.size() == normals.size() && normals.size() == texCoords.size() && texCoords.size() == tangents.size())) {
					std::cout << "Possible GLTF Error: Variable size of vertex attributes" << std::endl;
				}

				std::vector<TinyVertex> vertices(positions.size());

				for (unsigned int i = 0; i < positions.size(); i++) {
					vertices[i] = { positions.at(i), normals.at(i), tangents.at(i), texCoords.at(i) };
				}

				const tinygltf::Material& mat = model.materials.at(prim.material);

				unsigned int normalTexIndex = mat.normalTexture.index;
				unsigned int albedoTexIndex = mat.pbrMetallicRoughness.baseColorTexture.index;
				unsigned int armTexIndex = mat.pbrMetallicRoughness.metallicRoughnessTexture.index;

				Meshes.emplace_back(vertices, indices, normalTexIndex, albedoTexIndex, armTexIndex);
			}
		}
	}

	glGenBuffers(1, &MatVBO);
}

void TinyModel::SetMats(const std::vector<glm::mat4>& mats) {

	std::cout << "TinyModel: Setting mats of size " << mats.size() << std::endl;

	glBindBuffer(GL_ARRAY_BUFFER, MatVBO);
	glBufferData(GL_ARRAY_BUFFER, mats.size() * sizeof(glm::mat4), mats.data(), GL_STATIC_DRAW);

	// model matrices for instancing
	std::size_t vec4Size = sizeof(glm::vec4);
	GLsizei mat4Size = 4 * vec4Size;

	for (TinyMesh& mesh : Meshes) {

		glBindVertexArray(mesh.GetVAO());

		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)0);

		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)(1 * vec4Size));

		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)(2 * vec4Size));

		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, mat4Size, (void*)(3 * vec4Size));

		glEnableVertexAttribArray(4);
		glEnableVertexAttribArray(5);
		glEnableVertexAttribArray(6);
		glEnableVertexAttribArray(7);

		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);

		glBindVertexArray(0);
	}
}

const std::vector<TinyMesh>& TinyModel::GetMeshes() const {
	return Meshes;
}

const std::vector<Texture>& TinyModel::GetTextures() const {
	return Textures;
}

TinyModel::~TinyModel() {

	if (IsDeprecated())
		return;

	glDeleteBuffers(1, &MatVBO);
}


Model::Model(std::filesystem::path dir) : Dir(dir) {

	Assimp::Importer importer;
	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(dir)) {
		std::filesystem::path p = entry.path();
		if (p.extension() == ".obj") {
			Scene = importer.ReadFile(p.generic_string(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals); // TODO change later
			break;
		}
	}

	if (Scene == NULL || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || Scene->mRootNode == NULL) {
		std::cout << "Assimp Error: " << importer.GetErrorString() << std::endl;
		return;
	}
	ProcessNode(Scene->mRootNode);
}

Model::Model(std::vector<Mesh> meshes) : Meshes(meshes) {

}

void Model::ProcessNode(const aiNode* node) {
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = Scene->mMeshes[node->mMeshes[i]];
		Meshes.push_back(ConvertMesh(mesh));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		std::cout << "Processed Node " << (i + 1) << std::endl;
		ProcessNode(node->mChildren[i]);
	}
}

Mesh Model::ConvertMesh(const aiMesh* mesh) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<MeshTexture> diffuses;
	std::vector<MeshTexture> speculars;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		glm::vec3 pos = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
		glm::vec3 norm = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
		glm::vec3 texCoord;
		if (mesh->mTextureCoords[0]) {
			texCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y, mesh->mTextureCoords[0][i].z };
		}
		else {
			texCoord = { 0.0f, 0.0f, 0.0f };
		}
		vertices.push_back({ pos, norm, texCoord });
	}
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = Scene->mMaterials[mesh->mMaterialIndex];
		std::vector<MeshTexture> newDiffuses = LoadMaterialTextures(material, aiTextureType_DIFFUSE);
		std::vector<MeshTexture> newSpeculars = LoadMaterialTextures(material, aiTextureType_SPECULAR);

		diffuses.insert(diffuses.end(), newDiffuses.begin(), newDiffuses.end());
		speculars.insert(speculars.end(), newSpeculars.begin(), newSpeculars.end());
	}
	return Mesh{ vertices, indices, diffuses, speculars };
}

std::vector<MeshTexture> Model::LoadMaterialTextures(const aiMaterial* mat, aiTextureType type) {
	std::vector<MeshTexture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString path;
		mat->GetTexture(type, i, &path);
		const char* pathCString = path.C_Str();
		bool unique = true;
		for (MeshTexture& tex : LoadedTextures) {
			if (tex.Path == pathCString) {
				textures.push_back(tex); // consider making model textures a global list and reference indices instead of copying the same texture to multiple spots
				unique = false;
				break;
			}
		}
		if (unique) {
			Texture texObj{ Dir / std::filesystem::path(pathCString) };
			MeshTexture tex{ texObj.GetID(), pathCString};
			textures.push_back(tex);
			LoadedTextures.push_back(tex);
		}
	}
	return textures;
}

const std::vector<Mesh>& Model::GetMeshes() const {
	return Meshes;
}