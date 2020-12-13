#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "ShaderUtil.h"
#include <glm/glm.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace glm {
	typedef vec<2, int> vec2i;
}


/**
 * Basic demo of rendering a triangle in OpenGL through the new programmable pipeline.
 * In a real life example, this code should include error checking and refactor into classes/functions.
 * In addition VAO's are missing.
 */
glm::vec2 windowSize;
glm::vec2 pixelSize;

float mapRange(float s, float a1, float a2, float b1, float b2) {
	return b1 + (s - a1) * (b2 - b1) / (a2 - a1);
}

glm::vec2 screenToClip(glm::vec2 screenPos) {
	auto screen = glm::vec2(screenPos.x, screenPos.y);
	auto vpScreen = screen * pixelSize;
	auto vp = glm::vec2(
		mapRange(vpScreen.x, 0, 1, -1, 1),
		mapRange(vpScreen.y, 1, 0, -1, 1)
	);
	return vp;
}

void makePlane(glm::vec2i divisions, glm::vec2 size, glm::vec2 vertexOffset, float* vertices, float* uvs, uint32_t* indices) {
	glm::vec2 sizeFactor = glm::vec2(size.x/divisions.x, size.y/divisions.y);
	glm::vec2 uvFactor = glm::vec2(1.0f/divisions.x, 1.0f/divisions.y);

	int vertexIndex = 0;
	int textureCoordIndex = 0;
	for(int i = 0; i <= divisions.y; i++) {
		for(int j = 0; j <= divisions.x; j++) {
			glm::vec2 vert = screenToClip(glm::vec2(j, i) * sizeFactor + vertexOffset);

			vertices[vertexIndex] = vert.x;
			vertices[vertexIndex+1] = vert.y;
			vertices[vertexIndex+2] = 0.0f;
			vertexIndex+=3;
			
			uvs[textureCoordIndex] = j * uvFactor.x;
			uvs[textureCoordIndex+1] = 1 - i * uvFactor.y;
			textureCoordIndex+=2;
		}
	}

	int index = 0;
	for(int i = 0; i < divisions.y; i++) {
		int currentRow = i * (divisions.x + 1);
		int nextRow = (i+1) * (divisions.x + 1);
		for(int j = 0; j < divisions.x; j++) {
			indices[index + 0] = currentRow + j;
			indices[index + 1] = nextRow + j;
			indices[index + 2] = currentRow + j + 1;
			indices[index + 3] = currentRow + j + 1;
			indices[index + 4] = nextRow + j;
			indices[index + 5] = nextRow + j + 1;
			index += 6;
		}
	}
}

