#include "Scene.h"
#include "ResourceManager.h"

Scene::Scene(LightingType lighting, bool fog, glm::vec3 background, const std::vector<PointLight>& pointLights, const std::vector<Camera>& cameras, const std::unordered_map<std::string, SceneObject>& objects, const std::vector<std::string>& skyboxes) : LightingMethod(lighting),/*, MainLight(mainLight), */PointLights(pointLights), Cameras(cameras), CameraIndex(0), BackgroundColor(background), Fog(fog), Objects(objects), SkyboxNames(skyboxes), SkyboxIndex(0)
{
	
}

bool Scene::ConfigureModelMats() {

	for (const auto& [modelName, mats] : GetInstancedModels())
		ResourceManager::GetModels().Get(modelName).SetMats(mats);

	return false;
}

bool Scene::Resize(unsigned int w, unsigned int h) {
	for (Camera& cam : Cameras) {
		cam.SetAspect(w / (float)h);
		cam.UpdateProj();
	}

	return false;
}

std::vector<Camera>& Scene::GetCameras() {
	return Cameras;
}

const std::vector<Camera>& Scene::GetCameras() const {
	return Cameras;
}

Camera& Scene::GetActiveCamera() {
	return Cameras.at(CameraIndex);
}

const Camera& Scene::GetActiveCamera() const {
	return Cameras.at(CameraIndex);
}

const std::unordered_map<std::string, Scene::SceneObject>& Scene::GetObjects() const {
	return Objects;
}

const std::vector<PointLight>& Scene::GetPointLights() const {
	return PointLights;
}

Scene::LightingType Scene::GetLightingType() const {
	return LightingMethod;
}

bool Scene::HasFog() const {
	return Fog;
}

glm::vec3 Scene::GetBackgroundColor() const {
	return BackgroundColor;
}

const std::vector<std::string>& Scene::GetSkyboxNames() const {
	return SkyboxNames;
}

unsigned int Scene::GetSkyboxIndex() const {
	return SkyboxIndex;
}

void Scene::SetPointLight(unsigned int i, const PointLight& light) {
	PointLights[i] = light;
}

bool Scene::OutlineObject(std::string_view name) {

	if (OutlinedObjectNames.contains(std::string(name)))
		return true;

	OutlinedObjectNames.insert(std::string(name));

	return false;
}

bool Scene::UnoutlineObject(std::string_view name) {

	if (!OutlinedObjectNames.contains(std::string(name)))
		return true;

	OutlinedObjectNames.erase(std::string(name));

	return false;
}

bool Scene::ToggleObjectOutline(std::string_view name) {
	auto it = OutlinedObjectNames.find(std::string(name));

	if (it == OutlinedObjectNames.end())
		OutlinedObjectNames.insert(std::string(name));
	else
		OutlinedObjectNames.erase(it);

	return false;
}

bool Scene::ObjectOutlined(std::string_view name) {
	return OutlinedObjectNames.contains(std::string(name));
}

const std::unordered_set<std::string>& Scene::GetOutlinedObjectNames() const {
	return OutlinedObjectNames;
}

void Scene::MoveCameraBasedOnCursor(double xOffset, double yOffset) {
	Camera& camera = GetActiveCamera();
	double newYaw = camera.GetYaw() + xOffset;
	double newPitch = camera.GetPitch() + yOffset;

	if (newPitch < glm::radians(-90.0f))
		newPitch = glm::radians(-89.99f);
	else if (newPitch > glm::radians(90.0f))
		newPitch = glm::radians(89.99f);

	camera.SetYawPitch((float)newYaw, (float)newPitch);
	camera.UpdateView();
}

std::unordered_map<std::string, std::vector<glm::mat4>> Scene::GetInstancedModels() const {
	std::unordered_map<std::string, std::vector<glm::mat4>> instancedModels;

	for (const auto& [name, object] : Objects) {

		auto it = instancedModels.find(object.ModelName);

		if (it == instancedModels.end()) {
			instancedModels.try_emplace(object.ModelName, std::vector{ object.Transform });
			std::cout << "NEW: " << object.ModelName << std::endl;
		}
		else
			it->second.push_back(object.Transform);
	}

	std::cout << "found " << instancedModels.size() << std::endl;
	return instancedModels;
}

std::optional<Scene::MeshPick> Scene::TryMousePick(glm::vec3 pos, glm::vec3 dir) const {

	float leastDist = -1;

	for (const auto& [name, object] : Objects) {

		const TinyModel& model = ResourceManager::GetModels().Get(object.ModelName);

		const std::vector<TinyMesh>& meshes = model.GetMeshes();

		for (unsigned int k = 0; k < meshes.size(); k++) {
			const std::vector<TinyVertex>& vertices = meshes[k].GetVertices();
			const std::vector<unsigned int>& indices = meshes[k].GetIndices();
			for (unsigned int i = 0; i < indices.size(); i += 3) {
				glm::vec3 v0 = object.Transform * glm::vec4(vertices[indices[i]].Pos, 1.0f);
				glm::vec3 v1 = object.Transform * glm::vec4(vertices[indices[i + 1]].Pos, 1.0f);
				glm::vec3 v2 = object.Transform * glm::vec4(vertices[indices[i + 2]].Pos, 1.0f);

				glm::vec2 res;
				float dist;

				if (glm::intersectRayTriangle(pos, dir, v0, v1, v2, res, dist)) {
					if (dist < leastDist || leastDist == -1) {
						leastDist = dist;
						return MeshPick{ name, k };
					}
				}
			}
		}
	}

	return {};
}

//bool Scene::OutlineModel(InstancePick index) {
//	SelectedObjectNames
//}

//bool Scene::UnoutlineModel(InstancePick index) {
//	unsigned int i = GetSelectedModelIndex(index);
//	if (i == SelectedModelIndices.size()) {
//		return true;
//	}
//	SelectedModelIndices.erase(SelectedModelIndices.begin() + i);
//	return false;
//}
//
//bool Scene::ToggleModelOutline(InstancePick index) {
//	unsigned int i = GetSelectedModelIndex(index);
//	if (i == SelectedModelIndices.size()) {
//		SelectedModelIndices.push_back(index);
//		return true;
//	}
//	SelectedModelIndices.erase(SelectedModelIndices.begin() + i);
//	return false;
//}
//
//const std::vector<Scene::InstancePick>& Scene::GetSelectedModelIndices() const {
//	return SelectedModelIndices;
//}
//
//bool Scene::ModelSelected(InstancePick pick) const {
//	return GetSelectedModelIndex(pick) != SelectedModelIndices.size();
//}
//
//unsigned int Scene::GetSelectedModelIndex(InstancePick pick) const {
//	for (unsigned int i = 0; i < SelectedModelIndices.size(); i++) {
//		if (SelectedModelIndices.at(i) == pick)
//			return i;
//	}
//
//	return SelectedModelIndices.size();
//}