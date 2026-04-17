#pragma once

#include <array>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/ext/vector_float3.hpp>

#include "../../entities/entity.hpp"
#include "../../entities/entities.h"
#include "../../utils/octree.hpp"
#include "../../neuron.hpp"
#include "src/render/structs.hpp"
#include "structs.hpp"



/**
 * @struct AxialCoord
 * @brief The axial coordinate of one microcolumn in the column.
 */
struct AxialCoord
{
	int32_t q; // Column.
	int32_t r; // Row.
};

/**
 * @brief AxialCoord LUT generator.
 * @return A compilation-time computed LUT.
 */
constexpr inline std::array<AxialCoord, 91> generate_id_to_ax_lut()
{
	std::array<AxialCoord, 91> lut{};
	int32_t id = 0;
	for (int32_t r = -5; r <= 5; ++r) {
		int32_t r1 = (r < 0) ? -5 - r : -5;
		int32_t r2 = (r < 0) ? 5 : 5 - r;
		for (int32_t q = r1; q <= r2; ++q) lut[id++] = {q, r};
	}
	return lut;
}

// AxialCoord LUT.
constexpr inline auto ID_TO_AX_LUT = generate_id_to_ax_lut();

/**
 * @brief Convert an id to axial coordinates.
 * @param id The id to convert.
 * @return The computed coordinates.
 */
constexpr inline AxialCoord id_to_ax(int32_t id)
{
	assert(id >= 0 && id < 91 && "Microcolumn index out of bounds.");
	return ID_TO_AX_LUT[id];
}

/**
 * @brief Convert axial coordinates to an id.
 * @param coord The coordinates to convert.
 * @return The computed id.
 */
constexpr inline int32_t ax_to_id(AxialCoord coord)
{
	int32_t row_start = (coord.r <= 0) ? 
		(6 * (coord.r + 5) + ((coord.r + 5) * (coord.r + 4)) / 2) : 
		(40 + 11 * coord.r - (coord.r * (coord.r - 1)) / 2);
	int32_t q_offset = (coord.r <= 0) ? (coord.q + coord.r + 5) : (coord.q + 5);
	return row_start + q_offset;
}



/**
 * @struct MicroColumnDescriptor
 * @brief Geometrical parameters of one microcolumn.
 */
struct MicroColumnDescriptor
{
	float radius;              // Radius in µm.
	float height;              // Height in µm.
	glm::vec3 direction;       // Normalized up direction of the column.
	glm::vec3 position;        // Column base position in space.
};

/**
 * @struct MicroColumn
 * @brief Store a microcolumn informations.
 */
struct MicroColumn
{
	std::vector<Entity> somas;        // List of all somas of this microcolumn for fast access.
	MicroColumnDescriptor descriptor; // Descriptor of this microcolumn.
};

/**
 * @struct LayerRecipe
 * @brief Struct used to describe a layer composition for the generator.
 */
struct LayerRecipe
{
	uint32_t count;             // Number of neurons of this type in the layer.
	NeuronModelType::Enum type; // Type of the neuron to put in the layer.
};



/**
 * @struct Column
 * @brief Contains all data of one column.
 */
struct Column
{
	std::array<MicroColumn, 91> m_microcolumns; // List of all microcolumns in this column.
	std::vector<LayerRecipe> recipes[6];        // Column composition per microcolumn.
	float bounds[6];                            // Normalized end of each cortical layer.
};

Column new_column();



void draw_microcolumn_bounds(
	const Column &column, 
	AxialCoord coord, 
	std::vector<Point> &out_lines, 
	glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)
);



Position generate_position_in_layer(
	const MicroColumnDescriptor &desc,
	float start_pct,
	float end_pct
);

void build_microcolumn_somas(
	Entities &entities,
	Octree<OctreeEntity> &octree,
	MicroColumn &column,
	const std::vector<LayerRecipe> recipes[6],
	const float layer_boundaries[6]
);

void build_column(
	Entities &entities,
	Octree<OctreeEntity> &octree,
	Column &column
);
