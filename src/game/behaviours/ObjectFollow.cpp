#include "ObjectFollow.h"

#include "mge/core/GameObject.hpp"

ObjectFollow::ObjectFollow(GameObject* target, const glm::vec3 offset, const float angle, const glm::vec3 axis, const float followSpeed) : target(target), offset(offset), followSpeed(followSpeed) {
	SetRotation(angle, axis);
}

ObjectFollow::~ObjectFollow() {
}

void ObjectFollow::update(float ts) {
	if (target == nullptr) return;

	if(rotationDirty) {
		UpdateRotation();
		rotationDirty = false;
	}

	// Todo interpolate / apply move speed
	const auto targetPosition = target->getWorldPosition() + offset;
	_owner->setLocalPosition(targetPosition);
}

void ObjectFollow::SetRotation(float angle, glm::vec3 axis) {
	this->angle = angle;
	this->axis = axis;
	rotationDirty = true;
}

// ReSharper disable once CppMemberFunctionMayBeConst <- Shouldn't be const
void ObjectFollow::UpdateRotation() {
	// Get original transform & decompose into translation, rotation, scale
	const auto transform = _owner->getTransform();
	glm::vec3 translation;
	glm::quat rotation;
	glm::vec3 scale;
	decompose(transform, scale, rotation, translation);

	// Calculate new matrix using original translation & scale, but with new rotation
	const auto T = glm::translate(translation);
	const auto R = glm::rotate(angle, axis);
	const auto S = glm::scale(scale);
	const auto TRS = T * R * S;
	_owner->setTransform(TRS);
}
