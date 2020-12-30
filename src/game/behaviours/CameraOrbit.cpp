#include "CameraOrbit.h"


#include <glm/trigonometric.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>


#include "game/utils/constants.hpp"
#include "game/utils/glm_utils.hpp"
#include "game/utils/math_utils.hpp"
#include "mge/core/GameObject.hpp"
#include "mge/util/Input.hpp"

CameraOrbit::CameraOrbit(GameObject* target, glm::vec3 offset, glm::vec3 eulerAngles, float orbitSpeed, float mouseSensitivity, float scrollSensitivity) :
	target(target), offset(offset), orbitSpeed(orbitSpeed), mouseSensitivity(mouseSensitivity), scrollSensitivity(scrollSensitivity), targetRotation(utils::constants::degToRad * eulerAngles), currentRotation(utils::constants::degToRad * eulerAngles), currentZoom(offset.z),
	zoomRange(-2, -20) {
	targetRotationQuat = glm::quat(targetRotation);
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

	if(!utils::math::Approx(currentPosition, targetPosition, 0.01f)) {
		// currentPosition = glm::lerp(currentPosition, targetPosition, 0.1f);
		// currentPosition = utils::math::SmoothStep(currentPosition, targetPosition,0.2f);
		currentPosition = utils::math::SmoothDamp(currentPosition, targetPosition, moveVelocity, 0.2f, ts);
	}
	
	if(!utils::math::Approx(currentRotation, targetRotationQuat, utils::constants::oneDeg))
		currentRotation = glm::slerp(currentRotation, targetRotationQuat, 0.2f);

	auto translation = translate(currentPosition);
	
	auto rotation = glm::toMat4(currentRotation) * translate(-zoomedOffset) ;
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
		const auto deltaRotation = utils::constants::degToRad * glm::vec3(mouseSensitivityDelta * orbitSpeed, 0);
		targetRotation += deltaRotation;

		// Would it be better to clamp the interpolated rotation instead? 
		if(targetRotation.x < -utils::constants::deg45) targetRotation.x = -utils::constants::deg45;
		if(targetRotation.x > utils::constants::deg45) targetRotation.x = utils::constants::deg45;

		// Wrap
		// if(targetRotation.y > utils::constants::pi - utils::constants::epsilon * 8.0f) targetRotation.y -= utils::constants::twoPi + utils::constants::epsilon * 8.0f;
		// if(targetRotation.y < -utils::constants::pi + utils::constants::epsilon * 8.0f) targetRotation.y += utils::constants::twoPi - utils::constants::epsilon * 8.0f;
		
		targetRotationQuat = glm::quat(targetRotation);
		std::cout << targetRotation << " :: " << targetRotationQuat << "\n";
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
