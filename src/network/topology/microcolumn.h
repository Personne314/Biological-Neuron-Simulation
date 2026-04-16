#pragma once

#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/ext/vector_float3.hpp>

#include "../../entities/entity.hpp"
#include "../../entities/entities.h"
#include "../../utils/octree.hpp"
#include "../../neuron.hpp"
#include "structs.hpp"



struct MicroColumnDescriptor
{
	float radius; 
	float height;
	glm::vec3 direction; // Doit être normalisé
	glm::vec3 position;  // Position de la base de la colonne (Couche VI)
	float layer_boundaries[6]; // Fin de chaque couche normalisée (ex: 0.1, 0.25, 0.4, 0.7, 0.9, 1.0)
};

struct MicroColumn
{
	std::vector<Entity> somas;
	MicroColumnDescriptor descriptor;
};

// Recette de génération pour définir le contenu d'une couche
struct LayerRecipe
{
	uint32_t count;
	NeuronModelType::Enum type;
};


Position generate_position_in_layer(
	const MicroColumnDescriptor& desc, 
	float start_pct, 
	float end_pct
);

Entity build_microcolumn(
	Entities& entities, 
	Octree<OctreeEntity>& octree, 
	const MicroColumnDescriptor& desc, 
	const std::vector<LayerRecipe> recipes[6]
);
