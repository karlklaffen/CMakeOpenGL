#include "Cubemap.h"

Cubemap::Cubemap(std::filesystem::path dir) { /* : Resource(ResourceType::CUBEMAP, "Directory = " + dir.string())*/
	std::array<std::filesystem::path, 6> faceTextures = { dir / "right.jpg", dir / "left.jpg", dir / "top.jpg", dir / "bottom.jpg", dir / "front.jpg", dir / "back.jpg" };

	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureID);

	int width, height, numChannels;
	unsigned char* data;

	stbi_set_flip_vertically_on_load(false);

	bool notFullyLoaded = false;

	for (unsigned int i = 0; i < faceTextures.size(); i++) {
		data = stbi_load(faceTextures[i].string().c_str(), &width, &height, &numChannels, 0);

		if (data) {
			GLenum mapping = 0;
			switch (numChannels) {
			case 3:
				mapping = GL_RGB;
				break;
			case 4:
				mapping = GL_RGBA;
				break;
			}
			if (mapping == 0) {
				std::cout << "Cubemap Error: Unrecognized image mapping (numChannels = " << numChannels << ")" << std::endl;
				notFullyLoaded = true;
			}
			else {
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, mapping, GL_UNSIGNED_BYTE, data);
			}
		}
		stbi_image_free(data);
	}

	stbi_set_flip_vertically_on_load(true);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	std::array<float, 24> vertices = VertexHelper::GetCubemapVertices();
	std::array<unsigned int, 36> indices = VertexHelper::GetCubemapIndices();

	/*float vertices[] {
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
	}

	float indices[] {
		2, 0, 4, 4, 6, 2,
		1, 0, 2, 2, 3, 1,
		4, 5, 7, 7, 6, 4,
		1, 3, 7, 7, 5, 1,
		2, 6, 7, 7, 3, 2,
		0, 1, 4, 4, 1, 5
	}*/

	//std::array<float, 108> vertices = { // TODO: use EBO with custom VertexHelper cubemap function instead of all this
	//	// positions          
	//	-1.0f,  1.0f, -1.0f,
	//	-1.0f, -1.0f, -1.0f,
	//	 1.0f, -1.0f, -1.0f,
	//	 1.0f, -1.0f, -1.0f,
	//	 1.0f,  1.0f, -1.0f,
	//	-1.0f,  1.0f, -1.0f,

	//	-1.0f, -1.0f,  1.0f,
	//	-1.0f, -1.0f, -1.0f,
	//	-1.0f,  1.0f, -1.0f,
	//	-1.0f,  1.0f, -1.0f,
	//	-1.0f,  1.0f,  1.0f,
	//	-1.0f, -1.0f,  1.0f,

	//	 1.0f, -1.0f, -1.0f,
	//	 1.0f, -1.0f,  1.0f,
	//	 1.0f,  1.0f,  1.0f,
	//	 1.0f,  1.0f,  1.0f,
	//	 1.0f,  1.0f, -1.0f,
	//	 1.0f, -1.0f, -1.0f,

	//	-1.0f, -1.0f,  1.0f,
	//	-1.0f,  1.0f,  1.0f,
	//	 1.0f,  1.0f,  1.0f,
	//	 1.0f,  1.0f,  1.0f,
	//	 1.0f, -1.0f,  1.0f,
	//	-1.0f, -1.0f,  1.0f,

	//	-1.0f,  1.0f, -1.0f,
	//	 1.0f,  1.0f, -1.0f,
	//	 1.0f,  1.0f,  1.0f,
	//	 1.0f,  1.0f,  1.0f,
	//	-1.0f,  1.0f,  1.0f,
	//	-1.0f,  1.0f, -1.0f,

	//	-1.0f, -1.0f, -1.0f,
	//	-1.0f, -1.0f,  1.0f,
	//	 1.0f, -1.0f, -1.0f,
	//	 1.0f, -1.0f, -1.0f,
	//	-1.0f, -1.0f,  1.0f,
	//	 1.0f, -1.0f,  1.0f
	//};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	glBindVertexArray(0);

	/*float d[108];
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &d);
	std::cout << d << std::endl;*/

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/*Cubemap::Cubemap(Cubemap& original) : TextureID(original.TextureID), VAO(original.VAO), VBO(original.VBO), EBO(original.EBO), Resource(original) {
	
}

Cubemap::Cubemap(Cubemap&& original) noexcept : TextureID(original.TextureID), VAO(original.VAO), VBO(original.VBO), EBO(original.EBO), Resource(original) {
	
}*/

unsigned int Cubemap::GetVAO() const {
	return VAO;
}

unsigned int Cubemap::GetVBO() const {
	return VBO;
}

unsigned int Cubemap::GetEBO() const {
	return EBO;
}

unsigned int Cubemap::GetTextureID() const {
	return TextureID;
}

Cubemap::~Cubemap() {
	if (IsDeprecated())
		return;

	glDeleteTextures(1, &TextureID);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}