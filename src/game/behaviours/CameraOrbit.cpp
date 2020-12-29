#include "CameraOrbit.h"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include "game/utils/glm_utils.hpp"
#include "mge/core/GameObject.hpp"
#include "mge/util/Input.hpp"

CameraOrbit::CameraOrbit(GameObject* target, glm::vec3 offset, glm::vec3 eulerAngles, float orbitSpeed, float mouseSensitivity, float scrollSensitivity) :
	target(target), offset(offset), orbitSpeed(orbitSpeed), mouseSensitivity(mouseSensitivity), scrollSensitivity(scrollSensitivity), eulerAngles(eulerAngles), currentZoom(offset.z),
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

	auto targetPosition = target->getWorldPosition();
	auto translation = translate(targetPosition);
	auto rotation = utils::glm::RotateEulerXYZ(glm::mat4(1.0f), eulerAngles) * translate(-zoomedOffset);
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

		eulerAngles += glm::vec3(mouseSensitivityDelta * orbitSpeed, 0);

		if (eulerAngles.x > 45.0f) eulerAngles.x = 45.0f;
		if (eulerAngles.x < -45.0f) eulerAngles.x = -45.0f;
		if (eulerAngles.y >= 360.0f) eulerAngles.y -= 360.0f;
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
