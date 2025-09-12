#include "Application.h"
#include "CallbackManager.h"

Application::Application(std::string_view title, unsigned short w, unsigned short h, const std::filesystem::path& resourcesPath, unsigned short versionMajor, unsigned short versionMinor) :  DeltaTime(0), LastFrameTime(0), HasMovedMouse(false), CursorPos(0, 0), MouseSensitivity(0.01f), WindowWidth(w), WindowHeight(h), ActiveScene("NONE"), ActiveInterfaces({}), ActiveKernel("NONE"), SelectionColor(1.0f, 1.0f, 1.0f), ActiveScrollFuncName(""), FrameGroup(false),
Initializable("Application") {

	std::cout << "Initializing GLFW" << std::endl;
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, versionMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, versionMinor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	Window = glfwCreateWindow(w, h, title.data(), NULL, NULL);

	if (Window == nullptr) {
		std::cout << "ERROR" << std::endl;
		return;
	}

	glfwMakeContextCurrent(Window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "ERROR" << std::endl;
		return;
	}

	ResourceManager::SetPath(resourcesPath);
	Font::InitLibrary();

	TryLoadResources(ResourceType::SHADER, { "FRAME" });

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glEnable(GL_STENCIL_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE); // gl_keep, gl_replace, gl_replace

	FrameGroup.Gen();

	std::array<float, 16> frameVerts = VertexHelper::GetRectVerticesAndTextureCoords();
	std::array<unsigned int, 6> frameIndices = VertexHelper::GetRectIndices(0);

	FrameGroup.BindVAO();

	FrameGroup.BindVBO();
	FrameGroup.BufferVBO(frameVerts);

	FrameGroup.BindEBO();
	FrameGroup.BufferEBO(frameIndices);

	FrameGroup.AttribPointer<float>(0, 2, GLEnums::Type::FLOAT, 4);
	FrameGroup.AttribPointer<float>(1, 2, GLEnums::Type::FLOAT, 4);

	glGenFramebuffers(1, &FBO);
	glGenTextures(1, &FrameTextureID);
	glGenRenderbuffers(1, &RBO);

	UpdateFrame(WindowWidth, WindowHeight);

	if (!glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR: Framebuffer not completed" << std::endl;

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, FrameTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WindowWidth, WindowHeight, 0, GL_RGBA, GL_FLOAT, NULL); // hdr rendering

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindVertexArray(0);


#ifdef USE_INDIRECT
	// ibo test
	const Scene& scene = Scenes.at(ActiveScene);

	unsigned int instanceCount = 0;
	unsigned int firstIndex = 0;
	int baseVertex = 0;
	unsigned int baseInstance = 0;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	for (const InstancedModel& imodel : scene.GetResource<Model>s()) {

		instanceCount = static_cast<unsigned int>(imodel.GetResource<Model>Mats().size());

		for (const Mesh& mesh : imodel.GetResource<Model>().GetMeshes()) {

			unsigned int count = mesh.GetIndices().size();
			IndirectCommands.emplace_back(count, instanceCount, firstIndex, baseVertex, baseInstance);

			firstIndex += count;
			baseVertex += mesh.GetVertices().size();

			vertices.insert(vertices.end(), mesh.GetVertices().begin(), mesh.GetVertices().end());
			indices.insert(indices.end(), mesh.GetIndices().begin(), mesh.GetIndices().end());
		}
	}
	//std::cout << vertices.size() << " " << (scene.GetResource<Model>s()[0].GetResource<Model>().GetMeshes().size() * scene.GetResource<Model>s()[0].GetResource<Model>Mats().size()) << std::endl;

	glGenVertexArrays(1, &BigVAO);
	glBindVertexArray(BigVAO);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IBO);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawElementsIndirectCommand) * IndirectCommands.size(), IndirectCommands.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &BigVBO);
	glBindBuffer(GL_ARRAY_BUFFER, BigVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &BigEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BigEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Pos));

	// vertex normal vectors
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	// vertex texture coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoord));

	std::size_t vec4Size = sizeof(glm::vec4);

	glGenBuffers(1, &BigMBO);
	glBindBuffer(GL_ARRAY_BUFFER, BigMBO);
	glBufferData(GL_ARRAY_BUFFER, mats.size() * sizeof(glm::mat4), mats.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, (GLsizei)(4 * vec4Size), (void*)0);
	glVertexAttribDivisor(3, 1);

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, (GLsizei)(4 * vec4Size), (void*)(1 * vec4Size));
	glVertexAttribDivisor(4, 1);

	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, (GLsizei)(4 * vec4Size), (void*)(2 * vec4Size));
	glVertexAttribDivisor(5, 1);

	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, (GLsizei)(4 * vec4Size), (void*)(3 * vec4Size));
	glVertexAttribDivisor(6, 1);
