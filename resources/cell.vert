#version 430 core
layout (location = 0) in vec2 aQuadPos; // Un simple carré de (-1,-1) à (1,1)

struct Cell {
	vec4 pos_rad;
	vec4 color;
};

layout(std430, binding = 0) buffer CellBuffer {
	Cell cells[];
};

uniform mat4 view;
uniform mat4 projection;

out vec2 vUV;
out vec4 vColor;
out vec3 vPosEye;
out float vRadius;

void main() {
	Cell c = cells[gl_InstanceID];
	vColor = c.color;
	vRadius = c.pos_rad.w;
	vUV = aQuadPos; // Coordonnées de -1 à 1

	// On place le centre de la sphère en "Eye Space" (espace caméra)
	vec4 viewCenter = view * vec4(c.pos_rad.xyz, 1.0);
	
	// On décale les coins du carré pour qu'il soit toujours face à la caméra
	vec4 viewPos = viewCenter + vec4(aQuadPos * vRadius, 0.0, 0.0);
	vPosEye = viewPos.xyz;

	gl_Position = projection * viewPos;
}