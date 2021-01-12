#pragma once

#include "glm.hpp"
#include "mge/core/GameObject.hpp"

enum LightType: uint32_t {
	DIRECTIONAL = 1,
	POINT = 2,
	SPOTLIGHT = 4
};

struct LightData {
	LightType type;

	glm::vec3 position;
	glm::vec3 direction;

	glm::vec4 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	
	glm::vec3 attenuation; // x = const, y = linear, z = quadratic

	float innerCutOff;
	float outerCutOff;
};

/**
* Exercise for the student: implement the Light class...
* Imagine things like setLightType, setLightIntensity, setFalloffAngle etc, see the corresponding lectures
*/
class Light : public GameObject {
public:
	Light(const std::string& name = nullptr, const glm::vec3& position = glm::vec3(2.0f, 10.0f, 5.0f));
	virtual ~Light();

	glm::vec3 GetDirection() const { return lightData.direction; }
	void SetDirection(glm::vec3 direction);

	glm::vec3 GetDiffuseColor() const { return lightData.diffuse; }
	void SetDiffuseColor(glm::vec3 color);

	glm::vec4 GetAmbientColor() const { return lightData.ambient; }
	void SetAmbientColor(glm::vec4 color);

	glm::vec3 GetSpecularColor() const { return lightData.specular; }
	void SetSpecularColor(glm::vec3 color);

	glm::vec3 GetAttenuation() const { return lightData.attenuation; }
	void SetAttenuation(glm::vec3 attenuation);
	void SetAttenuation(float constant, float linear, float quadratic);

	LightType GetLightType() const { return lightData.type; }
	void SetLightType(LightType lightType);

	float GetOuterAngle() const { return glm::acos(lightData.outerCutOff);}
	void SetOuterAngle(float outerAngle);
	float GetInnerAngle() const { return glm::acos(lightData.innerCutOff);}
	void SetInnerAngle(float innerAngle);

	LightData GetLightData() const;

	//override set parent to register/deregister light...
	void _setWorldRecursively(World* world) override;

private:
	LightData lightData = {};
};