#endif
	
	glBindVertexArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//ConfirmInitialization(); // TODO: We don't need this with exceptions

	//SceneInterfaceBatches.insert({ "EMPTY", {} });

	ConfirmInitialization();
}

Application::~Application() {

	// TODO: Consider making class that encapsulates VAO, VBO, and EBO

	ResourceManager::UnloadAll();
	Font::CleanLibrary();

	glDeleteFramebuffers(1, &FBO);
	glDeleteTextures(1, &FrameTextureID);
	glDeleteRenderbuffers(1, &RBO);

	std::cout << "Application: Ending Application (terminating)" << std::endl;

	glfwTerminate();
	std::cout << "Terminating GLFW" << std::endl;
}

bool Application::Continue() {
	if (glfwWindowShouldClose(Window))
		return false;

	UpdateTime();

	if (FieldSelectionInfo.has_value())
		FieldSelectionInfo.value().TryUpdateBlinkTime(DeltaTime);

	return true;
}

void Application::UpdateTime() {
	double curFrameTime = glfwGetTime();
	DeltaTime = curFrameTime - LastFrameTime;
	LastFrameTime = curFrameTime;
}

void Application::PollEvents() {
	glfwPollEvents();
	Functions.ExecuteQueuedFunctions();
}

GLFWwindow* Application::GetWindowPointer() {
	return Window;
}

bool Application::Render() const {

	// setup for rendering

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// render elements

	if (ActiveScene != "NONE" && RenderScene(ActiveScene))
		return true;
	
	for (std::string_view name: ActiveInterfaces) {
		if (RenderUI(name))
			return true;
	}

	// render frame texture on screen

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // consider only using framebuffer for scene
	glDisable(GL_DEPTH_TEST);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	const Shader& frameShader = ResourceManager::GetShaders().Get("FRAME");

	frameShader.Activate();

	if (ActiveKernel == "NONE") {
		frameShader.SetInt("KernelActive", 0);
	}
	else {
		frameShader.SetInt("KernelActive", 1);
		const std::array<float, 9>& kernel = Kernels.at(ActiveKernel);
		frameShader.SetFloatArray("Kernel", std::vector<float>(std::begin(kernel), std::end(kernel)));
	}

	FrameGroup.BindVAO();
	glBindTexture(GL_TEXTURE_2D, FrameTextureID);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// reset arrays
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// swap after rendering
	glfwSwapBuffers(Window);

	return false;

}

