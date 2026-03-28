#pragma once

#include <GL/glew.h>
#include <cstddef>

#include "types.h"



/**
 * @class SSBO
 * @brief Wrapper arround OpenGL SSBOs.
 */
class SSBO 
{
public:

	SSBO(GPUType type, size_t size, const void *buffer = nullptr);
	SSBO(const SSBO& b) = delete;
	SSBO& operator=(const SSBO& b) = delete;
	SSBO(SSBO&& other) noexcept : 
		m_id(other.m_id), 
		m_type(other.m_type), 
		m_length(other.m_length), 
		m_size(other.m_size)
	{ 
		other.m_id = 0; 
		other.m_type = GPUType::GPU_UNKNOWN;
		other.m_length = 0;
		other.m_size = 0;
	}

	SSBO& operator=(SSBO&& other) noexcept
	{
		if (this != &other) {
			if (m_id) glDeleteBuffers(1, &m_id);
			m_id = other.m_id;
			m_type = other.m_type;
			m_length = other.m_length;
			m_size = other.m_size;
			other.m_id = 0;
			other.m_type = GPUType::GPU_UNKNOWN; 
			other.m_length = 0;
			other.m_size = 0;
		}
		return *this;
	}

	~SSBO() { if (m_id) glDeleteBuffers(1, &m_id); }

	uint32_t id() const { return m_id; }
	GPUType type() const { return m_type; }
	size_t length() const { return m_length; }
	size_t size() const { return m_size; }
	bool isInit() const { return m_id; }

protected:

	uint32_t m_id;   // The OpenGL ssbo ID.
	GPUType m_type;  // The data type of the ssbo.
	size_t m_length; // The number of elements in the ssbo.
	size_t m_size;   // The size in bytes of the ssbo.

};
