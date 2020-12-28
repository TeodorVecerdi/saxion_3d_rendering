#pragma once
#include "mge/behaviours/AbstractBehaviour.hpp"
#include <glm.hpp>

class ObjectFollow : public AbstractBehaviour {
public:
	explicit ObjectFollow(GameObject* target, glm::vec3 offset, float angle, glm::vec3 axis, float followSpeed = 5.0f);
	virtual ~ObjectFollow();
	void update(float ts) override;

	void SetTarget(GameObject* target) { this->target = target; }
	GameObject* GetTarget() const { return target; }
	void SetOffset(glm::vec3 offset) { this->offset = offset; }
	glm::vec3 GetOffset() const { return offset; }
	void SetRotation(float angle, glm::vec3 axis);
	std::pair<float, glm::vec3> GetRotation() const { return std::pair(angle, axis); }

private:
	void UpdateRotation();
	
	GameObject* target;
	glm::vec3 offset;
	float angle;
	glm::vec3 axis;
	float followSpeed;

	bool rotationDirty;
};
