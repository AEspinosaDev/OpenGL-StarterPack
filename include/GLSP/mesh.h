/*
    This file is part of OpenGL-StarterPack (GLSP), an open source OpenGL based framework
    that facilitates and speeds up demo and project creation by offering an abstraction to
    the basic objects of OpenGL as well as incluing the necessary libraries.

    MIT License

    Copyright (c) 2024 Antonio Espinosa Garcia

*/
#ifndef __MESH__
#define __MESH__

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <GLSP/object3D.h>
#include <GLSP/material.h>
#include <GLSP/utils.h>
#include <GLSP/buffers.h>

GLSP_NAMESPACE_BEGIN

/*
Canonical vertex definition. Most meshes will use some or all of these attributes.
*/
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec2 uv;
    glm::vec3 color;

    bool operator==(const Vertex &other) const
    {
        return position == other.position && normal == other.normal && tangent == other.tangent && uv == other.uv && color == other.color;
    }

    bool operator!=(const Vertex &other) const
    {
        return !(*this == other);
    }
};

/*
Class that defines the mesh geometry. Can be simply instanced by filling with a canonical vertex type array. 
It can also be instance using directly a custom VetexArray, letting the user have total freedom when defining the mesh vertex info and attribute layouts.
*/
class Geometry
{
protected:
    size_t m_vertexCount;
    VertexArray m_VAO;
    IndexBuffer m_IBO;

    unsigned int m_primitiveType;
    unsigned int m_vertexPerPatch; // In case of tesselation

    bool m_indexed;
    bool m_buffer_loaded{false};

public:
    /*
    Simplified constructor using the canonical vertex definition
    */
    Geometry(std::vector<Vertex> vertices, std::vector<unsigned int> indices, unsigned int primitive = GL_TRIANGLES);
    /*
    Low level constructor for directly handling vertex array structure and definition
    */
    Geometry(VertexArray VAO, size_t vertexCount, unsigned int primitive = GL_TRIANGLES) : m_VAO(VAO), m_IBO({}), m_vertexCount(vertexCount), m_primitiveType{primitive}, m_vertexPerPatch(4) {}
    /*
   Low level constructor for directly handling vertex array structure and definition plus index buffer
   */
    Geometry(VertexArray VAO, size_t vertexCount, IndexBuffer IBO, unsigned int primitive = GL_TRIANGLES) : m_VAO(VAO), m_IBO(IBO), m_vertexCount(vertexCount), m_primitiveType{primitive}, m_vertexPerPatch(4) {}

    /*
    Gets a read-only copy of the vertex array object. By accessing the vertex buffers inside it one can access the geometry vertex data
    */
    inline const VertexArray get_VAO() const { return m_VAO; }
    /*
    Gets a read-only copy of the index buffer object. One can accesss the indices in the mesh with this object. If geometry is not indexed, this object will be empty.
    */
    inline const IndexBuffer get_IBO() const { return m_IBO; }

    inline size_t get_vertex_count() const { return m_vertexCount; }

    inline void set_patch_vertex_number(unsigned int num) { m_vertexPerPatch = num; }
    inline unsigned int get_patch_vertex_number() const { return m_vertexPerPatch; }

    inline void set_primitive_type(unsigned int type) { m_primitiveType = type; }
    inline unsigned int get_primitive_type() const { return m_primitiveType; }

    inline bool is_indexed() const { return m_indexed; }

    virtual void generate_buffers();

    inline bool is_buffer_loaded() const { return m_buffer_loaded; }

    /**
     * Change GL_POINTS rasterization size for drawing
     */
    static void point_primitive_raster_size(float size)
    {
        GL_CHECK(glPointSize(size));
    }
    /**
     * Change GL_LINES rasterization width for drawing
     */
    static void line_primitive_raster_width(float width)
    {
        GL_CHECK(glLineWidth(width));
    }
};

/*
Class containing all data needed for drawing and manipulating with a 3D mesh.
*/
class Mesh : public Object3D
{
protected:
    Geometry *m_geometry;
    Material *m_material;

    static int INSTANCED_MESHES;

public:
    Mesh() : Object3D("Mesh", {0.0f, 0.0f, 0.0f}, Object3DType::MESH), m_material(nullptr) { Mesh::INSTANCED_MESHES++; }
    Mesh(Geometry *const geometry, Material *const material) : Object3D("Mesh", {0.0f, 0.0f, 0.0f}, Object3DType::MESH), m_material(material), m_geometry(geometry) { Mesh::INSTANCED_MESHES++; }
    ~Mesh()
    {
        delete m_geometry;
        delete m_material;
    }

    void set_geometry(Geometry *const g);

    inline Geometry *const get_geometry() const { return m_geometry; }

    inline void set_material(Material *const material) { m_material = material; }

    inline Material *const get_material() const { return m_material; }

    virtual void draw(bool useMaterial = true);

    inline static int get_number_of_instances() { return INSTANCED_MESHES; }

    /*
    Utility. Creates a screen quad for rendering textures onto the screen. Useful for postprocess and deferred screen space methods
    */
    static Mesh *create_screen_quad();

    /*
   Utility. Creates a quad primitive
   */
    static Mesh *create_quad(Extent2D subdivisions = {1, 1}, unsigned int primitiveType = GL_TRIANGLES);
};

GLSP_NAMESPACE_END

namespace std
{
    template <>
    struct hash<GLSP::Vertex>
    {
        size_t operator()(GLSP::Vertex const &vertex) const
        {
            size_t seed = 0;
            GLSP::utils::hash_combine(seed, vertex.position, vertex.normal, vertex.tangent, vertex.uv, vertex.color);
            return seed;
        }
    };
};
#endif
