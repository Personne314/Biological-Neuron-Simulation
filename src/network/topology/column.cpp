#include "column.h"

#include <cstdint>
#include <random>



/**
 * @brief Create a new empty column.
 * @return The created column with predefined recipes and bounds.
 */
Column new_column()
{
	Column column = { {}, {}, {0.1f, 0.25f, 0.4f, 0.7f, 0.9f, 1.0f} };
	column.recipes[0].push_back({3,  NeuronModelType::CortexLayer1NonBasketCell});
	column.recipes[1].push_back({15, NeuronModelType::CortexPyramidalCell});
	column.recipes[1].push_back({4,  NeuronModelType::CortexBasketCell});
	column.recipes[2].push_back({10, NeuronModelType::CortexPyramidalCell});
	column.recipes[2].push_back({3,  NeuronModelType::CortexChatteringCell});
	column.recipes[3].push_back({20, NeuronModelType::CortexSpinyStellateCell});
	column.recipes[4].push_back({10, NeuronModelType::CortexLayer5PyramidalCell});
	column.recipes[4].push_back({3,  NeuronModelType::CortexMartinottiCell});
	column.recipes[5].push_back({12, NeuronModelType::CortexPyramidalCell});
	return column;
}



/**
 * @brief Generate visual bounding lines (hexagonal prism) for a microcolumn.
 * @param column The column containing the microcolumn to draw.
 * @param coord The axial coordinates of the microcolumn.
 * @param out_lines The vector to append the lines to.
 * @param color The color of the lines (default red).
 */
void draw_microcolumn_bounds(
	const Column &column, 
	AxialCoord coord, 
	std::vector<Point> &lines, 
	glm::vec4 color
)
{
	// Récupération de l'ID via les coordonnées axiales (O(1))
	int32_t id = ax_to_id(coord);
	const MicroColumnDescriptor &desc = column.m_microcolumns[id].descriptor;

	constexpr float margin = 5.0f;
	float spacing = (desc.radius * 2.0f) + margin;
	float hex_radius = spacing / 1.73205080757f; // spacing / sqrt(3)

	// Create a orthonormal basis for the microcolumn.
	glm::vec3 up = glm::normalize(desc.direction);
	glm::vec3 world_up = std::abs(up.y) < 0.999f ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 right = glm::normalize(glm::cross(world_up, up));
	glm::vec3 forward = glm::cross(up, right);

	std::array<glm::vec3, 6> bottom_pts;
	std::array<glm::vec3, 6> top_pts;

	// Calculate the 6 vertices of the hexagon.
	for (int i = 0; i < 6; ++i) {
		float angle = i * std::numbers::pi_v<float> / 3.0f;
		float local_x = hex_radius * std::cos(angle);
		float local_z = hex_radius * std::sin(angle);

		bottom_pts[i] = desc.position + (right * local_x) + (forward * local_z);
		top_pts[i] = bottom_pts[i] + (up * desc.height);
	}

	// Connect the vertices to form the prism.
	for (int i = 0; i < 6; ++i) {
		int next = (i + 1) % 6;

		// Bottom hexagon
		lines.push_back({bottom_pts[i], color});
		lines.push_back({bottom_pts[next], color});

		// Top hexagon
		lines.push_back({top_pts[i], color});
		lines.push_back({top_pts[next], color});

		// Vertical pillars connecting bottom and top
		lines.push_back({bottom_pts[i], color});
		lines.push_back({top_pts[i], color});
	}

}



/**
 * @brief Generate a random position inside a specific oriented cylindrical layer.
 * @param desc The microcolumn descriptor.
 * @param start_norm The normalized layer starting level.
 * @param end_norm The normalized layer ending level.
 * @return The random neuron position.
 */
