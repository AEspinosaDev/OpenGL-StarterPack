/*
    This file is part of OpenGL-StarterPack (GLSP), an open source OpenGL based framework
    that facilitates and speeds up demo and project creation by offering an abstraction to
    the basic objects of OpenGL as well as incluing the necessary libraries.

    MIT License

    Copyright (c) 2024 Antonio Espinosa Garcia

*/
#include <GLSP/mesh.h>

GLSP_NAMESPACE_BEGIN

Geometry::Geometry(std::vector<Vertex> vertices, std::vector<unsigned int> indices, unsigned int primitive) : m_VAO(), m_IBO(indices), m_vertexCount(vertices.size()), m_primitiveType{primitive}, m_vertexPerPatch(4)
{
    // ATTRIBUTE LAYOUT SETUP
    // Interleaved
    //    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, vertexSize * m_geometry.vertices.size(), m_geometry.vertices.data(), GL_STATIC_DRAW));
    VertexBuffer VBO(vertices.data(), vertices.size() * sizeof(Vertex));
    VBO.push_attribute_layout<float>(3);
    VBO.push_attribute_layout<float>(3);
    VBO.push_attribute_layout<float>(3);
    VBO.push_attribute_layout<float>(2);
    VBO.push_attribute_layout<float>(3);
    m_VAO.push_vertex_buffer(VBO);
}

void Geometry::generate_buffers()
{
    m_VAO.generate();
    // Manage indices
    if (!m_IBO.empty())
    {
        m_VAO.bind();
        m_IBO.generate();
        m_IBO.upload_data();
        m_VAO.unbind();

        m_indexed = true;
    }
    else
        m_indexed = false;

    m_VAO.bind();
    if (m_primitiveType == GL_PATCHES)
        glPatchParameteri(GL_PATCH_VERTICES, m_vertexPerPatch);
    m_VAO.unbind();

    m_buffer_loaded = true;
}

int Mesh::INSTANCED_MESHES = 0;

void Mesh::set_geometry(Geometry *const g)
{
    m_geometry = g;
}

void Mesh::draw(bool useMaterial)
{
    if (m_enabled && m_geometry->is_buffer_loaded())
    {
        if (m_material && useMaterial)
            m_material->bind();

        m_geometry->get_VAO().bind();

        if (m_geometry->is_indexed() && m_geometry->get_primitive_type() != GL_PATCHES)
        {
            GL_CHECK(glDrawElements(m_geometry->get_primitive_type(), m_geometry->get_IBO().get_indices().size(), GL_UNSIGNED_INT, (void *)0));
        }
        else
        {

            GL_CHECK(glDrawArrays(m_geometry->get_primitive_type(), 0, m_geometry->get_vertex_count()));
        }

        m_geometry->get_VAO().unbind();

        if (m_material && useMaterial)
            m_material->unbind();
    }
    else
        m_geometry->generate_buffers();
}

Mesh *Mesh::create_screen_quad()
{
    Mesh *screen = new Mesh();

    Geometry *geometry = new Geometry({{{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
                                       {{1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}},
                                       {{-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}},
                                       {{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}}},
                                      {0, 1, 2, 1, 3, 2});
    screen->set_geometry(geometry);
    return screen;
}

Mesh *Mesh::create_quad(Extent2D subdivisions, unsigned int primitiveType)
{
    Mesh *quad = new Mesh();
    std::vector<Vertex> vertices;
    vertices.reserve(subdivisions.width * subdivisions.height);

    for (unsigned i = 0; i <= subdivisions.width - 1; i++)
    {
        for (unsigned j = 0; j <= subdivisions.height - 1; j++)
        {

            vertices.push_back({{-0.5f + 1.0f * i / (float)subdivisions.width,
                                 0.0f,
                                 -0.5f + 1.0f * j / (float)subdivisions.height},
                                {0.0f, 1.0f, 0.0f},
                                {0.0f, 0.0f, 1.0f},
                                {i / (float)subdivisions.width, j / (float)subdivisions.height},
                                {1.0f, 0.0f, 0.0f}});
            vertices.push_back({{-0.5f + 1.0f * (i + 1) / (float)subdivisions.width,
                                 0.0f,
                                 -0.5f + 1.0f * j / (float)subdivisions.height},
                                {0.0f, 1.0f, 0.0f},
                                {0.0f, 0.0f, 1.0f},
                                {(i + 1) / (float)subdivisions.width, j / (float)subdivisions.height},
                                {1.0f, 0.0f, 0.0f}});
            vertices.push_back({{-0.5f + 1.0f * i / (float)subdivisions.width,
                                 0.0f,
                                 -0.5f + 1.0f * (j + 1) / (float)subdivisions.height},
                                {0.0f, 1.0f, 0.0f},
                                {0.0f, 0.0f, 1.0f},
                                {i / (float)subdivisions.width, (j + 1) / (float)subdivisions.height},
                                {1.0f, 0.0f, 0.0f}});
            vertices.push_back({{-0.5f + 1.0f * (i + 1) / (float)subdivisions.width,
                                 0.0f,
                                 -0.5f + 1.0f * (j + 1) / (float)subdivisions.height},
                                {0.0f, 1.0f, 0.0f},
                                {0.0f, 0.0f, 1.0f},
                                {(i + 1) / (float)subdivisions.width, (j + 1) / (float)subdivisions.height},
                                {1.0f, 0.0f, 0.0f}});
        }
    }

    Geometry *geometry = new Geometry(vertices, {}, primitiveType);
    quad->set_geometry(geometry);
    return quad;
}

GLSP_NAMESPACE_END
