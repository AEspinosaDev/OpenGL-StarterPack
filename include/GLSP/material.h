/*
    This file is part of OpenGL-StarterPack (GLSP), an open source OpenGL based framework
    that facilitates and speeds up demo and project creation by offering an abstraction to
    the basic objects of OpenGL as well as incluing the necessary libraries.

    MIT License

    Copyright (c) 2024 Antonio Espinosa Garcia

*/
#ifndef __MATERIAL__
#define __MATERIAL__

#include <unordered_map>
#include <GLSP/shader.h>
#include <GLSP/texture.h>
#include <GLSP/renderer.h>

GLSP_NAMESPACE_BEGIN

struct PipelineState
{
    bool cullFace{false};
    bool depthWrites{true};
    bool depthTest{true};
    DepthFuncType depthFunction{LESS};
    bool blending{false};
    BlendFuncType blendingFuncSRC{SRC_ALPHA};
    BlendFuncType blendingFuncDST{ONE_MINUS_SRC_A};
    BlendOperationType blendingOperation{ADD};
    bool alphaToCoverage{false};
    bool alphaTest{false};
};

struct GraphicPipeline
{
    Shader *shader{nullptr};
    PipelineState state{};
};
struct MaterialUniforms
{
    std::unordered_map<std::string, glm::vec3> vec3Types;
    std::unordered_map<std::string, glm::vec4> vec4Types;
    std::unordered_map<std::string, glm::vec2> vec2Types;
    std::unordered_map<std::string, float> floatTypes;
    std::unordered_map<std::string, int> intTypes;
    std::unordered_map<std::string, bool> boolTypes;
    std::unordered_map<std::string, glm::mat4> mat4Types;
};

/*
Base class for everything related to a graphic pipeline (shader activation and setup, OpenGL state and uniforms related to a certain concept).
It should be inherited and extended upon.
*/
class Material
{
protected:
    GraphicPipeline m_pipeline;
    MaterialUniforms m_uniforms;

    struct TextureData
    {
        Texture *texture;
        unsigned int slot;
        std::string uniformName;
    };

    std::unordered_map<unsigned int, TextureData> m_textures;

    /*
    Should be inherited and overriden for uploading just the necessary material uniforms. Base implementation just uploads everything inside a generic MaterialUniforms struct.
    */
    virtual void upload_uniforms() const;

    virtual void setup_pipeline() const;

    virtual void bind_textures() const;

    virtual void unbind_textures() const;

public:
    Material(GraphicPipeline &pipeline) : m_pipeline(pipeline) {}
    Material(GraphicPipeline &pipeline, MaterialUniforms &uniforms) : m_pipeline(pipeline), m_uniforms(uniforms) {}

    inline virtual void set_texture(std::string uniformName, Texture *texture, unsigned int slot = 0)
    {
        m_pipeline.shader->bind();
        m_pipeline.shader->set_int(uniformName.c_str(), slot);
        m_pipeline.shader->unbind();
        m_textures[slot] = {texture, slot, uniformName};
    };

    inline virtual Texture *get_texture(unsigned int slot) { return m_textures[slot].texture; };

    inline virtual void set_uniforms(MaterialUniforms &uniforms) { m_uniforms = uniforms; }
    inline virtual MaterialUniforms get_uniforms() const { return m_uniforms; }

    inline virtual void set_pipeline(GraphicPipeline &pipeline) { m_pipeline = pipeline; }
    inline virtual GraphicPipeline get_pipeline() const { return m_pipeline; }

    /*
    Binds the material. Binds the shader, sets up the render state ,uploads uniforms and activates textures
    */
    virtual void bind() const;
    /*
   Unbinds the material
   */
    virtual void unbind() const;
};


GLSP_NAMESPACE_END
#endif