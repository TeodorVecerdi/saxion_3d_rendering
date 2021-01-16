#pragma once
#include "mge/behaviours/AbstractBehaviour.hpp"

#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>

class CameraOrbit: public AbstractBehaviour{
public:
	CameraOrbit(GameObject* target, glm::vec3 offset, glm::vec3 eulerAngles = glm::vec3(0), float orbitSpeed = 45.0f, float mouseSensitivity = 0.1f, float scrollSensitivity = 100.0f);
	virtual ~CameraOrbit();
	void LateUpdate(float ts) override;

private:
	void UpdateInput(float ts);
	
	GameObject* target;
	glm::vec3 currentPosition;
	glm::vec3 offset;
	glm::vec2 orbitSpeed;
	float mouseSensitivity;
	float scrollSensitivity;
	
	float currentAngleY;
	float targetAngleY;
	float currentAngleX;
	float targetAngleX;

	float currentZoom = 0.0f;
	float targetZoom = 0.0f;
	glm::vec2 zoomRange;

	glm::vec3 moveVelocity = {0,0,0};
};