bool Application::RenderScene(std::string_view sceneName) const {

	const Scene& scene = ResourceManager::GetScenes().Get(ActiveScene);

	glStencilMask(0);

	glm::vec3 color = scene.GetBackgroundColor();

	glClearColor(color.r, color.g, color.b, 1.0f);

	const Camera& camera = scene.GetActiveCamera();

	glDepthMask(GL_FALSE);

	const Shader& skyboxShader = ResourceManager::GetShaders().Get("SKYBOX");
	skyboxShader.Activate();
	skyboxShader.SetMat4("Projection", camera.GetProjMat());

	glm::mat4 skyboxView = glm::mat4(glm::mat3(camera.GetViewMat())); // no translation for skybox, stay in one place
	skyboxShader.SetMat4("View", skyboxView);
	skyboxShader.SetInt("Skybox", 0);

	const Cubemap& skybox = ResourceManager::GetCubemaps().Get(scene.GetSkyboxNames().at(scene.GetSkyboxIndex()));

	glBindVertexArray(skybox.GetVAO());

	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.GetTextureID());
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glDepthMask(GL_TRUE);

	//const DirLight& dirLight = scene.GetDirLight();
	const std::vector<PointLight>& pointLights = scene.GetPointLights();

	glEnable(GL_DEPTH_TEST);

	const std::unordered_map<std::string, std::vector<glm::mat4>>& models = scene.GetInstancedModels();

	switch (scene.GetLightingType()) {

	case Scene::LightingType::BLINN_PHONG: {

		const Shader& shader = ResourceManager::GetShaders().Get("MODEL");

		shader.Activate();

		bool fog = scene.HasFog();

		// vertex

		shader.SetMat4("View", camera.GetViewMat());
		shader.SetMat4("Projection", camera.GetProjMat());

		shader.SetInt("Fog", fog);

		if (fog)
			shader.SetVec3("FogColor", color);

		shader.SetVec3("ViewPos", camera.GetPos());

		shader.SetFloat("Far", camera.GetFar());

		shader.SetUnsignedInt("NumPointLights", pointLights.size());

		for (int i = 0; i < pointLights.size(); i++) {
			std::string target = "PointLights[" + std::to_string(i) + "]";
			shader.SetVec3(target + ".Pos", pointLights[i].Pos);
			shader.SetVec3(target + ".Color", pointLights[i].Color);
		}

		//shader->SetInt("AlbedoTex", 0);
		shader.SetInt("DiffuseTex", 0);
		shader.SetInt("NormalTex", 1);

		for (const auto& [modelName, mats] : models) {

			const TinyModel& m = ResourceManager::GetModels().Get(modelName);
			std::size_t count = mats.size();

			unsigned int albedo = 0, normal = 0;

			for (const TinyMesh& mesh : m.GetMeshes()) {

				glBindVertexArray(mesh.GetVAO());

				const std::vector<Texture>& textures = m.GetTextures();

				unsigned int newAlbedo = textures.at(mesh.GetAlbedoTexIndex()).GetID();

				if (albedo == 0 || albedo != newAlbedo) {
					albedo = newAlbedo;
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, albedo);
				}

				unsigned int newNormal = textures.at(mesh.GetNormalTexIndex()).GetID();

				if (normal == 0 || normal != newNormal) {
					normal = newNormal;
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, normal);
				}

				glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(mesh.GetIndices().size()), GL_UNSIGNED_INT, 0, (GLsizei)count);

				//std::cout << "drew " << mesh.GetIndices().size() << " for " << count << std::endl;

				glActiveTexture(GL_TEXTURE0);
			}
		}
	} break;

	case Scene::LightingType::PBR: {

		const Shader& shader = ResourceManager::GetShaders().Get("PBR");

		shader.Activate();

		bool fog = scene.HasFog();
		shader.SetInt("Fog", fog);

		if (fog) {
			shader.SetVec3("FogColor", color);
		}

		/*shader->SetFloat("SelfMaterial.Shininess", 64.0f);

		shader->SetVec3("MainLight.Dir", dirLight.Dir);
		shader->SetVec3("MainLight.Ambient", dirLight.Ambient);
		shader->SetVec3("MainLight.Diffuse", dirLight.Diffuse);
		shader->SetVec3("MainLight.Specular", dirLight.Specular);*/

		shader.SetVec3("ViewPos", camera.GetPos());

		shader.SetMat4("Projection", camera.GetProjMat());
		shader.SetMat4("View", camera.GetViewMat());

		shader.SetFloat("Far", camera.GetFar());

		shader.SetInt("NumPointLights", pointLights.size());

		for (int i = 0; i < pointLights.size(); i++) {
			std::string target = "PointLights[" + std::to_string(i) + "]";
			shader.SetVec3(target + ".Pos", pointLights[i].Pos);

			/*shader->SetFloat(target + ".Constant", pointLights[i].Constant);
			shader->SetFloat(target + ".Linear", pointLights[i].Linear);
			shader->SetFloat(target + ".Quadratic", pointLights[i].Quadratic);

			shader->SetVec3(target + ".Ambient", pointLights[i].Ambient);
			shader->SetVec3(target + ".Diffuse", pointLights[i].Diffuse);
			shader->SetVec3(target + ".Specular", pointLights[i].Specular);*/
			shader.SetVec3(target + ".Color", glm::vec3(800.0, 800.0, 800.0));
		}

#ifdef USE_INDIRECT

		glBindVertexArray(BigVAO);

		glActiveTexture(GL_TEXTURE0);
		shader.SetInt("SelfMaterial.Diffuse", 0);
		glBindTexture(GL_TEXTURE_2D, diffuse);

		glActiveTexture(GL_TEXTURE1);
		shader.SetInt("SelfMaterial.Specular", 1);
		glBindTexture(GL_TEXTURE_2D, specular);

		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, IndirectCommands.size(), 0);

