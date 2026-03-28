#version 430 core

in vec2 vUV;
in vec4 vColor;
in float vRadius;
in vec3 vPosEye; // On récupère la position locale du billboard

uniform mat4 view;
uniform mat4 projection; // On ajoute la matrice de projection

out vec4 FragColor;

void main()
{
	// 1. Découpage du disque
	float r2 = dot(vUV, vUV);
	if (r2 > 1.0) discard;

	// 2. Calcul du bombage en Z
	float z = sqrt(1.0 - r2);

	// --- CORRECTION DE LA PROFONDEUR ---
	// On recalcule la position 3D exacte du pixel sur la surface de la sphère.
	// En OpenGL, la caméra regarde vers -Z. Ajouter 'z * vRadius' rapproche
	// virtuellement le pixel de la caméra.
	vec4 realViewPos = vec4(vPosEye.xy, vPosEye.z + z * vRadius, 1.0);
	vec4 clipPos = projection * realViewPos;
	
	// On écrit cette nouvelle profondeur dans le Z-buffer
	gl_FragDepth = (clipPos.z / clipPos.w) * 0.5 + 0.5;
	// -----------------------------------

	// 3. Calcul de la normale et éclairage (inchangé)
	vec3 normalView = vec3(vUV, z);
	vec3 normalWorld = normalize(transpose(mat3(view)) * normalView);

	vec3 lightDirWorld = normalize(vec3(0.5, 1.0, 0.5));
	float diff = max(dot(normalWorld, lightDirWorld), 0.2);
	
	vec3 viewDirWorld = normalize(transpose(mat3(view)) * vec3(0.0, 0.0, 1.0));
	vec3 reflectDir = reflect(-lightDirWorld, normalWorld);
	float spec = pow(max(dot(viewDirWorld, reflectDir), 0.0), 32.0);

	FragColor = vec4(vColor.rgb * diff + spec * 0.3, vColor.a);
}