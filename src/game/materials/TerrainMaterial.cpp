#include "glm.hpp"

#include "TerrainMaterial.hpp"


#include <iostream>
#include <SFML/Window/Keyboard.hpp>

#include "game/config.hpp"
#include "game/utils/constants.hpp"
#include "game/utils/gl_utils.hpp"
#include "game/utils/string_utils.hpp"
#include "mge/core/Camera.hpp"
#include "mge/core/Light.hpp"
#include "mge/core/Mesh.hpp"
#include "mge/core/ShaderProgram.hpp"
#include "mge/core/Texture.hpp"
#include "mge/core/World.hpp"
#include "mge/util/Time.hpp"

ShaderProgram* TerrainMaterial::_shader = nullptr;

TerrainMaterial::TerrainMaterial(Texture* heightmap, Texture* splatmap, Texture* baseTexture, Texture* textureR, Texture* textureWaterA, Texture* textureWaterB, Texture* textureB,
                                 glm::vec4 textureSizes, const float height, const float normalStepSize) : heightmap(heightmap), splatmap(splatmap),
                                                                                                           textureR(textureR), textureWaterA(textureWaterA), textureWaterB(textureWaterB), textureB(textureB),
                                                                                                           baseTexture(baseTexture), textureSizes(textureSizes), height(height),
                                                                                                           normalStepSize(normalStepSize), ambientColor(1, 1, 1, 0.1f),
                                                                                                           specularColor(1, 1, 1, 0.5f), shininess(256), eye(0) {
	//every time we create an instance of colormaterial we check if the corresponding shader has already been loaded
	_lazyInitializeShader();
}

void TerrainMaterial::_lazyInitializeShader() {
	//this shader contains everything the material can do (it can render something in 3d using a single color)
	if (!_shader) {
		_shader = new ShaderProgram();
		_shader->addShader(GL_VERTEX_SHADER, game::config::Shader("terrain.vert"));
		_shader->addShader(GL_FRAGMENT_SHADER, game::config::Shader("terrain.frag"));
		_shader->finalize();
	}
}

void TerrainMaterial::SetHeightmap(Texture* heightmap) {
	this->heightmap = heightmap;
}

void TerrainMaterial::SetSplatmap(Texture* splatmap) {
	this->splatmap = splatmap;
}

void TerrainMaterial::SetAmbientColor(glm::vec3 ambientColor) {
	this->ambientColor = glm::vec4(ambientColor, this->ambientColor.w);
}

void TerrainMaterial::SetAmbientIntensity(float intensity) {
	ambientColor.w = intensity;
}

void TerrainMaterial::SetSpecularColor(glm::vec3 specularColor) {
	this->specularColor = glm::vec4(specularColor, this->specularColor.w);
}

void TerrainMaterial::SetSpecularIntensity(float intensity) {
	specularColor.w = intensity;
}

void TerrainMaterial::SetShininess(float shininess) {
	this->shininess = shininess;
}

void TerrainMaterial::SetHeight(float height) {
	this->height = height;
}

void TerrainMaterial::SetNormalStepSize(float normalStepSize) {
	this->normalStepSize = normalStepSize;
}

void TerrainMaterial::render(World* world, Mesh* mesh, const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	_shader->use();

	eye = world->getMainCamera()->GetWorldPosition();
	float change = 0;
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::J)) change -= 1;
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::K)) change += 1;

	normalStepSize += change * 0.01 * mge::Time::DeltaTime();
	std::cout << "Normal step size: " << normalStepSize << "\n"; 

	const size_t lightCount = std::min(world->getLightCount(), TerrainMaterial::MAX_LIGHTS);
	for (size_t i = 0; i < lightCount; i++) {
		const LightData ld = world->getLightAt(i)->GetLightData();
		const std::string lStr = utils::str::string_format("lights[%u]", i);
		glUniform1ui(_shader->getUniformLocation(lStr + ".type"), static_cast<uint32_t>(ld.type));
		glUniform3fv(_shader->getUniformLocation(lStr + ".direction"), 1, glm::value_ptr(ld.direction));
		glUniform3fv(_shader->getUniformLocation(lStr + ".position"), 1, glm::value_ptr(ld.position));
		glUniform4fv(_shader->getUniformLocation(lStr + ".ambient"), 1, glm::value_ptr(ld.ambient));
		glUniform3fv(_shader->getUniformLocation(lStr + ".diffuse"), 1, glm::value_ptr(ld.diffuse));
		glUniform3fv(_shader->getUniformLocation(lStr + ".specular"), 1, glm::value_ptr(ld.specular));
		glUniform3fv(_shader->getUniformLocation(lStr + ".attenuation"), 1, glm::value_ptr(ld.attenuation));
		glUniform1f(_shader->getUniformLocation(lStr + ".innerCutOff"), ld.innerCutOff);
		glUniform1f(_shader->getUniformLocation(lStr + ".outerCutOff"), ld.outerCutOff);
	}
	glUniform1ui(_shader->getUniformLocation("lightCount"), lightCount);

	glUniform4fv(_shader->getUniformLocation("material.specular"), 1, glm::value_ptr(specularColor));
	glUniform1f(_shader->getUniformLocation("material.shininess"), shininess);
	glUniform1f(_shader->getUniformLocation("terrainVert.height"), height);
	glUniform1f(_shader->getUniformLocation("terrainFrag.height"), height);
	glUniform1f(_shader->getUniformLocation("terrainVert.normalStepSize"), normalStepSize);
	glUniform1f(_shader->getUniformLocation("terrainFrag.uvS"), normalStepSize);

	// Base
	utils::gl::PassTexture(_shader, "terrainVert.heightmap", 0, heightmap->getId());
	utils::gl::PassTexture(_shader, "terrainFrag.heightmap", 0, heightmap->getId());
	utils::gl::PassTexture(_shader, "terrainVert.splatmap", 1, splatmap->getId());
	utils::gl::PassTexture(_shader, "terrainFrag.splatmap", 1, splatmap->getId());

	utils::gl::PassTexture(_shader, "terrainFrag.baseTexture", 2, baseTexture->getId());
	glUniform1f(_shader->getUniformLocation("terrainFrag.baseSize"), textureSizes.x);
	utils::gl::PassTexture(_shader, "terrainFrag.textureR", 3, textureR->getId());
	glUniform1f(_shader->getUniformLocation("terrainFrag.sizeR"), textureSizes.y);
	utils::gl::PassTexture(_shader, "terrainFrag.waterA", 4, textureWaterA->getId());
	glUniform1f(_shader->getUniformLocation("terrainFrag.sizeWaterA"), textureSizes.z);
	utils::gl::PassTexture(_shader, "terrainFrag.waterB", 5, textureWaterB->getId());
	glUniform1f(_shader->getUniformLocation("terrainFrag.sizeWaterB"), textureSizes.z);
	utils::gl::PassTexture(_shader, "terrainFrag.textureB", 6, textureB->getId());
	glUniform1f(_shader->getUniformLocation("terrainFrag.sizeB"), textureSizes.w);
	glUniform1f(_shader->getUniformLocation("time"), mge::Time::TotalTime());
	glUniform1i(_shader->getUniformLocation("shouldTriplanar"), sf::Keyboard::isKeyPressed(sf::Keyboard::T));

	glUniform3fv(_shader->getUniformLocation("eye"), 1, glm::value_ptr(eye));

	//pass in all MVP matrices separately
	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	//now inform mesh of where to stream its data
	mesh->streamToOpenGL(
		0, 1, 2
	);
}
