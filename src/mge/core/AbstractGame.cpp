#include <iostream>

#include "AbstractGame.hpp"

#include "Camera.hpp"
#include "mge/core/Renderer.hpp"
#include "mge/core/World.hpp"
#include "mge/util/Input.hpp"
#include "mge/util/Time.hpp"

AbstractGame::AbstractGame(): _window(nullptr), _renderer(nullptr), _world(nullptr), _fps(0), _input(nullptr) {
	//ctor
}

AbstractGame::~AbstractGame() {
	//dtor
	delete _window;
	delete _renderer;
	delete _world;
	delete _input;
}

void AbstractGame::initialize() {
	std::cout << "Initializing engine...\n\n";
	_initializeWindow();
	_printVersionInfo();
	_initializeGlew();
	_initializeRenderer();
	_initializeWorld();
	_initializeScene();
	std::cout << "\nEngine initialized.\n\n";
}

///SETUP

void AbstractGame::_initializeWindow() {
	std::cout << "Initializing window...\n";
	_window = new sf::RenderWindow(sf::VideoMode(1280, 720), "My Game!", sf::Style::Default, sf::ContextSettings(24, 8, 0, 3, 3));
	_window->setVerticalSyncEnabled(true);
	std::cout << "Window initialized.\n";

	// Create an input instance
	_input = new mge::Input(_window);
}

void AbstractGame::_printVersionInfo() {
	std::cout << "Context info:\n";
	std::cout << "----------------------------------\n";
	//print some debug stats for whoever cares
	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	//nice consistency here in the way OpenGl retrieves values
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	printf("GL Vendor : %s\n", vendor);
	printf("GL Renderer : %s\n", renderer);
	printf("GL Version (string) : %s\n", version);
	printf("GL Version (integer) : %d.%d\n", major, minor);
	printf("GLSL Version : %s\n", glslVersion);

	std::cout << "----------------------------------\n\n";
}

void AbstractGame::_initializeGlew() {
	std::cout << "Initializing GLEW..." << std::endl;
	//initialize the opengl extension wrangler
	GLint glewStatus = glewInit();
	std::cout << "Initialized GLEW, status (1 == OK, 0 == FAILED):" << (glewStatus == GLEW_OK) << "\n\n";
}

void AbstractGame::_initializeRenderer() {
	//setup our own renderer
	std::cout << "Initializing renderer...\n";
	_renderer = new Renderer();
	_renderer->setClearColor(0, 0, 0);
	std::cout << "Renderer done.\n\n";
}

void AbstractGame::_initializeWorld() {
	//setup the world
	std::cout << "Initializing world...\n";
	_world = new World();
	std::cout << "World initialized.\n\n";
}

///MAIN GAME LOOP

void AbstractGame::run() {
	//setting to calculate fps
	sf::Clock renderClock;
	int frameCount = 0;
	float timeSinceLastFPSCalculation = 0;

	sf::Clock updateClock;

	const auto windowSize = _window->getSize();
	auto mousePos = sf::Mouse::getPosition(*_window);
	mge::Input::OnViewportResized(windowSize.x, windowSize.y);
	mge::Input::ResetAll(mousePos.x, mousePos.y);

	while (_window->isOpen()) {
		const auto deltaTime = updateClock.restart().asSeconds();
		mge::Time::Update(deltaTime);

		// Update mouse position
		mousePos = sf::Mouse::getPosition(*_window);
		mge::Input::UpdateMousePosition(mousePos.x, mousePos.y);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		_update(deltaTime);
		_render();
		_window->display();

		//fps count is updated once every 1 second
		frameCount++;
		timeSinceLastFPSCalculation += renderClock.restart().asSeconds();
		if (timeSinceLastFPSCalculation > 1) {
			_fps = frameCount / timeSinceLastFPSCalculation;
			timeSinceLastFPSCalculation -= 1;
			frameCount = 0;
		}

		// Reset mouse delta
		mge::Input::ResetMouseDelta();
		mge::Input::ResetScrollDelta();

		_processEvents();
	}
}

void AbstractGame::_update(float pStep) {
	_world->update(pStep);
	_world->lateUpdate(pStep);
}

void AbstractGame::_render() {
	_renderer->render(_world);
}

void AbstractGame::_processEvents() {
	sf::Event event;
	bool exit = false;

	//we must empty the event queue
	while (_window->pollEvent(event)) {
		//give all system event listeners a chance to handle events
		//optionally to be implemented by you for example you could implement a
		//SystemEventDispatcher / SystemEventListener pair which allows Listeners to
		//register with the dispatcher and then do something like:
		//SystemEventDispatcher::dispatchEvent(event);
		mge::Input::OnEvent(event);
		switch (event.type) {
			case sf::Event::Closed:
				exit = true;
				break;
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Escape) {
					exit = true;
				}
				break;
			case sf::Event::Resized:
				//would be better to move this to the renderer
				//this version implements nonconstrained match viewport scaling
				glViewport(0, 0, event.size.width, event.size.height);
				break;

			default:
				break;
		}
	}

	if (exit) {
		_window->close();
	}
}
