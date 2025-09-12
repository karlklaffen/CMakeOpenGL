#define USE_TINY_GLTF
#define GLM_ENABLE_EXPERIMENTAL

//#define MULTITHREAD_TEXTURES

#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "stb_image.h"
#include "tiny_gltf.h"

#include <iostream>
#include <vector>
#include <filesystem>
#include <string>
#include <array>
#include <algorithm>
#include <thread>

#include "Mesh.h"
#include "NotCopyable.h"

class TinyModel : public NotCopyable {

public:
	TinyModel(std::filesystem::path dir);

	TinyModel(const TinyModel& original) = default;
	TinyModel(TinyModel&& original) = default;

	const std::vector<TinyMesh>& GetMeshes() const;
	const std::vector<Texture>& GetTextures() const;

	void SetMats(const std::vector<glm::mat4>& mats); // only sets opengl texture config, not actual variable

	~TinyModel();

private:

	template<typename T>
	std::vector<T> GetMeshSubData(const tinygltf::Model& model, unsigned int accessorIndex) {
		const tinygltf::Accessor& accessor = model.accessors.at(accessorIndex);
		const tinygltf::BufferView& bufferView = model.bufferViews.at(accessor.bufferView);
		const tinygltf::Buffer& buffer = model.buffers.at(bufferView.buffer);

		std::vector<T> target(accessor.count);

		std::memcpy(&target[0], &buffer.data[bufferView.byteOffset], bufferView.byteLength);

		if (target.empty())
			std::cout << "Possible GLTF Error: Empty list of vertex attributes" << std::endl;

		return target;
	}

	template<typename T, typename U>
	std::vector<U> GetMeshSubData(const tinygltf::Model& model, unsigned int accessorIndex) {

		std::vector<T> data = GetMeshSubData<T>(model, accessorIndex);

		return { data.begin(), data.end() };
	}

	unsigned int MatVBO;

	std::vector<TinyMesh> Meshes;
	std::vector<Texture> Textures;
	//std::vector<glm::mat4> Mats;
};


class Model {

public:
	Model(std::filesystem::path dir);
	Model(std::vector<Mesh> meshes);

	const std::vector<Mesh>& GetMeshes() const;

private:
	std::vector<Mesh> Meshes;
	std::filesystem::path Dir;
	const aiScene* Scene;

	void ProcessNode(const aiNode* node);
	Mesh ConvertMesh(const aiMesh* mesh);
	std::vector<MeshTexture> LoadMaterialTextures(const aiMaterial* mat, aiTextureType type);

	std::vector<MeshTexture> LoadedTextures;

};