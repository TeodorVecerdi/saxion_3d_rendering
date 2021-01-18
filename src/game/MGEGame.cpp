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
#include "behaviours/LightRotatingBehaviour.h"
#include "behaviours/ObjectFollow.h"
#include "behaviours/SimpleLightBehaviour.h"
#include "behaviours/SimpleLightBehaviour2.h"
#include "materials/LitColorMaterial.hpp"
#include "materials/LitTextureMaterial.hpp"
#include "materials/TerrainMaterial.hpp"
#include "materials/WobbleTextureMaterial.hpp"
#include "mge/behaviours/RotatingBehaviour.hpp"
#include "mge/core/Light.hpp"
#include "utils/constants.hpp"

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
	Mesh* cubeMeshSmooth = Mesh::load(mge::config::Model("cube_smooth.obj"));
	Mesh* sphereSmoothMesh = Mesh::load(mge::config::Model("sphere_smooth.obj"));
	Mesh* coneMesh = Mesh::load(game::config::Model("traffic_cone/model.obj"));
	Mesh* cylMesh = Mesh::load(mge::config::Model("cylinder_smooth.obj"));
	Mesh* terrainMesh = Mesh::load(mge::config::Model("plane_8192.obj"));

	//MATERIALS
	AbstractMaterial* lightMaterial = new TextureMaterial(Texture::load(mge::config::Texture("bricks.jpg")));
	LitColorMaterial* litMaterial = new LitColorMaterial(glm::vec3(0xba) / 255.0f);
	litMaterial->SetAmbientIntensity(0.1f);

	// mesh size / mesh vertices (on one side)
	const float vertexDistance = 2.0f / 64.0f;
	TerrainMaterial* terrainMaterial = new TerrainMaterial(Texture::load(game::config::Texture("terrain/heightmap2.png"), Texture::UV_WRAP::CLAMP),
	                                                       Texture::load(game::config::Texture("terrain/splatmap2.png"), Texture::UV_WRAP::CLAMP),
	                                                       Texture::load(mge::config::Texture("terrain/diffuse1.jpg")),
	                                                       Texture::load(game::config::Texture("terrain/uvtex.png")),
	                                                       Texture::load(game::config::Texture("water_seamless/water.jpg")),
	                                                       Texture::load(game::config::Texture("water_foam/foam.jpg")),
	                                                       Texture::load(mge::config::Texture("terrain/diffuse4.jpg")),
	                                                       glm::vec4(13.5, 10, 8, 10),
	                                                       0.5, 0.5*vertexDistance);
	terrainMaterial->SetAmbientIntensity(0.1);
	terrainMaterial->SetSpecularIntensity(0.5);
	terrainMaterial->SetShininess(64);

	LitTextureMaterial* trafficConeMaterial = new LitTextureMaterial(Texture::load(game::config::Texture("traffic_cone/diffuse.png")));
	trafficConeMaterial->SetAmbientIntensity(0.1f);
	AbstractMaterial* bricks_Material = new WobbleTextureMaterial(Texture::load(mge::config::Texture("bricks.jpg")));

	//SCENE SETUP
	GameObject* terrain = new GameObject("Terrain", utils::constants::zero);
	terrain->SetMaterial(terrainMaterial);
	terrain->SetMesh(terrainMesh);
	terrain->Scale(glm::vec3(100));
	//RotatingBehaviour* terrainBehaviour = new RotatingBehaviour(15, utils::constants::up);
	//terrain->SetBehaviour(terrainBehaviour);
	_world->AddChild(terrain);

	//add camera first (it will be updated last)
	Camera* camera = new Camera("camera", glm::vec3(0, 2, -10));
	camera->Rotate(-utils::constants::deg30, utils::constants::right);
	// camera->SetBehaviour(new RotatingBehaviour());
	_world->setMainCamera(camera);
	_world->AddChild(camera);

	GameObject* mainSphere = new GameObject("mainSphere", glm::vec3(0, 4, 0));
	mainSphere->Scale(glm::vec3(1.f, 1.f, 1.f));
	mainSphere->SetMesh(sphereSmoothMesh);
	mainSphere->SetMaterial(bricks_Material);
	mainSphere->SetBehaviour(new KeysBehaviour(20, 120));
	_world->AddChild(mainSphere);

	GameObject* cubeLeft = new GameObject("cubeLeft", glm::vec3(-2, 0, 0));
	cubeLeft->Scale(glm::vec3(0.75f));
	cubeLeft->SetMesh(cubeMeshSmooth);
	cubeLeft->SetMaterial(litMaterial);

	GameObject* cubeRight = new GameObject("cubeRight", glm::vec3(2, 0, 0));
	cubeRight->Scale(glm::vec3(0.75f));
	cubeRight->SetMesh(sphereSmoothMesh);
	cubeRight->SetMaterial(litMaterial);
	GameObject* cubeForward = new GameObject("cubeForward", glm::vec3(0, 0, 2));
	cubeForward->Scale(glm::vec3(0.75f));
	cubeForward->SetMesh(cubeMeshF);
	cubeForward->SetMaterial(litMaterial);

	mainSphere->AddChild(cubeLeft);
	mainSphere->AddChild(cubeRight);
	mainSphere->AddChild(cubeForward);

	GameObject* bigSphere = new GameObject("bigSphere", utils::constants::up * 10);
	bigSphere->SetMesh(sphereSmoothMesh);
	bigSphere->SetMaterial(litMaterial);
	bigSphere->Scale(glm::vec3(10));
	_world->AddChild(bigSphere);

	auto* cameraOrbit = new CameraOrbit(mainSphere, glm::vec3(0, 0, -10), glm::vec3(-25, 180, 0));
	camera->SetBehaviour(cameraOrbit);

	// Light
	Light* light = new Light("Light");
	light->SetAttenuation(1, 0.007f, 0.0004f);
	light->SetLightType(LightType::POINT);
	light->SetDiffuseColor(glm::vec3(255) / 255.0f);
	light->SetAmbientColor(glm::vec4(glm::vec3(255) / 255.0f, 0.175f));
	light->SetSpecularColor(glm::vec3(163, 13, 222) / 255.0f);
	auto* lightBeh = new SimpleLightBehaviour({0, 5, 0}, 10.0f);
	light->SetBehaviour(lightBeh);

	Light* spotlight = new Light("Spotlight Light");
	spotlight->SetAttenuation(1.0f, 0.007f, 0.0002f);
	spotlight->SetLightType(LightType::SPOTLIGHT);
	spotlight->SetDiffuseColor(2 * glm::vec3(158, 237, 31) / 255.0f);
	spotlight->SetAmbientColor(glm::vec4(1, 1, 1, 0.1f));
	spotlight->SetSpecularColor(glm::vec3(11, 214, 55) / 255.0f);
	spotlight->SetDirection(glm::vec3(0, -1, 0));
	spotlight->SetOuterAngle(utils::constants::oneDeg * 17.5f);
	spotlight->SetInnerAngle(utils::constants::oneDeg * 12.5f);
	auto* lightBeh2 = new SimpleLightBehaviour2({0, 20, 0}, 7.0f);
	spotlight->SetBehaviour(lightBeh2);

	Light* directional = new Light("Directional Light", {0, 10, 0});
	const glm::vec3 lightDirection = glm::quat(glm::vec3(30, 45, 0) * utils::constants::degToRad) * utils::constants::forward;
	directional->SetDirection(glm::normalize(lightDirection));
	directional->Rotate(utils::constants::deg45, utils::constants::up);
	directional->Rotate(utils::constants::deg30, utils::constants::right);
	directional->SetAttenuation(1, 0, 0.0f);
	directional->SetLightType(LightType::DIRECTIONAL);
	directional->SetDiffuseColor(0.8 * glm::vec3(1.0f, 1.0f, 1.0f));
	directional->SetAmbientColor(glm::vec4(1.0f, 1.0f, 1.0f, 0.1f));
	directional->SetSpecularColor(glm::vec3(210, 250, 248) / 255.0f);
	auto* lightBeh3 = new LightRotatingBehaviour(utils::constants::deg45, {30, 0, 0});
	//directional->SetBehaviour(lightBeh3);

	GameObject* lightCube = new GameObject("lightCube", glm::vec3(0, 0, 0));
	lightCube->Rotate(-utils::constants::deg90, utils::constants::right);
	lightCube->Scale(glm::vec3(0.5f));
	lightCube->SetMesh(cylMesh);
	lightCube->SetMaterial(lightMaterial);

	directional->AddChild(lightCube);
	_world->AddChild(directional);

	GameObject* lightCube2 = new GameObject("2", glm::vec3(0, 0, 0));
	lightCube2->Scale(glm::vec3(0.5f));
	lightCube2->SetMesh(sphereSmoothMesh);
	lightCube2->SetMaterial(lightMaterial);

	light->AddChild(lightCube2);
	//_world->AddChild(light);

	GameObject* lightCube3 = new GameObject("3", glm::vec3(0, 0, 0));
	lightCube3->Scale(0.25 * utils::constants::one);
	lightCube3->SetMesh(coneMesh);
	lightCube3->SetMaterial(trafficConeMaterial);
	spotlight->AddChild(lightCube3);
	//_world->AddChild(spotlight);
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