#else

		shader.SetInt("NormalTex", 0);
		shader.SetInt("AlbedoTex", 1);
		shader.SetInt("ARMTex", 2);

		for (const auto& [modelName, mats] : models) {

			//std::cout << mats << std::endl;

			const TinyModel& m = ResourceManager::GetModels().Get(modelName);
			std::size_t count = mats.size();

			/*unsigned int diffuse = m.GetMeshes().at(0).GetDiffuses().at(0).ID;
			unsigned int specular = m.GetMeshes().at(0).GetSpeculars().at(0).ID;*/

			// only draw first texture so far

			/*glActiveTexture(GL_TEXTURE0);
			shader.SetInt("SelfMaterial.Diffuse", 0);
			glBindTexture(GL_TEXTURE_2D, diffuse);

			glActiveTexture(GL_TEXTURE1);
			shader.SetInt("SelfMaterial.Specular", 1);
			glBindTexture(GL_TEXTURE_2D, specular);*/

			unsigned int normal = 0;
			unsigned int albedo = 0;
			unsigned int arm = 0;

			//const std::vector<glm::mat4>& mats = m.GetMats();

			const std::vector<TinyMesh>& meshes = m.GetMeshes();

			for (const TinyMesh& mesh : meshes) {

				glBindVertexArray(mesh.GetVAO());

				const std::vector<Texture>& textures = m.GetTextures();

				unsigned int newNormal = textures.at(mesh.GetNormalTexIndex()).GetID();
				unsigned int newAlbedo = textures.at(mesh.GetAlbedoTexIndex()).GetID();
				unsigned int newARM = textures.at(mesh.GetARMTexIndex()).GetID();


				if (normal == 0 || newNormal != normal) {
					normal = newNormal;
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, normal);
				}

				if (albedo == 0 || albedo != newAlbedo) {
					albedo = newAlbedo;
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, albedo);
				}

				if (arm == 0 || arm != newARM) {
					arm = newARM;
					glActiveTexture(GL_TEXTURE2);
					glBindTexture(GL_TEXTURE_2D, arm);
				}

				glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(mesh.GetIndices().size()), GL_UNSIGNED_INT, 0, (GLsizei)count);

				glActiveTexture(GL_TEXTURE0);
			}
		}
#endif
	} break;

	}

	const std::unordered_set<std::string>& outlinedObjects = scene.GetOutlinedObjectNames();

	if (!outlinedObjects.empty()) {
		const Shader& outlineShader = ResourceManager::GetShaders().Get("OUTLINE");

		outlineShader.Activate();
		outlineShader.SetVec4("Color", glm::vec4(SelectionColor, 1.0f));
		outlineShader.SetMat4("Projection", camera.GetProjMat());
		outlineShader.SetMat4("View", camera.GetViewMat());

		// draw outlined objects

		glDisable(GL_DEPTH_TEST);
		glStencilFunc(GL_NOTEQUAL, 1, UINT8_MAX); // only render when stencil bit is 0

		const std::unordered_map<std::string, Scene::SceneObject>& objects = scene.GetObjects();

		for (std::string_view name : outlinedObjects) {

			const Scene::SceneObject& obj = objects.at(std::string(name));

			const TinyModel& m = ResourceManager::GetModels().Get(obj.ModelName);

			const std::vector<TinyMesh>& meshes = m.GetMeshes();

			glStencilMask(UINT8_MAX); // enable writing to stencil buffer

			glClear(GL_STENCIL_BUFFER_BIT); // write all 0s to stencil buffer (only works after stencil mask command)

			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // ensure that nothing is actually drawn, only stencil buffer is written to

			outlineShader.SetMat4("Model", obj.Transform);
			outlineShader.SetFloat("Out", 0.0f);

			for (const TinyMesh& mesh : meshes) {
				glBindVertexArray(mesh.GetVAO());
				glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh.GetIndices().size()), GL_UNSIGNED_INT, 0);
			} // draw 1s on stencil where models are

			outlineShader.SetFloat("Out", 0.05f);

			glStencilMask(0); // disable writing to stencil buffer
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); // allow rendering

			for (const TinyMesh& mesh : meshes) {

				glBindVertexArray(mesh.GetVAO());

				glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(mesh.GetIndices().size()), GL_UNSIGNED_INT, 0);
			} // draw enlarged models (with only outlines being rendered)
		}

		glStencilMask(UINT8_MAX);
		glStencilFunc(GL_ALWAYS, 1, UINT8_MAX);
		glEnable(GL_DEPTH_TEST);
	}

	return false;
}

