#pragma once

#include <array>
#include <cstdint>
#include <glm/ext/vector_float4.hpp>
#include <glm/glm.hpp>

#include "../neuron.hpp"

/**
 * @struct Sphere
 * @brief Represent a sphere in space with a given color.
 */
struct Sphere
{
	glm::vec4 pos_rad; // xyz = position, w = radius
	glm::vec4 color;   // rgba
};

/**
 * @struct Point
 * @brief Represent a point of a line in space with a given color.
 */
struct Point
{
	glm::vec3 pos;   // xyz = position
	glm::vec4 color; // rgba
};

/**
 * @struct NeuronVisual
 * @brief Describe a neuron visual data.
 */
struct NeuronVisual
{
	NeuronModelType::Enum type;
	glm::vec4 color;
	float size;
};

/**
 * @brief Return all neuron visual data.
 * @return 
 */
consteval auto get_neuron_visual_data()
{
	return std::array{
		// Cortex
		NeuronVisual{NeuronModelType::CortexPyramidalCell,       {0.1f, 0.4f, 1.0f, 1.0f}, 4.5f},
		NeuronVisual{NeuronModelType::CortexSpinyStellateCell,   {0.2f, 0.5f, 1.0f, 1.0f}, 3.0f},
		NeuronVisual{NeuronModelType::CortexLayer5PyramidalCell, {0.0f, 0.2f, 0.8f, 1.0f}, 8.0f},
		NeuronVisual{NeuronModelType::CortexChatteringCell,      {0.4f, 0.6f, 1.0f, 1.0f}, 4.5f},
		NeuronVisual{NeuronModelType::CortexBasketCell,          {0.0f, 0.8f, 0.8f, 1.0f}, 5.0f},
		NeuronVisual{NeuronModelType::CortexNonBasketCell,       {0.0f, 0.6f, 0.6f, 1.0f}, 3.5f},
		NeuronVisual{NeuronModelType::CortexMartinottiCell,      {0.0f, 0.5f, 0.5f, 1.0f}, 3.5f},
		NeuronVisual{NeuronModelType::CortexLayer1NonBasketCell, {0.2f, 0.7f, 0.7f, 1.0f}, 3.0f},

		// Thalamus
		NeuronVisual{NeuronModelType::ThalamusThalamocorticalRelay,   {0.6f, 0.1f, 0.8f, 1.0f}, 6.0f},
		NeuronVisual{NeuronModelType::ThalamusReticularNucleusNeuron, {0.8f, 0.2f, 1.0f, 1.0f}, 7.0f},
		NeuronVisual{NeuronModelType::ThalamusTIn_FS,                 {0.4f, 0.0f, 0.6f, 1.0f}, 3.5f},
		NeuronVisual{NeuronModelType::ThalamusTIn_LTS,                {0.5f, 0.1f, 0.7f, 1.0f}, 3.5f},

		// Hippocampus
		NeuronVisual{NeuronModelType::HippocampusCA3PyramidalCell, {0.0f, 0.8f, 0.0f, 1.0f}, 7.0f},
		NeuronVisual{NeuronModelType::HippocampusCA1PyramidalCell, {0.2f, 0.9f, 0.2f, 1.0f}, 5.0f},
		NeuronVisual{NeuronModelType::HippocampusGranuleCell,      {0.4f, 1.0f, 0.4f, 1.0f}, 3.0f},
		NeuronVisual{NeuronModelType::HippocampusBasketCell,       {0.6f, 0.8f, 0.0f, 1.0f}, 5.0f},
		NeuronVisual{NeuronModelType::HippocampusOLMCell,          {0.5f, 0.7f, 0.1f, 1.0f}, 4.5f},

		// Basal Ganglia
		NeuronVisual{NeuronModelType::BasalGangliaMediumSpinyNeuron,   {1.0f, 0.5f, 0.0f, 1.0f}, 4.5f},
		NeuronVisual{NeuronModelType::BasalGangliaStriatalFastSpiking, {0.8f, 0.4f, 0.0f, 1.0f}, 5.0f},
		NeuronVisual{NeuronModelType::BasalGangliaPallidalNeuron,      {1.0f, 0.6f, 0.2f, 1.0f}, 8.0f},

		// Brainstem
		NeuronVisual{NeuronModelType::BrainstemDopaminergicNeuron,  {1.0f, 0.0f, 0.0f, 1.0f}, 7.0f},
		NeuronVisual{NeuronModelType::BrainstemCholinergicNeuron,   {0.8f, 0.1f, 0.1f, 1.0f}, 7.0f},
		NeuronVisual{NeuronModelType::BrainstemMesencephalicNeuron, {0.9f, 0.2f, 0.2f, 1.0f}, 7.0f},

		// Cerebellum
		NeuronVisual{NeuronModelType::CerebellumPurkinjeCell, {1.0f, 0.9f, 0.0f, 1.0f}, 12.0f},
		NeuronVisual{NeuronModelType::CerebellumGranuleCell,  {1.0f, 1.0f, 0.4f, 1.0f}, 2.0f},
		NeuronVisual{NeuronModelType::CerebellumGolgiCell,    {0.8f, 0.7f, 0.0f, 1.0f}, 5.0f},
		NeuronVisual{NeuronModelType::CerebellumStellateCell, {0.9f, 0.8f, 0.2f, 1.0f}, 3.0f},

		// Spinal Cord
		NeuronVisual{NeuronModelType::SpinalCordAlphaMotorNeuron, {0.6f, 0.4f, 0.2f, 1.0f}, 15.0f},
		NeuronVisual{NeuronModelType::SpinalCordGammaMotorNeuron, {0.7f, 0.5f, 0.3f, 1.0f}, 7.0f},
		NeuronVisual{NeuronModelType::SpinalCordRenshawCell,      {0.4f, 0.3f, 0.2f, 1.0f}, 4.5f},

		// Amygdala
		NeuronVisual{NeuronModelType::AmygdalaBasolateralPyramidal, {1.0f, 0.4f, 0.7f, 1.0f}, 5.0f},
		NeuronVisual{NeuronModelType::AmygdalaCentralNucleusNeuron, {0.8f, 0.2f, 0.5f, 1.0f}, 3.5f},

		// Olfactory Bulb
		NeuronVisual{NeuronModelType::OlfactoryBulbMitralCell,         {0.0f, 1.0f, 0.8f, 1.0f}, 8.0f},
		NeuronVisual{NeuronModelType::OlfactoryBulbTuftedCell,         {0.2f, 0.9f, 0.7f, 1.0f}, 5.0f},
		NeuronVisual{NeuronModelType::OlfactoryBulbPeriglomerularCell, {0.0f, 0.7f, 0.5f, 1.0f}, 2.5f},
	};
}

consteval auto InitNeuronColors()
{
	std::array<glm::vec4, static_cast<uint32_t>(NeuronModelType::Count)> colors{};
	for (auto const &e : get_neuron_visual_data()) {
		colors[static_cast<uint32_t>(e.type)] = e.color;
	}
	return colors;
}

/**
 * @brief Return the array of the soma diameters.
 */
consteval auto InitSomaDiameters()
{
	std::array<float, static_cast<uint32_t>(NeuronModelType::Count)> sizes{};
	for (auto const &e : get_neuron_visual_data()) {
		sizes[static_cast<uint32_t>(e.type)] = e.size;
	}
	return sizes;
}

constexpr auto SomaDiameters = InitSomaDiameters();
constexpr auto NeuronColors = InitNeuronColors();