Position generate_position_in_layer(const MicroColumnDescriptor &desc, float start_norm, float end_norm)
{

	// Setup random distributions.
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist_radius(0.0f, 1.0f);
	std::uniform_real_distribution<float> dist_angle(0.0f, 2.0f * std::numbers::pi_v<float>);
	std::uniform_real_distribution<float> dist_height(start_norm, end_norm);

	// Get random coordinates in cylindrical coordinates.
	float r = desc.radius * std::sqrt(dist_radius(gen));
	float theta = dist_angle(gen);
	float h = dist_height(gen) * desc.height;

	// Convert this into carthesian coordinates.
	float local_x = r * std::cos(theta);
	float local_y = h;
	float local_z = r * std::sin(theta);

	// Create a orthonormal basis for the microcolumn. 
	glm::vec3 up = glm::normalize(desc.direction);
	glm::vec3 world_up = std::abs(up.y) < 0.999f ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 right = glm::normalize(glm::cross(world_up, up));
	glm::vec3 forward = glm::cross(up, right);

	// Transform the local coordinates into global coordinates.
	glm::vec3 final_pos = desc.position + (right * local_x) + (up * local_y) + (forward * local_z);
	return { final_pos.x, final_pos.y, final_pos.z };

}

/**
 * @brief Create a microcolumn entity and populate it with somas based on the provided recipes.
 * @param entities The entities used to create the somas.
 * @param octree The octree to put the soma in.
 * @param column The microcolumn to populate.
 * @param recipes Description of a microcolumn content.
 * @param layer_boundaries Normalized boundaries of cortical layers.
 */
void build_microcolumn_somas(
	Entities &entities,
	Octree<OctreeEntity> &octree,
	MicroColumn &column,
	const std::vector<LayerRecipe> recipes[6],
	const float layer_boundaries[6]	
) {

	// Get the number of somas.
	uint32_t total_somas = 0;
	for (uint32_t i = 0; i < 6; ++i) {
		for (const auto &recipe : recipes[i]) total_somas += recipe.count;
	}
	column.somas.reserve(total_somas);

	// Loop over the layer to add the neuron populations.
	float start_norm = 0.0f;
	for (int i = 0; i < 6; ++i) {
		const float end_norm = layer_boundaries[i];
		for (const auto &recipe : recipes[i]) {

			// For each neuron, get a random position in the layer, then create the soma.
			for (uint32_t j = 0; j < recipe.count; ++j) {
				Position pos = generate_position_in_layer(column.descriptor, start_norm, end_norm);
				Soma soma = {0, 0, recipe.type};
				Entity soma_entity = make_soma_entity(entities, octree, soma, pos);
				column.somas.push_back(soma_entity);
			}
		}
		start_norm = end_norm;
	}

}



/**
 * @brief Generate all column content.
 * @param entities The entities used to create the somas.
 * @param octree The octree to put the soma in.
 * @param column The column to populate.
 */
void build_column(
	Entities &entities,
	Octree<OctreeEntity> &octree,
	Column &column
) {
	constexpr float micro_radius = 30.0f;
	constexpr float margin = 5.0f;
	constexpr float spacing = (micro_radius * 2.0f) + margin;
	constexpr float sqrt3_over_2 = 0.86602540378f; // sqrt(3) / 2.
	constexpr glm::vec3 macro_center(0.0f, -1000.0f, 0.0f);
	
	// Loop over each microcolumn.
	for (int32_t id = 0; id < 91; ++id) {
		const AxialCoord coord = id_to_ax(id);
		
		// Get the carthesian coordinates.
		const float x = spacing * (coord.q + coord.r / 2.0f);
		const float z = spacing * sqrt3_over_2 * coord.r;
		
		// Initialize the microcolumn descriptor.
		MicroColumn &micro_column = column.m_microcolumns[id];
		micro_column.descriptor.radius = micro_radius;
		micro_column.descriptor.height = 2000.0f;
		micro_column.descriptor.direction = glm::vec3(0.0f, 1.0f, 0.0f);
		micro_column.descriptor.position = macro_center + glm::vec3(x, 0.0f, z);
		
		// Generate the microcolumn somas.
		build_microcolumn_somas(entities, octree, micro_column, column.recipes, column.bounds);

	}

}
