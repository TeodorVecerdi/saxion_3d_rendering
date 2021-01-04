#include "mge/core/Light.hpp"

#include "Camera.hpp"
#include "game/utils/constants.hpp"
#include "mge/core/World.hpp"

Light::Light(const std::string& name, const glm::vec3& position) : GameObject(name, position) {
	SetDiffuseColor(utils::constants::one);
	SetAttenuation(glm::vec3(1,1,0)); // linear falloff
	SetLightType(POINT);
}

Light::~Light() {
}

void Light::SetDirection(const glm::vec3 direction) {
	lightData.lightDirection = direction;
}

void Light::SetDiffuseColor(const glm::vec3 color) {
	lightData.lightColor = color;
}

void Light::SetAttenuation(const glm::vec3 attenuation) {
	lightData.attenuation = attenuation;
}

void Light::SetAttenuation(const float constant, const float linear, const float quadratic) {
	lightData.attenuation = glm::vec3(constant, linear, quadratic);
}

void Light::SetLightType(const LightType lightType) {
	lightData.lightType = lightType;
}

LightData Light::GetLightData() const {
	const auto worldPosition = getWorldPosition();
	
	auto copyLightData = lightData;
	copyLightData.lightPosition = worldPosition;
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
