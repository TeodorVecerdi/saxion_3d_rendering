#pragma once
#include "mge/behaviours/AbstractBehaviour.hpp"
#include <glm/glm.hpp>

class ObjectFollow : public AbstractBehaviour {
public:
	ObjectFollow(GameObject* target, glm::vec3 offset, glm::vec3 eulerAngles, float orbitSpeed = 30.0f);
	virtual ~ObjectFollow();
	void Update(float ts) override;

	void SetTarget(GameObject* target) { this->target = target; }
	GameObject* GetTarget() const { return target; }
	void SetOffset(glm::vec3 offset) { this->offset = offset; }
	glm::vec3 GetOffset() const { return offset; }
	void SetRotation(glm::vec3 eulerAngles);
	glm::vec3 GetRotation() const { return eulerAngles; }

private:
	void UpdateRotation();
	
	GameObject* target;
	glm::vec3 offset;
	glm::vec3 eulerAngles;
	glm::vec3 orbitEulerAngles;
	float orbitSpeed;

	bool rotationDirty;
};
