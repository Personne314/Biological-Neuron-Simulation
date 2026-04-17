#version 430 core

in vec2 vUV;
in vec4 vColor;
in float vRadius;
in vec3 vPosEye; // On récupère la position locale du billboard

uniform mat4 projection; // La matrice view a été retirée car inutile sans éclairage

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

    // 3. Application de la couleur brute (unlit)
    FragColor = vColor;
}