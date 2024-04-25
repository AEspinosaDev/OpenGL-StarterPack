/*
    This file is part of OpenGL-StarterPack (GLSP), an open source OpenGL based framework
    that facilitates and speeds up demo and project creation by offering an abstraction to
    the basic objects of OpenGL as well as incluing the necessary libraries.

    MIT License

    Copyright (c) 2024 Antonio Espinosa Garcia

*/
#ifndef __BUFFERS__
#define __BUFFERS__

#include <vector>
#include <GLSP/core.h>

GLSP_NAMESPACE_BEGIN

// Ahead declare
class Buffer;
class VertexBuffer;
class IndexBuffer;
class VertexArray;
class UniformBuffer;

/*
Base abstract class for buffer objects
*/
class Buffer
{
protected:
    unsigned int m_id;

    bool m_generated{false};

public:
    Buffer() {}

    virtual void generate() = 0;
    virtual void bind() const = 0;
    virtual void unbind() const = 0;

    virtual inline unsigned int get_id() const { return m_id; }
    virtual inline bool is_generated() const { return m_generated; }
};

#pragma region VBO
/*
Utility struct for Vertex buffers
*/
struct AttributeLayout
{
    unsigned int type;
    size_t count;
    unsigned char normalized;

    static size_t get_size(unsigned int type)
    {
        switch (type)
        {
        case GL_FLOAT:
            return 4;
        case GL_UNSIGNED_INT:
            return 4;
        case GL_UNSIGNED_BYTE:
            return 1;
        }
        ASSERT(false);
        return 0;
    }
};

/*
OpenGL VBO abstraction
*/
class VertexBuffer : public Buffer
{
    std::vector<AttributeLayout> m_layouts;
    size_t m_strideBytes;
    size_t m_totalBytes;
    void *m_data;

public:
    VertexBuffer(void *data, const size_t sizeInBytes);

    ~VertexBuffer();

    void generate();

    /**
     * Copy data to the GPU
     */
    void upload_data();

    void bind() const;

    void unbind() const;

    /*
    Bind this buffer as read only
    */
    void bind_base(unsigned int type, unsigned int readLayout) const;

    /*
    Push and subscribes a new attribute layout to the VBO.
    */
    template <typename T>
    void push_attribute_layout(size_t itemCount)
    {
        ASSERT("Must have a type")
    }
    /*
        Push and subscribes a new FLOAT attribute layout to the VBO.
        */
    template <>
    void push_attribute_layout<float>(size_t itemCount)
    {
        m_layouts.push_back({GL_FLOAT, itemCount, GL_FALSE});
        m_strideBytes += itemCount * AttributeLayout::get_size(GL_FLOAT);
    }
    /*
          Push and subscribes a new UINT attribute layout to the VBO.
          */
    template <>
    void push_attribute_layout<unsigned int>(size_t itemCount)
    {
        m_layouts.push_back({GL_UNSIGNED_INT, itemCount, GL_FALSE});
        m_strideBytes += itemCount * AttributeLayout::get_size(GL_UNSIGNED_INT);
    }
    /*
          Push and subscribes a new UCHAR attribute layout to the VBO.
          */
    template <>
    void push_attribute_layout<unsigned char>(size_t itemCount)
    {
        m_layouts.push_back({GL_UNSIGNED_BYTE, itemCount, GL_TRUE});
        m_strideBytes += itemCount * AttributeLayout::get_size(GL_UNSIGNED_BYTE);
    }

    inline const std::vector<AttributeLayout> get_layouts() const { return m_layouts; }
    /*
    Returns read only size in bytes of the vertex  stride
    */
    inline const size_t get_stride_size() const { return m_strideBytes; }
    /*
       Returns read only size in bytes of the entire data
       */
    inline const size_t get_total_size() const { return m_totalBytes; }

    inline const size_t get_element_count() const { return m_totalBytes / m_strideBytes; }

    inline bool empty() const { return m_totalBytes == 0; }
    /*
    CAUTION !! Slow operation. Retrieves data from the GPU for reading purposes.
    */
    void read_data(void *readData, size_t offset = 0, size_t sizeInBytes = 0) const;
};
#pragma endregion
#pragma region IBO
/*
OpenGL IBO abstraction
*/
class IndexBuffer : public Buffer
{
    std::vector<unsigned int> m_indices;
    size_t m_totalBytes;

public:
    IndexBuffer(std::vector<unsigned int> indices) : Buffer(), m_indices(indices), m_totalBytes(indices.size() * sizeof(unsigned int)) {}
    ~IndexBuffer();

    void generate();

    /**
     * Copy data to the GPU
     */
    void upload_data();

    void bind() const;
    void unbind() const;

    /*
      Returns read only size in bytes of the entire data
      */
    inline const size_t get_total_size() const { return m_totalBytes; }

    inline bool empty() const { return m_totalBytes == 0; }

    inline std::vector<unsigned int> get_indices() const { return m_indices; }
};

#pragma endregion
#pragma region VAO
/*
OpenGL VAO abstraction
*/
class VertexArray : public Buffer
{
    unsigned int m_layoutCount;
    std::vector<VertexBuffer> m_VBOs;

public:
    VertexArray() : Buffer(), m_layoutCount(0) {}
    ~VertexArray();

    /*
    Generates VAO, all VBOs inside and uploads their thata to the GPU.
    */
    void generate();

    void bind() const;
    void unbind() const;

    /*
    Adds a VBO.
    */
    void push_vertex_buffer(const VertexBuffer vbo);

    void set_layout_divisor(const unsigned int divisor);

    inline unsigned int get_layout_count() const { return m_layoutCount; }

    inline const std::vector<VertexBuffer> get_vertex_buffers() const { return m_VBOs; }

    inline bool empty() const { return m_VBOs.empty(); }
};

#pragma endregion
#pragma region UBO
/*
OpenGL UBO abstraction
*/
class UniformBuffer : public Buffer
{

    unsigned int m_id;
    const size_t BYTES;

    struct Layout
    {
        size_t bytes;
        size_t offset;
    };

    std::vector<Layout>
        m_layouts;
    size_t m_layoutBinding;

    bool m_generated{false};

public:
    UniformBuffer(const size_t sizeInBytes, const size_t layoutBinding = 0) : Buffer(), BYTES(sizeInBytes), m_layouts({{sizeInBytes, 0}}), m_layoutBinding(layoutBinding){};

    ~UniformBuffer();

    void generate();

    void bind() const;

    void unbind() const;

    /**
     * Copy data to the GPU
     */
    void upload_data(const size_t sizeInBytes, const void *data, const size_t offset = 0) const;
};

GLSP_NAMESPACE_END

#endif
