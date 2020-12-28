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


#include "behaviours/ObjectFollow.h"

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
    Mesh* cubeMeshF = Mesh::load (config::mge::MODEL_PATH+"cube_flat.obj");
    Mesh* sphereMeshS = Mesh::load (config::mge::MODEL_PATH+"sphere_smooth.obj");
	Mesh* planeMeshDefault = Mesh::load (config::mge::MODEL_PATH+"plane.obj");

    //MATERIALS

    //create some materials to display the cube, the plane and the light
    AbstractMaterial* lightMaterial = new ColorMaterial (glm::vec3(1,1,0));
    AbstractMaterial* runicStoneMaterial = new TextureMaterial (Texture::load (config::mge::TEXTURE_PATH+"runicfloor.png"));

    //SCENE SETUP
	GameObject* plane = new GameObject ("plane", glm::vec3(0,-5,0));
	plane->scale(glm::vec3(20,5,20));
	plane->setMesh(planeMeshDefault);
	plane->setMaterial(runicStoneMaterial);
	_world->add(plane);

   //add camera first (it will be updated last)
    Camera* camera = new Camera ("camera", glm::vec3(0,2,-10));
    _world->setMainCamera(camera);
	_world->add(camera);

    //add a spinning sphere
    GameObject* sphere = new GameObject ("sphere", glm::vec3(0,0,0));
    sphere->scale(glm::vec3(.5,.5,.5));
    sphere->setMesh (sphereMeshS);
    sphere->setMaterial(runicStoneMaterial);

    //add a light. Note that the light does ABSOLUTELY ZIP! NADA ! NOTHING !
    //It's here as a place holder to get you started.
    //Note how the texture material is able to detect the number of lights in the scene
    //even though it doesn't implement any lighting yet!

    GameObject* cube = new GameObject("cube", glm::vec3(0,4,0));
    cube->scale(glm::vec3(1.f, 1.f, 1.f));
    cube->setMesh(cubeMeshF);
    cube->setMaterial(runicStoneMaterial);
    cube->setBehaviour(new KeysBehaviour(5, 120));
	_world->add(cube);

	cube->add(sphere);

	auto* objectFollow = new ObjectFollow(cube, glm::vec3(1, 4, -10), 90+45, glm::normalize(glm::vec3(0,5,1)));
	camera->setBehaviour(objectFollow);

	sphere->setLocalPosition(glm::vec3(0, 3, 0));
	cube->setLocalPosition(glm::vec3(0, 0, 0));
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
