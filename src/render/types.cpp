#include "types.h"




/**
 * @brief This function init the gpu types descriptors array.
 */
constexpr std::array<GPUTypeDescriptor, static_cast<size_t>(GPUType::_GPU_COUNT)> make_gputype_descriptors()
{
    std::array<GPUTypeDescriptor, GPUType::_GPU_COUNT> array;
    array[GPUType::GPU_UNKNOWN] = {
        "unknown", 0, 0, 0, false
    };
    array[GPUType::GPU_FLOAT] = {
        "float", sizeof(float), sizeof(float), 1, false
    };
    array[GPUType::GPU_INT] = {
        "int", sizeof(int32_t), sizeof(int32_t), 1, true
    };
    array[GPUType::GPU_UINT] = {
        "uint", sizeof(uint32_t), sizeof(uint32_t), 1, true
    };
    array[GPUType::GPU_BOOL] = {
        "bool", sizeof(uint32_t), sizeof(uint32_t), 1, true
    };
    array[GPUType::GPU_VEC2] = {
        "vec2", sizeof(float)*2, sizeof(float)*2, 2, false
    };
    array[GPUType::GPU_VEC3] = {
        "vec3", sizeof(float)*3, sizeof(float)*4, 3, false
    };
    array[GPUType::GPU_VEC4] = {
        "vec4", sizeof(float)*4, sizeof(float)*4, 4, false
    };
    return array;
}

// The GPUType descriptors array.
constexpr std::array<GPUTypeDescriptor, static_cast<size_t>(GPUType::_GPU_COUNT)> 
	gputype_descriptors = make_gputype_descriptors();

