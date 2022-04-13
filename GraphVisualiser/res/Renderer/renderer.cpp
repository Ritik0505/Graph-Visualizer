#include"renderer.h"

void VertexBuffer::prepareBuffer(const void* data,unsigned int size)
{
	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void VertexBuffer::bindBuffer() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &m_VBO);
}