int main() {
#pragma region Init 
	//Open SFML Window == Valid OpenGL Context
	sf::Window window(
		sf::VideoMode(800, 600), //800 x 600 x 32 bits pp
		"OpenGL Window",         //title
		sf::Style::Default,      //Default, Fullscreen, Titlebar, etc
		sf::ContextSettings(
			24, //24 bits depth buffer, important for 3D!
			0,  //no stencil buffer
			0,  //no anti aliasing
			3,  //requested major OpenGL version
			3   //requested minor OpenGL version
		)
	);

	const auto _wSize = window.getSize();
	windowSize = glm::vec2(_wSize.x, _wSize.y);
	const float pixelSizeW = 1.0f / windowSize.x;
	const float pixelSizeH = 1.0f / windowSize.y;
	pixelSize = glm::vec2(pixelSizeW, pixelSizeH);

	window.setVerticalSyncEnabled(true);

	//initialize glew to load all available opengl functions/extensions
	GLenum glewResult = glewInit();
	if (glewResult != GLEW_OK) {
		std::cout << "Could not initialize GLEW, byeeee!" << std::endl;
		return -1;
	}
#pragma endregion 

	const std::string normalShader = "checkerboard";
	const std::string radialShader = "radial";

	//Create the shader program
	GLuint programID =
		ShaderUtil::createProgram("assets/" + normalShader + ".vert", "assets/" + normalShader + ".frag");
	GLuint radialProgramID = ShaderUtil::createProgram("assets/" + radialShader + ".vert",
	                                                   "assets/" + radialShader + ".frag");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_PROGRAM_POINT_SIZE);
	
	const int cols = 9;
	const int rows = 9;
	
	GLfloat vertices[3 * (cols+1) * (rows+1)]; 
	GLfloat uvs[2 * (cols+1) * (rows+1)];
	GLuint indices[6 * cols * rows];
	makePlane(glm::vec2i(cols, rows), glm::vec2(500, 500), glm::vec2(100, 100), vertices, uvs, indices);

	GLuint vertexBufferId;
	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint uvBufferId;
	glGenBuffers(1, &uvBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, uvBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof uvs, uvs, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint indexBuffer;
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof indices, indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	sf::Clock clock;
	float lastElapsedTime = 0.f;

	glm::vec2 checkerboardRowsCols = glm::vec2(10);
	const float checkerboardRowsColsChange = 10.0f;

	const float pi = 3.14159265358979323846264338327950288f;
	const float twoPi = 2.0f * pi;
	float rotation = 0;
	float rotationSpeed = pi / 8;
	const float rotationSpeedChange = 1.0f;

	float scaleTime = 0.0f;
	float checkerboardScaleSpeed = 1.0f;
	const float checkerboardScaleChange = 0.5f;

	float lightSize = 0.5f;
	float lightStrength = 1.0f;
	float ambientLight = 0.1f;

	const float autoUpdateTime = 0.5f;
	float autoUpdateTimer = 0.0f;

	glClearColor(0, 0, 0, 1);
	while (window.isOpen()) {
		float elapsedTime = clock.getElapsedTime().asSeconds();
		float deltaTime = elapsedTime - lastElapsedTime;
		scaleTime += deltaTime * checkerboardScaleSpeed;

		autoUpdateTimer += deltaTime;
		if (autoUpdateTimer >= autoUpdateTime) {
			autoUpdateTimer -= autoUpdateTime;
			glDeleteProgram(programID);
			programID = ShaderUtil::createProgram("assets/" + normalShader + ".vert", "assets/" + normalShader + ".frag");
		}

		const auto mousePos = sf::Mouse::getPosition(window);
		// viewport mouse position (-1 -> 1)
		glm::vec2 vpMousePos = glm::clamp(
			glm::vec2((mousePos.x * 2.0f - windowSize.x) / windowSize.x, -(mousePos.y * 2.0f - windowSize.y) / windowSize.y), -1.0f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT);

		if (window.hasFocus()) {

			// Update
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
				checkerboardRowsCols.x -= checkerboardRowsColsChange * deltaTime;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
				checkerboardRowsCols.x += checkerboardRowsColsChange * deltaTime;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
				checkerboardRowsCols.y -= checkerboardRowsColsChange * deltaTime;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
				checkerboardRowsCols.y += checkerboardRowsColsChange * deltaTime;

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
				rotationSpeed -= rotationSpeedChange * deltaTime;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
				rotationSpeed += rotationSpeedChange * deltaTime;

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
				checkerboardScaleSpeed -= checkerboardScaleChange * deltaTime;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
				checkerboardScaleSpeed += checkerboardScaleChange * deltaTime;
			
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
				lightSize -= 1.0f * deltaTime;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
				lightSize += 1.0f * deltaTime;
			if (lightSize < 0) lightSize = 0;
			
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
				lightStrength -= 1.0f * deltaTime;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
				lightStrength += 1.0f * deltaTime;
			if (lightStrength < 0) lightStrength = 0;
			if (lightStrength > 1) lightStrength = 1;
			
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
				ambientLight -= 1.0f * deltaTime;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::V))
				ambientLight += 1.0f * deltaTime;
			if (ambientLight < 0) ambientLight = 0;
			if (ambientLight > 1) ambientLight = 1;
		}

		rotation += rotationSpeed * deltaTime;
		if (rotation > twoPi) rotation -= twoPi;
		//// End Update

		//tell the GPU to use this program
		glUseProgram(programID);

		//offset
		glUniform2f(glGetUniformLocation(programID, "offset"), 0.5f * cos(elapsedTime), 0.5f * sin(elapsedTime));
		glUniform2f(glGetUniformLocation(programID, "checkerboardRowsCols"), checkerboardRowsCols.x, checkerboardRowsCols.y);
		glUniform2f(glGetUniformLocation(programID, "rotation"), cos(rotation), sin(rotation));
		glUniform1f(glGetUniformLocation(programID, "checkerScale"), 0.5f * sin(scaleTime));

		// light
		glUniform2f(glGetUniformLocation(programID, "mousePosition"), vpMousePos.x, vpMousePos.y);
		glUniform2f(glGetUniformLocation(programID, "viewportSize"), windowSize.x, windowSize.y);
		glUniform1f(glGetUniformLocation(programID, "lightSize"), lightSize);
		glUniform1f(glGetUniformLocation(programID, "lightStrength"), lightStrength);
		glUniform1f(glGetUniformLocation(programID, "ambientLight"), ambientLight);
		glUniform1f(glGetUniformLocation(programID, "time"), elapsedTime);

		//get index for the attributes in the shader
		GLint vertexIndex = glGetAttribLocation(programID, "vertex");
		GLint uvIndex = glGetAttribLocation(programID, "uv");
		// GLint colorIndex = glGetAttribLocation(programID, "color");

		//tell OpenGL that the data for the vertexIndex/colorIndex is coming in from an array
		glEnableVertexAttribArray(vertexIndex);
		glEnableVertexAttribArray(uvIndex);

		//bind the buffer with data.
		//the moment this buffer is bound instead of 0, the last param of glVertexAttribPointer
		//is interpreted as an offset and not a pointer
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
		//send the data for this index to OpenGL, specifying it's format and structure
		//vertexIndex // 3 bytes per element // floats // don't normalize // the data itself
		glVertexAttribPointer(vertexIndex, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//send the data for this index to OpenGL, specifying it's format and structure
		//colorIndex // 3 bytes per element // floats // don't normalize // the data itself
		glBindBuffer(GL_ARRAY_BUFFER, uvBufferId);
		glVertexAttribPointer(uvIndex, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		glDrawElements(GL_TRIANGLES, sizeof indices / sizeof indices[0], GL_UNSIGNED_INT, (void*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glDisableVertexAttribArray(vertexIndex);
		glDisableVertexAttribArray(uvIndex);

		glUseProgram(radialProgramID);

		glUniform2f(glGetUniformLocation(radialProgramID, "offset"), 0.15f * cos(elapsedTime*2) + 0.5, 0.15f * sin(elapsedTime*2) + 0.7);
		glUniform2f(glGetUniformLocation(radialProgramID, "checkerboardRowsCols"), checkerboardRowsCols.x, checkerboardRowsCols.y);
		glUniform2f(glGetUniformLocation(radialProgramID, "rotation"), cos(rotation), sin(rotation));
		glUniform1f(glGetUniformLocation(radialProgramID, "checkerScale"), 0.5f * sin(scaleTime));
		glUniform2f(glGetUniformLocation(radialProgramID, "mousePosition"), vpMousePos.x, vpMousePos.y);
		glUniform2f(glGetUniformLocation(radialProgramID, "viewportSize"), windowSize.x, windowSize.y);
		glUniform1f(glGetUniformLocation(radialProgramID, "lightSize"), lightSize);
		glUniform1f(glGetUniformLocation(radialProgramID, "lightStrength"), lightStrength);
		glUniform1f(glGetUniformLocation(radialProgramID, "ambientLight"), ambientLight);
		glUniform1f(glGetUniformLocation(radialProgramID, "time"), elapsedTime);

		vertexIndex = glGetAttribLocation(radialProgramID, "vertex");
		uvIndex = glGetAttribLocation(radialProgramID, "uv");
		glEnableVertexAttribArray(vertexIndex);
		glEnableVertexAttribArray(uvIndex);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
		glVertexAttribPointer(vertexIndex, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, uvBufferId);
		glVertexAttribPointer(uvIndex, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		
		// glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		glDrawElements(GL_TRIANGLES, sizeof indices / sizeof indices[0], GL_UNSIGNED_INT, (void*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glDisableVertexAttribArray(vertexIndex);
		glDisableVertexAttribArray(uvIndex);

		//display it
		window.display();

		//empty the event queue
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Escape) window.close();
			if (event.type == sf::Event::Resized) glViewport(0, 0, event.size.width, event.size.height);
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Space) {
				glDeleteProgram(programID);
				programID = ShaderUtil::createProgram("assets/" + normalShader + ".vert",
				                                      "assets/" + normalShader + ".frag");
				glDeleteProgram(radialProgramID);
				radialProgramID = ShaderUtil::createProgram("assets/" + radialShader + ".vert",
				                                            "assets/" + radialShader + ".frag");
			}
		}

		lastElapsedTime = elapsedTime;

	}

	return 0;
}