bool Application::RenderText(const Text& text) const {

	const Shader& shader = ResourceManager::GetShaders().Get("TEXT");

	const glm::mat4& proj = glm::ortho(0.0f, (float)WindowWidth, 0.0f, (float)WindowHeight);

	shader.Activate();
	shader.SetInt("Texture", 0);
	shader.SetVec3("TexColor", text.GetColor());

	shader.SetMat4("Projection", proj);

	glActiveTexture(GL_TEXTURE0);

	const Font& font = ResourceManager::GetFonts().Get(text.GetFontName());

	glBindVertexArray(font.GetVAO());

	const std::map<char, Glyph>& glyphs = font.GetGlyphs();

	std::string_view str = text.GetString();

	for (unsigned int i = 0; i < str.size(); i++) {

		if (str.at(i) == '\n')
			continue;

		Glyph g = glyphs.at(str.at(i));

		std::array<float, 16> vertices = VertexHelper::GetRectVerticesAndTextureCoords(text.GetCharRects().at(i), true);

		glBindTexture(GL_TEXTURE_2D, g.TextureID);

		// update buffers

		glBindBuffer(GL_ARRAY_BUFFER, font.GetVBO());
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vertices.size(), vertices.data());

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// draw

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	return false;
}

bool Application::RenderUI(std::string_view interfaceName) const {

	glDisable(GL_DEPTH_TEST);

	const Shader& uiShader = ResourceManager::GetShaders().Get("UI");
	uiShader.Activate();
	uiShader.SetInt("Texture", 0);
	uiShader.SetMat4("Projection", glm::ortho(0.0f, (float)WindowWidth, 0.0f, (float)WindowHeight));

	const Interface& interface = ResourceManager::GetInterfaces().Get(interfaceName);

	std::string_view bgTexName = interface.GetBackgroundTexName();

	if (!bgTexName.empty()) {
		
		glBindVertexArray(interface.GetBackgroundVAO());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ResourceManager::GetTextures().Get(bgTexName).GetID());
		
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	std::string_view wgTexName = interface.GetWidgetTexName();

	if (!wgTexName.empty()) {

		glActiveTexture(GL_TEXTURE0);

		glBindVertexArray(interface.GetWidgetVAO());

		glBindTexture(GL_TEXTURE_2D, ResourceManager::GetTextures().Get(wgTexName).GetID());
		glDrawElements(GL_TRIANGLES, interface.GetWidgets().size() * 6, GL_UNSIGNED_INT, 0);

	}

	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(0);

	for (const auto& [name, text] : interface.GetTexts()) {

		if (RenderText(text))
			return true;
	}

	if (FieldSelectionInfo.has_value()) {

		const SelectedFieldInfo& info = FieldSelectionInfo.value();

		if (info.IsCursorRendered()) {
			const Interface& interface = ResourceManager::GetInterfaces().Get(info.GetInterfaceName());
			const Interface::Field& field = interface.GetFields().at(std::string(info.GetFieldName()));

			const Shader& cursorShader = ResourceManager::GetShaders().Get("COLOR");

			cursorShader.Activate();
			cursorShader.SetMat4("Projection", glm::ortho(0.0f, (float)WindowWidth, 0.0f, (float)WindowHeight));
			cursorShader.SetVec4("Color", glm::vec4(interface.GetTexts().at(field.TextRef).GetColor(), 1.0));

			glBindVertexArray(info.GetCursorVAO());
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}

	glEnable(GL_DEPTH_TEST);

	return false;
}

void Application::FramebufferSizeCallback(int width, int height) {
	glViewport(0, 0, width, height);
	WindowWidth = width;
	WindowHeight = height;

	std::cout << "Resized screen to " << width << " x " << height << std::endl;

	if (ActiveScene != "NONE")
		ResourceManager::GetScenes().Get(ActiveScene).Resize(WindowWidth, WindowHeight);

	for (std::string_view name : ActiveInterfaces)
		ResourceManager::GetInterfaces().Get(name).Resize(WindowWidth, WindowHeight);

	UpdateFrame(WindowWidth, WindowHeight);

}

void Application::UpdateFrame(unsigned int w, unsigned int h) {
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glBindTexture(GL_TEXTURE_2D, FrameTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FrameTextureID, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
}

void Application::KeyCallback(GLEnums::Key key, GLEnums::KeyAction action, int scancode, int mods) {

	bool charEnter = action == GLEnums::KeyAction::PRESS || action == GLEnums::KeyAction::REPEAT;

	if (charEnter && FieldSelectionInfo.has_value()) {
		SelectedFieldInfo& info = FieldSelectionInfo.value();
		Interface& interface = ResourceManager::GetInterfaces().Get(info.GetInterfaceName());
		Interface::Field& field = interface.GetFields().at(std::string(info.GetFieldName()));

		switch (key) {
		case GLEnums::Key::LEFT:
			info.TryMoveCursor(Utils::Direction::LEFT);
			// TODO: Make custom struct that contains simple VAO, VBO, EBO structure
			break;
		case GLEnums::Key::RIGHT:
			info.TryMoveCursor(Utils::Direction::RIGHT);
			break;
		case GLEnums::Key::UP:
			info.TryMoveCursor(Utils::Direction::UP);
			break;
		case GLEnums::Key::DOWN:
			info.TryMoveCursor(Utils::Direction::DOWN);
			break;
		case GLEnums::Key::BACKSPACE:
			info.TryRemoveChar(true);
			break;
		case GLEnums::Key::DEL:
			info.TryRemoveChar(false);
			break;
		case GLEnums::Key::ENTER:
			info.TryAddChar('\n');
			break;
		}
		return;
	}

	if (action == GLEnums::KeyAction::REPEAT)
		return;

	bool press = action == GLEnums::KeyAction::PRESS;

	unsigned int uKey = static_cast<unsigned int>(key);

	Keys[uKey] = press;

	std::cout << "Key Callback: #" << uKey << (press ? " pressed" : " released") << std::endl;

	std::unordered_map<GLEnums::Key, std::string>::iterator it = KeyButtonFuncMaps.find(key);

	if (it != KeyButtonFuncMaps.end()) {
		std::string_view funcName = it->second;
		std::cout << "Key Callback: Trying to execute function with id " << funcName << std::endl;
		Functions.QueueButtonFunctionExecution(funcName, press);
	}
}

void Application::CursorPosCallback(double xPos, double yPos) {

	CursorOffset = { xPos - CursorPos.x, yPos - CursorPos.y };

	CursorPos = { xPos, yPos };

	if (GetCursorMode() == GLEnums::CursorMode::NORMAL) {
		UpdateInterfaceWidgets();
	}
}

void Application::ScrollCallback(double xOffset, double yOffset) {

	if (ActiveScrollFuncName != "")
		Functions.QueueScrollFunctionExecution(ActiveScrollFuncName, xOffset, yOffset);
}

void Application::MouseButtonCallback(GLEnums::MouseButton button, bool press, int mods) {

	Mouse[static_cast<unsigned int>(button)] = press;

	std::unordered_map<GLEnums::MouseButton, std::string>::iterator it = MouseButtonFuncMaps.find(button);

	if (it != MouseButtonFuncMaps.end()) {
		std::string_view funcName = it->second;
		Functions.QueueButtonFunctionExecution(funcName, press);
	}

	if (GetCursorMode() == GLEnums::CursorMode::NORMAL) {

		UpdateInterfaceWidgets();

		if (press) {

			for (std::string_view interfaceName : ActiveInterfaces) {

				const Interface& interface = ResourceManager::GetInterfaces().Get(interfaceName);

				for (const auto& [fieldName, field] : interface.GetFields()) {

					const Text& text = interface.GetTexts().at(field.TextRef);

					if (Utils::PointInRect(CursorPos, text.GetBoundingBox())) {

						SelectField(interfaceName, fieldName, CursorPos);
						goto afterCheckFields;
					}
				}
			}
			DeselectField();
		}

	afterCheckFields:;
	}

	// events:
	// hover (unpressed) -> typically execute hover action
	// press (hovered) -> typically execute press action
	// unpress (hovered) -> typically execute unpress action
	// unpress (unhovered) -> no special action, just set state to unhovered
	// unhover (unpressed) -> typically execute unhover action
}

void Application::CharCallback(char character) {

	if (FieldSelectionInfo.has_value()) {

		SelectedFieldInfo& info = FieldSelectionInfo.value();

		info.TryAddChar(character);
	}
}

bool Application::KeyHeld(GLEnums::Key key) {
	return Keys.at(static_cast<unsigned int>(key));
}

bool Application::MouseButtonHeld(GLEnums::MouseButton button) {
	return Mouse.at(static_cast<unsigned int>(button));
}

void Application::UpdateInterfaceWidgets() {

	for (std::string_view name : ActiveInterfaces) {

		Interface& interface = ResourceManager::GetInterfaces().Get(name);

		if (interface.GetMode() == GLEnums::CursorMode::DISABLED)
			continue;

		std::vector<Interface::WidgetFuncData> data = interface.UpdateWidgets(CursorPos.x, CursorPos.y, MouseButtonHeld(GLEnums::MouseButton::LEFT));
		for (const Interface::WidgetFuncData& widgetData : data) {

			Functions.QueueWidgetFunctionExecution(interface.GetWidgets().at(widgetData.WidgetName).FuncName, widgetData);
		}
	}
}

bool Application::SelectField(std::string_view interfaceName, std::string_view fieldName, glm::uvec2 cursorPos) {

	if (!FieldSelectionInfo.has_value()) {
		FieldSelectionInfo.emplace();
	}

	FieldSelectionInfo.value().SelectField(interfaceName, fieldName, cursorPos);
	return false;
}

bool Application::DeselectField() {
	FieldSelectionInfo.reset();
	return false;
}

bool Application::TryLoadResources(ResourceType type, const std::unordered_set<std::string>& names) {
	if (!ResourceManager::SmartEnumerateSpecific(type, names, ResourceManager::LoadType::LOAD)) {
		LoadedResources.TryAddMultiple(type, names);
		return false;
	}
	return true;
}

bool Application::TryUnloadResources(ResourceType type, const std::unordered_set<std::string>& names) {
	if (!ResourceManager::SmartEnumerateSpecific(type, names, ResourceManager::LoadType::UNLOAD)) {
		LoadedResources.TryRemoveMultiple(type, names);
		return false;
	}
	return true;
}

bool Application::TryReloadAll() {
	return ResourceManager::SmartEnumerate(LoadedResources, ResourceManager::LoadType::RELOAD);
}

bool Application::TryReloadAllOfType(ResourceType type) {
	return ResourceManager::SmartEnumerate(LoadedResources, ResourceManager::LoadType::LOAD, type);
}

bool Application::TryMakeLoadedOnly(const SetBatch<ResourceType>& batch) {
	if (!ResourceManager::SmartUnloadLoad(batch, LoadedResources)) {
		LoadedResources = batch;
		return false;
	}
	return true;
}

bool Application::SwapScene(std::string_view name) {
	if (ActiveScene == name)
		return true;

	if (!ResourceManager::GetScenes().IsLoaded(name)) {
		std::cout << "tried to swap to not loaded scene " << name << std::endl;
		return true;
	}

	/*if (ErrorManager::Assert(SceneInterfaceBatches.at(LoadedSceneInterfaceBatch).SceneNames.contains(name), std::format("Swapped-to scene ({}) not yet loaded", name)))
		return true;*/
		
	ActiveScene = name;
	ResourceManager::GetScenes().Get(ActiveScene).ConfigureModelMats();
	ResourceManager::GetScenes().Get(ActiveScene).Resize(WindowWidth, WindowHeight);

	return false;
}

bool Application::EnableInterface(std::string_view name) {
	if (ActiveInterfaces.contains(std::string(name)))
		return true;

	/*if (ErrorManager::Assert(SceneInterfaceBatches.at(LoadedSceneInterfaceBatch).InterfaceNames.contains(name), std::format("Trying to enable not-loaded interface ({})", name)))
		return true;*/
	
	ActiveInterfaces.insert(std::string(name));

	// Rule for mouse cursor hiding/activation:
	// If at least one Interface exists that has NORMAL as mode, cursor is normal
	// Else, if at least one Interface exists that has HIDDEN as mode, cursor is hidden
	// Last priority is DISABLED
	// Therefore, all active interfaces need to have DISABLED as mode to not have cursor active (like FPS camera)
	// This way, normal GUI elements can still appear on background even with temp menus open (like Pause) and pause menu can be functional
	// We could consider having a NEUTRAl or NO_MODE option that allows for either cursor active or no cursor active, but this way is easier

	
	glfwSetInputMode(Window, GLFW_CURSOR, static_cast<int>(GetPriorityMode(ActiveInterfaces)));

	ResourceManager::GetInterfaces().Get(name).Resize(WindowWidth, WindowHeight);

	UpdateInterfaceWidgets();

	return false;
}

bool Application::EnableInterfaces(const std::unordered_set<std::string>& names) {
	bool bad = false;
	for (std::string_view name : names) {
		if (!EnableInterface(name))
			bad = true;
	}

	return bad;
}

bool Application::InterfaceEnabled(std::string_view name) {
	return ActiveInterfaces.contains(std::string(name));
}

bool Application::InterfacesEnabled(const std::unordered_set<std::string>& names) {
	for (std::string_view name : names) {
		if (!InterfaceEnabled(name))
			return false;
	}

	return true;
}

bool Application::SwapInterfaces(const std::unordered_set<std::string>& first, const std::unordered_set<std::string>& second) {
	
	bool firstEnabled = InterfacesEnabled(first);
	bool secondEnabled = InterfacesEnabled(second);
	std::cout << firstEnabled << " " << secondEnabled << std::endl;
	
	// goal of function is to disable one group of interfaces and enable another based on which one is already active
	// so one needs should be enabled and the other needs to be disabled to begin with so they can switch
	// otherwise, what are we doing?

	if (firstEnabled && !secondEnabled) {
		std::cout << "Swapping Interfaces: " << first << " -> " << second << std::endl;
		DisableInterfaces(first);
		EnableInterfaces(second);
	}
	else if (secondEnabled && !firstEnabled) {
		std::cout << "Swapped Interfaces: " << second << " -> " << first << std::endl;
		DisableInterfaces(second);
		EnableInterfaces(first);
	}
	else {
		//ErrorManager::Assert(false, "Some of swapped interfaces already enabled or disabled");
		return true;
	}

	return false;
}

GLEnums::CursorMode Application::GetPriorityMode(const std::unordered_set<std::string>& interfaceNames) {
	bool hidden = false, disabled = false;

	for (std::string_view name : interfaceNames) {
		const Interface& interface = ResourceManager::GetInterfaces().Get(name);
		switch (interface.GetMode()) {
		case GLEnums::CursorMode::NORMAL:
			return GLEnums::CursorMode::NORMAL; // normal takes priority
		case GLEnums::CursorMode::HIDDEN:
			hidden = true;
			break;
		case GLEnums::CursorMode::DISABLED:
			disabled = true;
			break;
		}
	}

	return hidden ? GLEnums::CursorMode::HIDDEN : GLEnums::CursorMode::DISABLED;
}

bool Application::DisableInterface(std::string_view name) {

	ActiveInterfaces.erase(std::string(name));
	ResourceManager::GetInterfaces().Get(name).ResetWidgetStates();

	return false;
}

bool Application::DisableInterfaces(const std::unordered_set<std::string>& names) {
	bool bad = false;
	for (std::string_view name : names) {
		if (DisableInterface(name))
			bad = true;
	}

	return bad;
}

void Application::AddWidgetFunction(std::string_view name, const std::function<void(const Interface::WidgetFuncData& data)>& func) {
	Functions.CacheWidgetFunction(name, func);
}

void Application::AddScrollFunction(std::string_view name, const std::function<void(double xOffset, double yOffset)>& func) {
	Functions.CacheScrollFunction(name, func);
}

void Application::BindScrollFunction(std::string_view name) {
	ActiveScrollFuncName = name;
}

void Application::AddButtonFunction(std::string_view name, const std::function<void(bool press)>& func) {
	Functions.CacheButtonFunction(name, func);
}

void Application::BindKeyButtonFunction(GLEnums::Key key, std::string_view name) {
	KeyButtonFuncMaps.try_emplace(key, name);
}

void Application::BindMouseButtonFunction(GLEnums::MouseButton button, std::string_view name) {
	MouseButtonFuncMaps.try_emplace(button, name);
}

void Application::SetCursorMode(GLEnums::CursorMode mode) {
	glfwSetInputMode(Window, GLFW_CURSOR, static_cast<unsigned int>(mode));
}

GLEnums::CursorMode Application::GetCursorMode() {
	return (GLEnums::CursorMode)glfwGetInputMode(Window, GLFW_CURSOR);
}

std::string_view Application::GetActiveSceneName() {
	return ActiveScene;
}

float Application::GetDeltaTime() const {
	return DeltaTime;
}

const glm::vec2& Application::GetCursorOffset() const {
	return CursorOffset;
}

unsigned int Application::GetWindowWidth() const {
	return WindowWidth;
}

unsigned int Application::GetWindowHeight() const {
	return WindowHeight;
}

void Application::Stop() {
	glfwSetWindowShouldClose(Window, true);
}