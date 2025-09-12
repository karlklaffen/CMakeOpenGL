//#include "InstancedModel.h"
//
//InstancedModel::InstancedModel(Resources::ModelName modelName, const std::vector<glm::mat4>& mats) : ModelName(modelName), ModelMats(mats), Forwarded(false) {
//	
//}
//
//void InstancedModel::ForwardMats() {
//	const TinyModel& source = Resources::GetResource<TinyModel>(ModelName, true);
//
//	Forwarded = Forwarded && source.IsLoaded();
//
//	if (!Forwarded) {
//		Resources::GetResource<TinyModel>(ModelName, true).SetMats(ModelMats);
//		Forwarded = true;
//	}
//}
//
//void InstancedModel::ForwardMats(const std::vector<glm::mat4>& mats) {
//	ModelMats = mats;
//
//	Forwarded = false;
//
//	ForwardMats();
//}
//
//Resources::ModelName InstancedModel::GetResource<Model>Name() const {
//	return ModelName;
//}
//
//const std::vector<glm::mat4>& InstancedModel::GetResource<Model>Mats() const {
//	return ModelMats;
//}