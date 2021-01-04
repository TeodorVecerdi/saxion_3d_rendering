#pragma once

#include "glm.hpp"
#include "mge/core/GameObject.hpp"

enum LightType: uint32_t {
	DIRECTIONAL = 0,
	POINT = 1,
	SPOTLIGHT = 2
};

struct LightData {
	LightType lightType;
	glm::vec3 lightPosition;
	glm::vec3 lightDirection;
	glm::vec3 lightColor;
	glm::vec3 attenuation; // x = const, y = linear, z = quadratic
};

/**
* Exercise for the student: implement the Light class...
* Imagine things like setLightType, setLightIntensity, setFalloffAngle etc, see the corresponding lectures
*/
class Light : public GameObject {
public:
	Light(const std::string& name = nullptr, const glm::vec3& position = glm::vec3(2.0f, 10.0f, 5.0f));
	virtual ~Light();

	glm::vec3 GetDirection() const { return lightData.lightDirection; }
	void SetDirection(glm::vec3 direction);

	glm::vec3 GetDiffuseColor() const { return lightData.lightColor; }
	void SetDiffuseColor(glm::vec3 color);

	glm::vec3 GetAttenuation() const { return lightData.attenuation; }
	void SetAttenuation(glm::vec3 attenuation);
	void SetAttenuation(float constant, float linear, float quadratic);

	LightType GetLightType() const { return lightData.lightType; }
	void SetLightType(LightType lightType);

	LightData GetLightData() const;

	//override set parent to register/deregister light...
	void _setWorldRecursively(World* world) override;

private:
	LightData lightData = {};
};
