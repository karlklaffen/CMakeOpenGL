#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <vector>
#include <unordered_map>
#include <filesystem>
#include <array>
#include <algorithm>
#include <optional>

#include <glm/glm.hpp>
#include <glm/gtx/intersect.hpp>

#include "Lights.h"
#include "Camera.h"
#include "Cubemap.h"
#include "NotCopyable.h"
#include "EntityComponents.h"

class Scene : public NotCopyable {

public:

	enum class LightingType {
		BLINN_PHONG,
		PBR
	};

	struct MeshPick {
		std::string Name;
		unsigned int MeshIndex;
	};

	struct SceneObject {
		std::string ModelName;
		glm::mat4 Transform;
	};

	Scene(LightingType lighting, bool fog, glm::vec3 background, const std::vector<PointLight>& pointLights, const std::vector<Camera>& cameras, const std::unordered_map<std::string, SceneObject>& objects, const std::vector<std::string>& skyboxes);

	Scene(const Scene& original) = default;
	Scene(Scene&& original) = default;

	// send mat data to model resources
	bool ConfigureModelMats();
	bool Resize(unsigned int w, unsigned int h);

	Camera& GetActiveCamera();
	const Camera& GetActiveCamera() const;

	std::vector<Camera>& GetCameras();
	const std::vector<Camera>& GetCameras() const;

	const std::unordered_map<std::string, SceneObject>& GetObjects() const;

	const std::vector<PointLight>& GetPointLights() const;

	LightingType GetLightingType() const;

	bool HasFog() const;
	glm::vec3 GetBackgroundColor() const;

	const std::vector<std::string>& GetSkyboxNames() const;
	unsigned int GetSkyboxIndex() const;

	void SetPointLight(unsigned int i, const PointLight& light);

	std::optional<MeshPick> TryMousePick(glm::vec3 pos, glm::vec3 dir) const;

	bool OutlineObject(std::string_view name);
	bool UnoutlineObject(std::string_view name);
	bool ToggleObjectOutline(std::string_view name);
	bool ObjectOutlined(std::string_view name);

	const std::unordered_set<std::string>& GetOutlinedObjectNames() const;

	void MoveCameraBasedOnCursor(double xOffset, double yOffset);

	std::unordered_map<std::string, std::vector<glm::mat4>> GetInstancedModels() const;

private:

	//unsigned int GetSelectedModelIndex(InstancePick pick) const;

	LightingType LightingMethod;

	std::vector<Camera> Cameras;
	unsigned int CameraIndex;

	std::vector<std::string> SkyboxNames;
	unsigned int SkyboxIndex;

	std::unordered_map<std::string, SceneObject> Objects;
	std::unordered_set<std::string> OutlinedObjectNames;

	std::vector<PointLight> PointLights;

	bool Fog;
	glm::vec3 BackgroundColor; // fog color

	EntityManager ObjectManager;
};

