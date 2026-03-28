#pragma once

#include <cstddef>
#include <cstdint>
#include <array>


	
/**
 * @brief Constants for the GLSL types.
 */
enum GPUType
{
	GPU_UNKNOWN,	// Unknown type. This should not be used.
	GPU_FLOAT,		// Constant for type 'float'.
	GPU_INT,		// Constant for type 'int'.
	GPU_UINT,		// Constant for type 'uint'.
	GPU_BOOL,		// Constant for type 'bool'.
	GPU_VEC2,		// Constant for type 'vec2'.
	GPU_VEC3,		// Constant for type 'vec3'.
	GPU_VEC4,		// Constant for type 'vec4'.
	_GPU_COUNT		// The counter of the enum. Equals the number of constants. 
};



/**
 * @brief Descriptor of an GLSL type.
 */
struct GPUTypeDescriptor
{
    const char *name;          // The GLSL name of the type.
    size_t size_bytes;         // The real size of the type.
	size_t aligned_size_bytes; // The size aligned for SSBO usage.
	uint32_t dimension;        // The number of components of this type. 	
	bool is_int;               // true if this is a GLSL integer type.
};



/**
 * @brief Array of GPUType type descriptors. 
 */
extern const std::array<GPUTypeDescriptor, static_cast<size_t>(GPUType::_GPU_COUNT)> gputype_descriptors;
#define GPUTYPE_INFO(type) gputype_descriptors.at(static_cast<size_t>(type))
