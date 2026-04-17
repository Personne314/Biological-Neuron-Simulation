#include "microcolumn.h"

#include <random>



/**
 * @brief Generate a random position inside a specific oriented cylindrical layer.
 */
Position generate_position_in_layer(const MicroColumnDescriptor& desc, float start_pct, float end_pct)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist_radius(0.0f, 1.0f);
	std::uniform_real_distribution<float> dist_angle(0.0f, 2.0f * std::numbers::pi_v<float>);
	std::uniform_real_distribution<float> dist_height(start_pct, end_pct);

	// 1. Position locale dans un cylindre "droit" (aligné sur l'axe Y)
	float r = desc.radius * std::sqrt(dist_radius(gen)); // sqrt pour distribution uniforme dans le disque
	float theta = dist_angle(gen);
	float h = dist_height(gen) * desc.height;

	float local_x = r * std::cos(theta);
	float local_y = h;
	float local_z = r * std::sin(theta);

	// 2. Création d'une base orthonormée à partir de la direction de la colonne
	glm::vec3 up = glm::normalize(desc.direction);
	glm::vec3 world_up = std::abs(up.y) < 0.999f ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 right = glm::normalize(glm::cross(world_up, up));
	glm::vec3 forward = glm::cross(up, right);

	// 3. Transformation vers l'espace global
	glm::vec3 final_pos = desc.position + (right * local_x) + (up * local_y) + (forward * local_z);

	return {final_pos.x, final_pos.y, final_pos.z};
}


/**
 * @brief Create a microcolumn entity and populate it with somas based on the provided recipes.
 */
Entity build_microcolumn(Entities& entities, Octree<OctreeEntity>& octree, const MicroColumnDescriptor& desc, const std::vector<LayerRecipe> recipes[6])
{
	Entity col_entity = entities.make_entity();
	MicroColumn mc;
	mc.descriptor = desc;

	// Estimation de la taille pour éviter les réallocations du vecteur
	uint32_t total_somas = 0;
	for (int i = 0; i < 6; ++i) {
		for (const auto& recipe : recipes[i]) total_somas += recipe.count;
	}
	mc.somas.reserve(total_somas);

	float start_pct = 0.0f;

	// Parcours des 6 couches
	for (int i = 0; i < 6; ++i) {
		float end_pct = desc.layer_boundaries[i];

		// Parcours des types de neurones demandés pour cette couche
		for (const auto& recipe : recipes[i]) {
			for (uint32_t j = 0; j < recipe.count; ++j) {
				
				Position pos = generate_position_in_layer(desc, start_pct, end_pct);
				
				Soma soma = {0, 0, recipe.type}; // axon et dendrite roots à 0 pour l'instant
				Entity soma_entity = make_soma_entity(entities, octree, soma, pos);
				
				mc.somas.push_back(soma_entity);
			}
		}
		
		start_pct = end_pct; // Le début de la couche suivante est la fin de l'actuelle
	}

	// On ajoute le composant MicroColumn à l'entité colonne
	entities.add_component<MicroColumn>(col_entity, mc);
	// Optionnel : donner une position à la colonne elle-même pour les recherches spatiales globales
	entities.add_component<Position>(col_entity, {desc.position.x, desc.position.y, desc.position.z});

	return col_entity;
}
