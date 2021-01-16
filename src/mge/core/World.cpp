#include <iostream>
#include <algorithm>

#include "mge/core/World.hpp"
#include "mge/core/Light.hpp"

World::World(): GameObject("root"), _mainCamera(nullptr) {
	_world = this;
}

void World::setMainCamera(Camera* pCamera) {
	if (pCamera != nullptr) _mainCamera = pCamera;
}

Camera* World::getMainCamera() const {
	return _mainCamera;
}

void World::registerLight(Light* pLight) {
	std::cout << "Registering light " << pLight->GetName() << std::endl;
	_lights.push_back(pLight);
}

void World::unregisterLight(Light* pLight) {
	std::cout << "Unregistering light " << pLight->GetName() << std::endl;
	if (_lights.size() == 0) return;
	_lights.erase(std::remove(_lights.begin(), _lights.end(), pLight), _lights.end());
}

Light* World::getLightAt(int pIndex) const {
	return _lights[pIndex];
}

uint32_t World::getLightCount() const {
	return _lights.size();
}
