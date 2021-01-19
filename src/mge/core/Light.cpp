#include "mge/core/Light.hpp"

#include "Camera.hpp"
#include "game/utils/constants.hpp"
#include "mge/core/World.hpp"

Light::Light(const std::string& name, const glm::vec3& position) : GameObject(name, position) {
	SetDiffuseColor(utils::constants::one);
	SetDiffuseIntensity(1.0f);
	SetAmbientColor(utils::constants::one);
	SetAmbientIntensity(0.1f);
	SetSpecularColor(utils::constants::one);
	SetSpecularIntensity(0.5f);
	SetAttenuation(glm::vec3(1,0.1,0.02)); // linear falloff
	SetLightType(POINT);
}

Light::~Light() {
}

void Light::SetDirection(const glm::vec3 direction) {
	lightData.direction = direction;
}

void Light::SetDiffuseColor(const glm::vec3 color) {
	lightData.diffuse.x = color.x;
	lightData.diffuse.y = color.y;
	lightData.diffuse.z = color.z;
}

void Light::SetDiffuseIntensity(float intensity) {
	lightData.diffuse.w = intensity;
}

void Light::SetAmbientColor(glm::vec3 color) {
	lightData.ambient.x = color.x;
	lightData.ambient.y = color.y;
	lightData.ambient.z = color.z;
}

void Light::SetAmbientIntensity(float intensity) {
	lightData.ambient.w = intensity;
}

void Light::SetSpecularColor(glm::vec3 color) {
	lightData.specular.x = color.x;
	lightData.specular.y = color.y;
	lightData.specular.z = color.z;
}

void Light::SetSpecularIntensity(float intensity) {
	lightData.specular.w = intensity;
}

void Light::SetAttenuation(const glm::vec3 attenuation) {
	lightData.attenuation = attenuation;
}

void Light::SetAttenuation(const float constant, const float linear, const float quadratic) {
	lightData.attenuation = glm::vec3(constant, linear, quadratic);
}

void Light::SetLightType(const LightType lightType) {
	lightData.type = lightType;
}

void Light::SetOuterAngle(float outerAngle) {
	lightData.outerCutOff = glm::cos(outerAngle);
}

void Light::SetInnerAngle(float innerAngle) {
	lightData.innerCutOff = glm::cos(innerAngle);
}

LightData Light::GetLightData() const {
	const auto worldPosition = GetWorldPosition();
	
	auto copyLightData = lightData;
	copyLightData.position = worldPosition;
	return copyLightData;
}

//Override setWorldRecursively to make sure we are registered
//to the world we are a part of.
void Light::_setWorldRecursively(World* world) {

	//store the previous and new world before and after storing the given world
	World* previousWorld = _world;
	GameObject::_setWorldRecursively(world);
	World* newWorld = _world;

	//check whether we need to register or unregister
	if (previousWorld != nullptr) previousWorld->unregisterLight(this);
	if (newWorld != nullptr) newWorld->registerLight(this);
}
