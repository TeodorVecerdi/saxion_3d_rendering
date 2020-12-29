#include "CameraOrbit.h"


#include <glm/trigonometric.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include "game/utils/glm_utils.hpp"
#include "game/utils/math_utils.hpp"
#include "mge/core/GameObject.hpp"
#include "mge/util/Input.hpp"

CameraOrbit::CameraOrbit(GameObject* target, glm::vec3 offset, glm::vec3 eulerAngles, float orbitSpeed, float mouseSensitivity, float scrollSensitivity) :
	target(target), offset(offset), orbitSpeed(orbitSpeed), mouseSensitivity(mouseSensitivity), scrollSensitivity(scrollSensitivity), targetRotation(glm::degrees(eulerAngles)), currentRotation(glm::degrees(eulerAngles)), currentZoom(offset.z),
	zoomRange(-2, -20) {

	// Lock mouse to middle of screen and hide
	mge::Input::SetMouseLock(true);
}

CameraOrbit::~CameraOrbit() {
	// Unlock mouse and show
	mge::Input::SetMouseLock(false);
}

void CameraOrbit::lateUpdate(const float ts) {

	UpdateInput(ts);

	// Zoom test
	auto zoomedOffset = offset;
	zoomedOffset.z = currentZoom;

	const auto targetPosition = target->getWorldPosition();

	if(length(glm::abs(targetPosition-currentPosition)) > 0.01f) {
		// currentPosition = glm::lerp(currentPosition, targetPosition, 0.1f);
		// currentPosition = utils::math::SmoothStep(currentPosition, targetPosition,0.2f);
		currentPosition = utils::math::SmoothDamp(currentPosition, targetPosition, moveVelocity, 0.2f, ts);
	}

	currentRotation = glm::slerp(currentRotation, targetRotation, 0.2f);
	
	auto translation = translate(currentPosition);
	
	auto rotation = glm::toMat4(targetRotation) * translate(-zoomedOffset) ;
	auto matrix = translation * rotation;

	_owner->setTransform(matrix);
}

void CameraOrbit::UpdateInput(const float ts) {
	///----------------------------------------------------------
	///						  UPDATE ORBIT
	///----------------------------------------------------------
	// Update speed
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) orbitSpeed.y -= 60.0f * ts;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) orbitSpeed.y += 60.0f * ts;
	if (orbitSpeed.y < 0.0f) orbitSpeed.y = 0.0f;

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		//											   v- Use yx to flip input as `x delta` should control y rotation and vice-versa 
		const auto mouseDelta = yx(mge::Input::MouseDelta());
		const auto mouseSensitivityDelta = mouseDelta * mouseSensitivity;
		const auto deltaRotation = glm::radians(glm::vec3(mouseSensitivityDelta * orbitSpeed, 0));
		// targetEulerAngles += glm::vec3(mouseSensitivityDelta * orbitSpeed, 0);
		targetRotation = targetRotation * glm::normalize(glm::angleAxis(deltaRotation.x, glm::vec3(1, 0, 0)));
		targetRotation =  glm::normalize(glm::angleAxis(deltaRotation.y, glm::vec3(0, 1, 0))) * targetRotation;
		// targetRotation = utils::math::ClampRotation(targetRotation, glm::vec3(45, 180, 180));
		// targetRotation = glm::rotate(targetRotation, deltaRotation.x, glm::vec3(1, 0, 0));
		// targetRotation = glm::rotate(targetRotation, deltaRotation.y, glm::vec3(0, 1, 0));

		// if(pitch(targetRotation))
		// if(targetEulerAngles.y < -180.0f) targetEulerAngles.y += 360.0f;
		// if(targetEulerAngles.y > 180.0f) targetEulerAngles.y -= 360.0f;
		// if ( targetEulerAngles.y>= 360.0f) targetEulerAngles.y -= 360.0f;
	}

	///----------------------------------------------------------
	///						  UPDATE ZOOM
	///----------------------------------------------------------
	const auto scrollDelta = mge::Input::ScrollDelta();
	const auto scrollSensitivityDelta = scrollDelta * scrollSensitivity;
	currentZoom += scrollSensitivityDelta;
	if(currentZoom > zoomRange.x) currentZoom = zoomRange.x;
	if(currentZoom < zoomRange.y) currentZoom = zoomRange.y;
}
