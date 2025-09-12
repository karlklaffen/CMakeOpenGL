#include "Texture.h"
#include "ResourceManager.h"

Texture::Texture(const std::filesystem::path& path, unsigned int minFilter, unsigned int magFilter, bool compressed, bool flipped) /* : Resource("Texture (Path = " + path.string() + ", Min Filter = " + std::to_string(minFilter) + ", Mag Filter = " + std::to_string(magFilter) + ", Compressed = " + std::to_string(compressed) + ")")*/ {
	int width, height, numChannels;
	unsigned char* data;
	//float borderColor[]{ 0.0f, 0.0f, 0.0f, 1.0f };

	unsigned int id;
	//glEnable(GL_DEPTH_TEST);
	glGenTextures(1, &id);

	if (flipped)
		stbi_set_flip_vertically_on_load(true);

	std::cout << "Texture: Starting Texture Loading" << std::endl;
	data = stbi_load(path.string().c_str(), &width, &height, &numChannels, 0);
	std::cout << "Texture: Ending Texture Loading" << std::endl;

	if (flipped)
		stbi_set_flip_vertically_on_load(false);

	bool init = false;

	if (data) {
		GLenum format = 0, internalFormat = 0;
		switch (numChannels) {
		case 3:
			format = GL_RGB;
			internalFormat = compressed ? GL_COMPRESSED_RGB : GL_RGB8;
			break;
		case 4:
			format = GL_RGBA;
			internalFormat = compressed ? GL_COMPRESSED_SRGB_ALPHA : GL_RGBA8;
			break;
		}
		if (format == 0) {
			std::cout << "Texture Error: Unrecognized image mapping (numChannels = " << numChannels << ")" << std::endl;
		}
		else {
			glBindTexture(GL_TEXTURE_2D, id);
			//std::cout << "tex image 2d" << std::endl;
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			std::cout << "Texture: Generating mipmap" << std::endl;
			glGenerateMipmap(GL_TEXTURE_2D);
			std::cout << "Texture: Stopping generating mipmap" << std::endl;

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

			////glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
			//std::cout << "Texture: Loaded texture (" << path << ")" << std::endl;
			init = true;
		}
	}

	if (!init) {
		std::cout << "Texture Error: Failed to load texture (" << path << ")" << std::endl;
	}

	stbi_image_free(data);
	ID = id;
	Width = width;
	Height = height;
}

unsigned int Texture::GetID() const {
	return ID;
}

unsigned int Texture::GetWidth() const {
	return Width;
}

unsigned int Texture::GetHeight() const {
	return Height;
}

//Texture::Texture(Texture& original) : Width(original.Width), Height(original.Height), ID(original.ID) {
//	original.Deprecate();
//}
//
//Texture::Texture(Texture&& original) noexcept : Width(original.Width), Height(original.Height), ID(original.ID) {
//	original.Deprecate();
//}

Texture::~Texture() {

	if (IsDeprecated())
		return;

	//std::cout << "deleting texture " << ID << std::endl;

	glDeleteTextures(1, &ID);
}