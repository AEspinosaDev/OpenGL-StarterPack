/*
    This file is part of OpenGL-StarterPack (GLSP), an open source OpenGL based framework
    that facilitates and speeds up demo and project creation by offering an abstraction to
    the basic objects of OpenGL as well as incluing the necessary libraries.

    MIT License

    Copyright (c) 2024 Antonio Espinosa Garcia

*/
#include <GLSP/buffers.h>

GLSP_NAMESPACE_BEGIN
VertexBuffer::VertexBuffer(void *data, const size_t sizeInBytes) : Buffer(), m_strideBytes(0), m_data(nullptr), m_totalBytes(sizeInBytes)
{
    m_data = malloc(sizeInBytes);
    memcpy(m_data, data, sizeInBytes);
}

VertexBuffer::~VertexBuffer()
{

}

void VertexBuffer::upload_data()
{
    bind();
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, m_totalBytes, m_data, GL_STATIC_DRAW));
    unbind();
}
void VertexBuffer::generate()
{
    GL_CHECK(glGenBuffers(1, &m_id));
    m_generated = true;
}

void VertexBuffer::bind() const
{
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_id));
}

void VertexBuffer::unbind() const
{
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void VertexBuffer::bind_base(unsigned int type, unsigned int readLayout) const
{
    GL_CHECK(glBindBufferBase(type, readLayout, m_id));
}
void VertexBuffer::read_data(void *readData, size_t offset, size_t sizeInBytes) const
{
    bind();
    GL_CHECK(glGetBufferSubData(GL_ARRAY_BUFFER, offset, sizeInBytes == 0 ? m_totalBytes : sizeInBytes, readData));
    unbind();
}
VertexArray::~VertexArray()
{
    //  GL_CHECK(glDeleteVertexArrays(1, &m_id))
}

void VertexArray::generate()
{
    GL_CHECK(glGenVertexArrays(1, &m_id));
    bind();
    for (VertexBuffer &vbo : m_VBOs)
    {
        vbo.generate();
        vbo.upload_data();
        vbo.bind();
        size_t offset = 0;
        const auto &layouts = vbo.get_layouts();
        for (int i = 0; i < layouts.size(); i++)
        {
            const auto &layout = layouts[i];
            GL_CHECK(glEnableVertexAttribArray(m_layoutCount));
            GL_CHECK(glVertexAttribPointer(m_layoutCount, layout.count, layout.type,
                                           layout.normalized, vbo.get_stride_size(), (void *)offset));
            offset += layout.count * AttributeLayout::get_size(layout.type);
            m_layoutCount++;
        }
    }
    unbind();
    if (m_layoutCount == 0)
    {
        if (m_VBOs.empty())
        {
            ERR_LOG("VAO Error:: does not have ANY vertex buffers !");
        }
        else
        {
            ERR_LOG("VAO Error:: some vertex buffers do not have ANY layouts defined !");
        }
        return;
    }
    m_generated = true;
}

void VertexArray::bind() const
{
    GL_CHECK(glBindVertexArray(m_id));
}

void VertexArray::unbind() const
{
    GL_CHECK(glBindVertexArray(0));
}

void VertexArray::push_vertex_buffer(const VertexBuffer vbo)
{
    m_VBOs.push_back(vbo);
}

void VertexArray::set_layout_divisor(const unsigned int divisor)
{
    GL_CHECK(glVertexAttribDivisor(m_layoutCount - 1, divisor));
}

IndexBuffer::~IndexBuffer()
{
}

void IndexBuffer::generate()
{
    GL_CHECK(glGenBuffers(1, &m_id));
    m_generated = true;
}

void IndexBuffer::upload_data()
{
    ASSERT(sizeof(GLuint) == sizeof(unsigned int));
    bind();
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_totalBytes, m_indices.data(), GL_STATIC_DRAW));
    // unbind();
}
void IndexBuffer::bind() const
{
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id));
}

void IndexBuffer::unbind() const
{
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id));
}

void UniformBuffer::generate()
{
    GL_CHECK(glGenBuffers(1, &m_id));
    GL_CHECK(glBindBuffer(GL_UNIFORM_BUFFER, m_id));
    GL_CHECK(glBufferData(GL_UNIFORM_BUFFER, BYTES, NULL, GL_STATIC_DRAW));
    GL_CHECK(glBindBuffer(GL_UNIFORM_BUFFER, 0));

    size_t i = 0;
    for (Layout &layout : m_layouts)
    {
        GL_CHECK(glBindBufferRange(GL_UNIFORM_BUFFER, m_layoutBinding + i, m_id, layout.offset, layout.bytes));
        i++;
    }
    m_generated = true;
}

void UniformBuffer::upload_data(const size_t sizeInBytes, const void *data, const size_t offset) const
{
    GL_CHECK(glBindBuffer(GL_UNIFORM_BUFFER, m_id));
    glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeInBytes, data);
    GL_CHECK(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

void UniformBuffer::bind() const
{
    GL_CHECK(glBindBuffer(GL_UNIFORM_BUFFER, m_id));
}

void UniformBuffer::unbind() const
{
    GL_CHECK(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

UniformBuffer::~UniformBuffer(){
    GL_CHECK(glDeleteBuffers(1, &m_id))}

GLSP_NAMESPACE_END
