#version 450

struct TerrainData {
    sampler2D heightmap;
	float height;
	float normalStepSize;
};

layout (location = 0) in vec3 i_vertex;
layout (location = 1) in vec3 i_normal;
layout (location = 2) in vec2 i_uv;

layout (location = 0) uniform mat4 projectionMatrix;
layout (location = 1) uniform mat4 viewMatrix;
layout (location = 2) uniform mat4 modelMatrix;

uniform TerrainData terrainVert;

out vec2 uv;
out vec3 normal;
out vec3 fragPosition;

float height(vec2 position) {
	return textureLod(terrainVert.heightmap, position, 0).r;
}

void main() {
	float h = height(i_uv);
	vec3 vertex = vec3(i_vertex.x, terrainVert.height * h, i_vertex.z);
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertex, 1.0);

	vec3 norm;

	if(true) {
		vec2 uvLeft = i_uv + vec2(-0.5*terrainVert.normalStepSize, 0);
		vec2 uvRight = i_uv + vec2(0.5*terrainVert.normalStepSize, 0);
		vec2 uvUp = i_uv + vec2(0, 0.5*terrainVert.normalStepSize);
		vec2 uvDown = i_uv + vec2(0, -0.5*terrainVert.normalStepSize);

		float hLeft = height(uvLeft);
		float hRight = height(uvRight);
		float hUp = height(uvUp);
		float hDown = height(uvDown);

		// Get position of neighbouring vertices
		vec3 vertexLeft = vec3(i_vertex.x - terrainVert.normalStepSize, terrainVert.height * hLeft, i_vertex.z);
		vec3 vertexRight = vec3(i_vertex.x + terrainVert.normalStepSize, terrainVert.height * hRight, i_vertex.z);
		vec3 vertexUp = vec3(i_vertex.x, terrainVert.height * hUp, i_vertex.z + terrainVert.normalStepSize);
		vec3 vertexDown = vec3(i_vertex.x, terrainVert.height * hDown, i_vertex.z - terrainVert.normalStepSize);

		// Calculate direction from current vertex to the neighbouring vertices;
		vec3 vertLeft = normalize(vertexLeft - vertex);
		vec3 vertRight = normalize(vertexRight - vertex);
		vec3 vertUp = normalize(vertexUp - vertex);
		vec3 vertDown = normalize(vertexDown - vertex);

		// Calculate normals based on that
		vec3 normA = cross(vertLeft, vertUp);
		vec3 normB = cross(vertDown, vertLeft);
		vec3 normC = cross(vertUp, vertRight);
		vec3 normD = cross(vertRight, vertDown);

		// Average
		//norm = normalize(normA);
		norm = (normA + normB + normC + normD) / 4.0;
		norm = vec3(norm.x, norm.y, -norm.z);
		norm = normalize(norm);
	} else {
		// Calculate that damn normal by first obtaining the height of 4 surrounding vertices 
		vec2 uvLeft = i_uv + vec2(-0.5*terrainVert.normalStepSize, 0);
		vec2 uvRight = i_uv + vec2(0.5*terrainVert.normalStepSize, 0);
		vec2 uvUp = i_uv + vec2(0, 0.5*terrainVert.normalStepSize);
		vec2 uvDown = i_uv + vec2(0, -0.5*terrainVert.normalStepSize);

		float hLeft = height(uvLeft);
		float hRight = height(uvRight);
		float hUp = height(uvUp);
		float hDown = height(uvDown);

		// Take the difference left->right and down->up
		float nX = (hLeft - hRight)/2.0;
		float nY = terrainVert.normalStepSize;
		float nZ = (hUp - hDown)/2.0;
		norm = normalize(vec3(nX, nY, nZ));
	}

	
	uv = i_uv;
	normal = vec3(modelMatrix * vec4(norm, 0.0f));
	//normal = norm;
	fragPosition = vec3(modelMatrix * vec4(vertex, 1.0));
}