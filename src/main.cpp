#include <GL/glew.h>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "ShaderUtil.h"
#include <glm/glm.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
 * Basic demo of rendering a triangle in OpenGL through the new programmable pipeline.
 * In a real life example, this code should include error checking and refactor into classes/functions.
 * In addition VAO's are missing.
 */

int main() {
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

	window.setVerticalSyncEnabled(true);

	//initialize glew to load all available opengl functions/extensions
	GLenum glewResult = glewInit();
	if (glewResult != GLEW_OK) {
		std::cout << "Could not initialize GLEW, byeeee!" << std::endl;
		return -1;
	}

	//Create the shader program
	GLuint programID = ShaderUtil::createProgram("assets/lighting.vert", "assets/lighting.frag");

	//declare the data to upload
	const GLfloat vertices[] = {
		//1 triangle, 3 vertices per triangle, 3 floats per vertex = 9 floats in total
		-0.5f, -0.5f, 0,
		0.5f, 0.5f, 0,
		-0.5f, 0.5f, 0,
		0.5f, -0.5f, 0,
	};

	//create a handle to the buffer
	GLuint vertexBufferId;
	glGenBuffers(1, &vertexBufferId);
	//bind our buffer to the GL_ARRAY_BUFFER endpoint, since none was bound yet,
	//a new array buffer for vertex position data will be created
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	//stream all our data to the array buffer endpoint to which our vertexPositionsBufferId is connected
	//note that vertexPositionsBufferId is not mentioned, instead the ARRAY_BUFFER is set as the data "sink"
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//disconnect the funnel
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/*const GLfloat colors[] {
		//1 triangle, 3 vertices per triangle, 1 color per vertex, 3 "floats" per color RGB = 9 floats in total
		1,0,0,
		1,1,0,
		0,1,0,
		0,0,1
	};

	//create a handle to the buffer
	GLuint colorBufferId;
	glGenBuffers(1, &colorBufferId);
	//bind our buffer to the GL_ARRAY_BUFFER endpoint, since none was bound yet,
	//a new array buffer for vertex color data will be created
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferId);
	//stream all our data to the array buffer endpoint to which our vertexColorsBufferId is connected
	//note that vertexColorsBufferId is not mentioned, instead the ARRAY_BUFFER is set as the data "sink"
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	//disconnect the funnel
	glBindBuffer(GL_ARRAY_BUFFER, 0);*/

	// triangle indices ccw
	GLuint indices[] = {
		0, 1, 2,
		0, 3, 1
	};
	GLuint indexBuffer;
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof indices, indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	sf::Clock clock;
	float lastElapsedTime = 0.f;

	/*glm::vec2 checkerboardRowsCols = glm::vec2(10);
	const float checkerboardRowsColsChange = 10.0f;

	const float pi = glm::pi<float>();
	const float twoPi = 2.0f*pi;
	float rotation = 0;
	float rotationSpeed = pi / 8;
	const float rotationSpeedChange = 1.0f;

	float scaleTime = 0.0f;
	float checkerboardScaleSpeed = 1.0f;
	const float checkerboardScaleChange = 0.5f;*/

	glClearColor(0, 0, 0, 1);
	while (window.isOpen()) {
		float elapsedTime = clock.getElapsedTime().asSeconds();
		float deltaTime = elapsedTime - lastElapsedTime;
		// scaleTime += deltaTime * checkerboardScaleSpeed;

		glClear(GL_COLOR_BUFFER_BIT);

		// Update

		/*if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) checkerboardRowsCols.x -= checkerboardRowsColsChange * deltaTime;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))checkerboardRowsCols.x += checkerboardRowsColsChange * deltaTime;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) checkerboardRowsCols.y -= checkerboardRowsColsChange * deltaTime;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) checkerboardRowsCols.y += checkerboardRowsColsChange * deltaTime;
		
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) rotationSpeed -= rotationSpeedChange * deltaTime;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) rotationSpeed += rotationSpeedChange * deltaTime;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) checkerboardScaleSpeed -= checkerboardScaleChange * deltaTime;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) checkerboardScaleSpeed += checkerboardScaleChange * deltaTime;
		
		rotation += rotationSpeed * deltaTime;
		if(rotation > twoPi) rotation -= twoPi;*/

		// End Update

		//tell the GPU to use this program
		glUseProgram(programID);

		//offset
		/*glUniform2f(glGetUniformLocation(programID, "offset"), 0.5f * cos(elapsedTime), 0.5f * sin(elapsedTime));
		glUniform2f(glGetUniformLocation(programID, "checkerboardRowsCols"), checkerboardRowsCols.x, checkerboardRowsCols.y);
		glUniform2f(glGetUniformLocation(programID, "rotation"), cos(rotation), sin(rotation));
		glUniform1f(glGetUniformLocation(programID, "checkerScale"), 0.5f * sin(scaleTime));*/

		const auto mousePos = sf::Mouse::getPosition(window);
		// viewport mouse position (-1 -> 1)
		glm::vec2 vpMousePos = glm::clamp(glm::vec2((mousePos.x * 2.0f - 800)/800.0f, -(mousePos.y * 2.0f - 600)/600.0f), -1.0f, 1.0f);
		// std::cout << "Mouse position: [" << mousePos.x << "," << mousePos.y << "]\nViewport position: [" << vpMousePos.x << "," << vpMousePos.y << "]\n";
		glUniform2f(glGetUniformLocation(programID, "mousePosition"), vpMousePos.x, vpMousePos.y);
		glUniform2f(glGetUniformLocation(programID, "viewportSize"), 800, 600);

		//get index for the attributes in the shader
		GLint vertexIndex = glGetAttribLocation(programID, "vertex");
		// GLint colorIndex = glGetAttribLocation(programID, "color");

		//tell OpenGL that the data for the vertexIndex/colorIndex is coming in from an array
		glEnableVertexAttribArray(vertexIndex);
		// glEnableVertexAttribArray(colorIndex);

		//bind the buffer with data.
		//the moment this buffer is bound instead of 0, the last param of glVertexAttribPointer
		//is interpreted as an offset and not a pointer
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
		//send the data for this index to OpenGL, specifying it's format and structure
		//vertexIndex // 3 bytes per element // floats // don't normalize // the data itself
		glVertexAttribPointer(vertexIndex, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//send the data for this index to OpenGL, specifying it's format and structure
		//colorIndex // 3 bytes per element // floats // don't normalize // the data itself
		// glBindBuffer(GL_ARRAY_BUFFER, colorBufferId);
		// glVertexAttribPointer(colorIndex, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

		glDrawElements(GL_TRIANGLES, sizeof indices / sizeof indices[0], GL_UNSIGNED_INT, (void*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glDisableVertexAttribArray(vertexIndex);
		// glDisableVertexAttribArray (colorIndex);

		//display it
		window.display();

		//empty the event queue
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Escape) window.close();
			if (event.type == sf::Event::Resized) glViewport(0, 0, event.size.width, event.size.height);
			if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Space) {
				glDeleteProgram(programID);
				programID = ShaderUtil::createProgram("assets/lighting.vert", "assets/lighting.frag");
			}
		}

		lastElapsedTime = elapsedTime;

	}

	return 0;
}
