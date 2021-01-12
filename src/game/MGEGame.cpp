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
#include "materials/WobbleTextureMaterial.hpp"
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
	// -----------------------------------------
	//		          SPLAT MAP
	// -----------------------------------------

	/*                         /->   1-(r+g+b+a) = baseTex
	*                        \/
	* inputs => splatMap, baseTex, redTex, greenTex, blueTex, alphaTex
	* rgba = sampleTexture(splatMap, uv)
	* finalColor = mix using channels + textures
	* gl_FragColor = finalColor
	*
	*
	*
	*
	*	    TERRAIN UV COORDINATES
	*	    
	*      				    (1,1)
	*       -----------------
	*       |               |
	*       |               |
	*       |    terrain    |
	*       |      mesh     |
	*       |               |
	*       |               |
	*       -----------------
	* (0,0)
	*/

	//MESHES

	//load a bunch of meshes we will be using throughout this demo
	//each mesh only has to be loaded once, but can be used multiple times:
	//F is flat shaded, S is smooth shaded (normals aligned or not), check the models folder!
	Mesh* cubeMeshF = Mesh::load(mge::config::Model("cube_flat.obj"));
	Mesh* cubeMeshSmooth = Mesh::load(mge::config::Model("cube_smooth.obj"));
	Mesh* sphereSmoothMesh = Mesh::load(mge::config::Model("sphere_smooth.obj"));
	Mesh* coneMesh = Mesh::load(game::config::Model("traffic_cone/model.obj"));
	Mesh* cylMesh = Mesh::load(mge::config::Model("cylinder_smooth.obj"));
	Mesh* sniperMesh = Mesh::load(game::config::Model("sniper/sniper2.obj"));
	Mesh* planeMeshDefault = Mesh::load(mge::config::Model("plane.obj"));

	//MATERIALS
	AbstractMaterial* colorA_Material = new LitColorMaterial(glm::vec3(233.0f, 196.0f, 106.0f) / 255.0f);
	AbstractMaterial* lightMaterial = new TextureMaterial(Texture::load(mge::config::Texture("bricks.jpg")));
	AbstractMaterial* colorB_Material = new LitColorMaterial(glm::vec3(42.0f, 157.0f, 143.0f) / 255.0f);
	LitColorMaterial* litMaterial = new LitColorMaterial(glm::vec3(0xba) / 255.0f);
	litMaterial->SetAmbientIntensity(0.1f);
	LitColorMaterial* sphereMat = new LitColorMaterial(glm::vec3(0xff, 0x25, 0x5a) / 255.0f);
	sphereMat->SetShininess(32.0f);
	sphereMat->SetSpecularIntensity(1.0f);
	sphereMat->SetAmbientIntensity(0.1f);

	LitTextureMaterial* runicStoneMaterial = new LitTextureMaterial(Texture::load(mge::config::Texture("runicfloor.png")));
	AbstractMaterial* bricks_Material = new WobbleTextureMaterial(Texture::load(mge::config::Texture("bricks.jpg")));
	LitTextureMaterial* sniperMaterial = new LitTextureMaterial(Texture::load(game::config::Texture("sniper/sniper-color.jpg")));
	TextureMaterial* trafficConeMaterial = new TextureMaterial(Texture::load(game::config::Texture("traffic_cone/diffuse.png")));

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

	GameObject* mainSphere = new GameObject("mainSphere", glm::vec3(0, 4, 0));
	mainSphere->scale(glm::vec3(1.f, 1.f, 1.f));
	mainSphere->setMesh(sphereSmoothMesh);
	mainSphere->setMaterial(bricks_Material);
	mainSphere->setBehaviour(new KeysBehaviour(20, 120));
	_world->add(mainSphere);

	GameObject* cubeLeft = new GameObject("cubeLeft", glm::vec3(-2, 0, 0));
	cubeLeft->scale(glm::vec3(0.75f));
	cubeLeft->setMesh(cubeMeshSmooth);
	cubeLeft->setMaterial(colorA_Material);

	GameObject* cubeRight = new GameObject("cubeRight", glm::vec3(2, 0, 0));
	cubeRight->scale(glm::vec3(0.75f));
	cubeRight->setMesh(sphereSmoothMesh);
	cubeRight->setMaterial(colorB_Material);
	GameObject* cubeForward = new GameObject("cubeForward", glm::vec3(0, 0, 2));
	cubeForward->scale(glm::vec3(0.75f));
	cubeForward->setMesh(cubeMeshF);
	cubeForward->setMaterial(litMaterial);

	GameObject* bigSphere = new GameObject("bigSphere", glm::vec3(0, 2, 0));
	bigSphere->scale(glm::vec3(3.0f));
	bigSphere->setMesh(sphereSmoothMesh);
	bigSphere->setMaterial(runicStoneMaterial);
	_world->add(bigSphere);

	mainSphere->add(sniper);
	mainSphere->add(cubeLeft);
	mainSphere->add(cubeRight);
	mainSphere->add(cubeForward);

	// auto* objectFollow = new ObjectFollow(mainSphere, glm::vec3(0, 6, -6), glm::vec3(-45, -180, 0));
	// camera->setBehaviour(objectFollow);
	auto* cameraOrbit = new CameraOrbit(mainSphere, glm::vec3(0, 0, -10), glm::vec3(-25, 180, 0));
	camera->setBehaviour(cameraOrbit);

	// Light
	Light* light = new Light("Light");
	light->SetAttenuation(1, 0.03f, 0.024f);
	light->SetLightType(LightType::POINT);
	light->SetDiffuseColor(glm::vec3(237, 89, 59)/255.0f);
	light->SetAmbientColor(glm::vec4(glm::vec3(139, 237, 59)/255.0f, 0.175f));
	light->SetSpecularColor(glm::vec3(163, 13, 222)/255.0f);
	auto* lightBeh = new SimpleLightBehaviour({0, 5, 0}, 10.0f);
	light->setBehaviour(lightBeh);

	Light* spotlight = new Light("Spotlight Light");
	spotlight->SetAttenuation(1, 0.007, 0.0002);
	spotlight->SetLightType(LightType::SPOTLIGHT);
	spotlight->SetDiffuseColor(2*glm::vec3(158, 237, 31)/255.0f);
	spotlight->SetAmbientColor(glm::vec4(1,1,1,0.1f));
	spotlight->SetSpecularColor(glm::vec3(11, 214, 55)/255.0f);
	spotlight->SetDirection(glm::vec3(0, -1, 0));
	spotlight->SetOuterAngle(utils::constants::oneDeg * 17.5f);
	spotlight->SetInnerAngle(utils::constants::oneDeg * 12.5f);
	auto* lightBeh2 = new SimpleLightBehaviour2({0, 10, 0}, 7.0f);
	spotlight->setBehaviour(lightBeh2);

	Light* directional = new Light("Directional Light", {0, 10, 0});
	const glm::vec3 lightDirection = glm::quat(glm::vec3(45, 45, 0) * utils::constants::degToRad) * utils::constants::forward;
	directional->SetDirection(glm::normalize(lightDirection));
	directional->rotate(utils::constants::deg45, utils::constants::up);
	directional->rotate(utils::constants::deg45, utils::constants::right);
	directional->SetAttenuation(1, 0.0f, 0.0f);
	directional->SetLightType(LightType::DIRECTIONAL);
	directional->SetOuterAngle(utils::constants::oneDeg * 17.5f);
	directional->SetInnerAngle(utils::constants::oneDeg * 12.5f);
	directional->SetDiffuseColor(glm::vec3(1.0f, 1.0f, 1.0f));
	directional->SetAmbientColor(glm::vec4(1.0f, 1.0f, 1.0f, 0.1f));
	directional->SetSpecularColor(glm::vec3(210, 250, 248)/255.0f);
	auto* lightBeh3 = new LightRotatingBehaviour(utils::constants::deg45, {45, 0, 0});
	directional->setBehaviour(lightBeh3);

	GameObject* lightCube = new GameObject("lightCube", glm::vec3(0, 0, 0));
	lightCube->rotate(-utils::constants::deg90, utils::constants::right);
	lightCube->scale(glm::vec3(0.5f));
	lightCube->setMesh(cylMesh);
	lightCube->setMaterial(lightMaterial);
	
	directional->add(lightCube);
	_world->add(directional);

	GameObject* lightCube2 = new GameObject("2", glm::vec3(0, 0, 0));
	lightCube2->scale(glm::vec3(0.5f));
	lightCube2->setMesh(sphereSmoothMesh);
	lightCube2->setMaterial(lightMaterial);
	
	light->add(lightCube2);
	_world->add(light);

	GameObject* lightCube3 = new GameObject("3", glm::vec3(0, 0, 0));
	lightCube3->scale(0.25*utils::constants::one);
	lightCube3->setMesh(coneMesh);
	lightCube3->setMaterial(trafficConeMaterial);
	spotlight->add(lightCube3);
	_world->add(spotlight);
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
