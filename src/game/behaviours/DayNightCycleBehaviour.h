#pragma once
#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>


#include "mge/behaviours/AbstractBehaviour.hpp"

class Light;

class DayNightCycleBehaviour : public AbstractBehaviour {
public:
	DayNightCycleBehaviour(Light& day, Light& night, float speed, glm::vec3 axis, glm::vec3 offset);
	void Update(float ts) override;
	void Start() override;
private:
	float currentRotation;
	float speed;
	glm::vec3 axis;
	glm::vec3 offset;
	Light* day;
	Light* night;

	float dayAmbientIntensity = 0.0f;
	float dayDiffuseIntensity = 0.0f;
	float daySpecularIntensity = 0.0f;
	
	float nightAmbientIntensity = 0.0f;
	float nightDiffuseIntensity = 0.0f;
	float nightSpecularIntensity = 0.0f;

	glm::quat baseRotation;
};
