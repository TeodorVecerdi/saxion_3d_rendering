#include <iostream>
#include <string>

#include "glm.hpp"

#include "mge/core/Mesh.hpp"
#include "mge/core/World.hpp"
#include "mge/core/Texture.hpp"
#include "mge/core/Camera.hpp"
#include "mge/core/GameObject.hpp"

#include "mge/materials/AbstractMaterial.hpp"
#include "mge/materials/ColorMaterial.hpp"
#include "mge/materials/TextureMaterial.hpp"

#include "mge/behaviours/KeysBehaviour.hpp"

#include "mge/util/DebugHud.hpp"

#include "mge/config.hpp"
#include "game/MGEGame.hpp"

#include "config.hpp"
#include "behaviours/CameraOrbit.h"

//construct the game class into _window, _renderer and hud (other parts are initialized by build)
MGEGame::MGEGame(): AbstractGame(), _hud(0) {
}

void MGEGame::initialize() {
	//setup the core part
	AbstractGame::initialize();

	//setup the custom part so we can display some text
	std::cout << "Initializing HUD" << std::endl;
	_hud = new DebugHud(_window);
	std::cout << "HUD initialized." << std::endl << std::endl;
}

//build the game _world
void MGEGame::_initializeScene() {
	//MESHES

	//load a bunch of meshes we will be using throughout this demo
	//each mesh only has to be loaded once, but can be used multiple times:
	//F is flat shaded, S is smooth shaded (normals aligned or not), check the models folder!
	Mesh* cubeMeshF = Mesh::load(mge::config::Model("cube_flat.obj"));
	Mesh* sniperMesh = Mesh::load(game::config::Model("sniper/sniper2.obj"));
	Mesh* planeMeshDefault = Mesh::load(mge::config::Model("plane.obj"));

	//MATERIALS
	AbstractMaterial* colorA_Material = new ColorMaterial(glm::vec3(233.0f, 196.0f, 106.0f) / 255.0f);
	AbstractMaterial* colorB_Material = new ColorMaterial(glm::vec3(42.0f, 157.0f, 143.0f) / 255.0f);
	AbstractMaterial* colorC_Material = new ColorMaterial(glm::vec3(233.0f, 56.0f, 105.0f) / 255.0f);
	AbstractMaterial* runicStoneMaterial = new TextureMaterial(Texture::load(mge::config::Texture("runicfloor.png")));
	AbstractMaterial* bricks_Material = new TextureMaterial(Texture::load(mge::config::Texture("bricks.jpg")));
	AbstractMaterial* sniperMaterial = new TextureMaterial(Texture::load(game::config::Texture("sniper/sniper-color.jpg")));

	//SCENE SETUP
	GameObject* plane = new GameObject("plane", glm::vec3(0, -5, 0));
	plane->scale(glm::vec3(100, 0.01f, 100));
	plane->setMesh(planeMeshDefault);
	plane->setMaterial(runicStoneMaterial);
	_world->add(plane);

	//add camera first (it will be updated last)
	Camera* camera = new Camera("camera", glm::vec3(0, 2, -10));
	_world->setMainCamera(camera);
	_world->add(camera);

	//add a spinning sphere
	GameObject* sniper = new GameObject("sniper", glm::vec3(0, 0, 0));
	sniper->rotate(glm::radians(-90.0f), glm::vec3(0, 1, 0));
	sniper->scale(glm::vec3(.75, .75, .75));
	sniper->setLocalPosition(glm::vec3(0, 1.5, 0));
	sniper->setMesh(sniperMesh);
	sniper->setMaterial(sniperMaterial);

	//add a light. Note that the light does ABSOLUTELY ZIP! NADA ! NOTHING !
	//It's here as a place holder to get you started.
	//Note how the texture material is able to detect the number of lights in the scene
	//even though it doesn't implement any lighting yet!

	GameObject* mainCube = new GameObject("mainCube", glm::vec3(0, 4, 0));
	mainCube->scale(glm::vec3(1.f, 1.f, 1.f));
	mainCube->setMesh(cubeMeshF);
	mainCube->setMaterial(bricks_Material);
	mainCube->setBehaviour(new KeysBehaviour(20, 120));
	_world->add(mainCube);

	GameObject* cubeLeft = new GameObject("cubeLeft", glm::vec3(-2, 0, 0));
	cubeLeft->scale(glm::vec3(0.75f));
	cubeLeft->setMesh(cubeMeshF);
	cubeLeft->setMaterial(colorA_Material);

	GameObject* cubeRight = new GameObject("cubeRight", glm::vec3(2, 0, 0));
	cubeRight->scale(glm::vec3(0.75f));
	cubeRight->setMesh(cubeMeshF);
	cubeRight->setMaterial(colorB_Material);
	GameObject* cubeForward = new GameObject("cubeForward", glm::vec3(0, 0, 2));
	cubeForward->scale(glm::vec3(0.75f));
	cubeForward->setMesh(cubeMeshF);
	cubeForward->setMaterial(colorC_Material);

	mainCube->add(sniper);
	mainCube->add(cubeLeft);
	mainCube->add(cubeRight);
	mainCube->add(cubeForward);

	// auto* objectFollow = new ObjectFollow(mainCube, glm::vec3(0, 6, -6), glm::vec3(-45, -180, 0));
	// camera->setBehaviour(objectFollow);
	auto* cameraOrbit = new CameraOrbit(mainCube, glm::vec3(0, 0, -10), glm::vec3(-25, 180, 0));
	camera->setBehaviour(cameraOrbit);

}

void MGEGame::_render() {
	AbstractGame::_render();
	_updateHud();
}

void MGEGame::_updateHud() {
	std::string debugInfo = "";
	debugInfo += std::string("FPS:") + std::to_string((int)_fps) + "\n";

	_hud->setDebugInfo(debugInfo);
	_hud->draw();
}

MGEGame::~MGEGame() {
	//dtor
}
