#pragma once
#include "mge/behaviours/AbstractBehaviour.hpp"

#include <glm/glm.hpp>

class CameraOrbit: public AbstractBehaviour{
public:
	CameraOrbit(GameObject* target, glm::vec3 offset, float orbitSpeed = 45.0f);
	virtual ~CameraOrbit();
	void lateUpdate(float ts) override;

private:
	void UpdateOrbit(float ts);
	
	GameObject* target;
	glm::vec3 offset;
	glm::vec2 orbitSpeed;

	glm::vec3 eulerAngles;
};
